// MBLoopa Core Logic
// (c) Hawkeye 2015
//
// This unit provides the looper core data structures and the menu navigation
//
// Multi-Clip logic and memory layout:
//
// When recording clips, these are directly stored to a MIDI file, we cannot loop record (so what)
// After recording a clip, it will be instantly re-scanned from SD to gather key informations,
// like tick length, noteroll information, and so on.
//
// We cannot store all sequencer notes in memory, but we can pre-fetch notes for every midi-file
// from SD-Card, so that we might be able to play eight tracks (unmuted) with no sync issues.
// Track looping can be implemented likewise - if our SEQ Handler detects, that we are close
// to the end of a clip, and that clip is set to loop, we can resupply the seq_midi_out buffers...

#include <mios32.h>
#include <mid_parser.h>
#include <FreeRTOS.h>
#include <seq_bpm.h>
#include <string.h>

#include "tasks.h"
#include "file.h"
#include "loopa.h"
#include "seq.h"
#include "mid_file.h"
#include "hardware.h"
#include "screen.h"
#include "voxelspace.h"


// --  Local types ---

typedef struct
{
   u32  initial_file_pos;
   u32  initial_tick;
   u32  file_pos;
   u32  chunk_end;
   u32  tick;
   u8   running_status;
} midi_track_t;


// --- Global vars ---

u8 baseView_ = 0;               // if not in baseView, we are in single clipView
u8 displayMode2d_ = 0;          // if not in 2d display mode, we are rendering voxelspace
u8 selectedClipNumber_ = 0;     // currently active or last active clip number (1-8)
u16 sessionNumber_ = 0;         // currently active session number (directory e.g. /SESSIONS/0001)

static u32 clipFilePos_[8];     // file position within midi file (for 8 clips)
static u32 clipFileLen_[8];     // file length of midi file (for 8 clips)
u8 clipFileAvailable_[8];       // true, if clip midi file is available (for 8 clips)
static file_t clipFileFi_[8];   // clip file reference handles, only valid if clipFileAvailable is true (for 8 clips)
u16 clipMidiFileFormat_[8];     // clip midi file format (for 8 clips)
u16 clipMidiFilePPQN_[8];       // pulses per quarter note (for 8 clips)
midi_track_t clipMidiTrack_[8]; // max one midi track per clip mid file supported (for 8 clips)

u32 clipEvents_[8];             // number of events in the respective clip
u32 clipTicks_[8];              // number of available ticks in the respective clip
static s32 (*clipPlayEventCallback)(u8 clipNumber, mios32_midi_package_t midi_package, u32 tick) = 0; // fetchClipEvents() callback


/**
 * Help function: convert tick number to step number
 * @return step
 *
 */
u16 tickToStep(u32 tick)
{
   return tick / (SEQ_BPM_PPQN_Get() / 16);
}
// -------------------------------------------------------------------------------------------------


/**
 * Reads <len> bytes from the .mid file into <buffer>
 * @return number of read bytes
 *
 */
u32 clipFileRead(u8 clipNumber, void *buffer, u32 len)
{
   s32 status;

   if (!clipFileAvailable_[clipNumber])
      return FILE_ERR_NO_FILE;

   MUTEX_SDCARD_TAKE;
   if ((status=FILE_ReadReOpen(&clipFileFi_[clipNumber])) >= 0)
   {
      status = FILE_ReadBuffer(buffer, len);
      FILE_ReadClose(&clipFileFi_[clipNumber]);
   }
   MUTEX_SDCARD_GIVE;

   return (status >= 0) ? len : 0;
}
// -------------------------------------------------------------------------------------------------


/**
 * @return 1, if end of file reached
 *
 */
s32 clipFileEOF(u8 clipNumber)
{
   if (clipFilePos_[clipNumber] >= clipFileLen_[clipNumber] || !FILE_SDCardAvailable())
      return 1; // end of file reached or SD card disconnected

   return 0;
}
// -------------------------------------------------------------------------------------------------


/**
 * Sets file pointer to a specific position
 * @return -1, if end of file reached
 *
 */
s32 clipFileSeek(u8 clipNumber, u32 pos)
{
   s32 status;

   if (!clipFileAvailable_[clipNumber])
      return -1; // end of file reached

   clipFilePos_[clipNumber] = pos;

   MUTEX_SDCARD_TAKE;

   if (clipFilePos_[clipNumber] >= clipFileLen_[clipNumber])
      status = -1; // end of file reached
   else
   {
      if ((status=FILE_ReadReOpen(&clipFileFi_[clipNumber])) >= 0)
      {
         status = FILE_ReadSeek(pos);
         FILE_ReadClose(&clipFileFi_[clipNumber]);
      }
   }

   MUTEX_SDCARD_GIVE;

   return status;
}
// -------------------------------------------------------------------------------------------------


/**
 * Help function: reads a byte/hword/word from the .mid file
 *
 */
static u32 clipReadWord(u8 clipNumber, u8 len)
{
   int word = 0;
   int i;

   for (i=0; i<len; ++i)
   {
      // due to unknown endianess of the host processor, we have to read byte by byte!
      u8 byte;
      clipFileRead(clipNumber, &byte, 1);
      word = (word << 8) | byte;
   }

   return word;
}
// -------------------------------------------------------------------------------------------------


/**
 * Help function: reads a variable-length number from the .mid file
 * based on code example in MIDI file spec
 *
 */
static u32 clipReadVarLen(u8 clipNumber, u32* pos)
{
   u32 value;
   u8 c;

   *pos += clipFileRead(clipNumber, &c, 1);
   if ((value = c) & 0x80)
   {
      value &= 0x7f;

      do
      {
         *pos += clipFileRead(clipNumber, &c, 1);
         value = (value << 7) | (c & 0x7f);
      } while (c & 0x80);
   }

   return value;
}
// -------------------------------------------------------------------------------------------------


/**
 * Open a clip .mid file (associated with current sessionNumber_ and given clip number)
 * and parses for available header/track chunks
 * @return 0, if no errors occured
 *
 */
s32 clipFileOpen(u8 clipNumber)
{
   // invalidate current file
   clipFileAvailable_[clipNumber] = 0;

   static char filepath[48];
   sprintf(filepath, "/SESSIONS/%04d/CLIP%d.MID", sessionNumber_, clipNumber);

   MUTEX_SDCARD_TAKE;
   s32 status = FILE_ReadOpen(&clipFileFi_[clipNumber], filepath);
   FILE_ReadClose(&clipFileFi_[clipNumber]); // close again - file will be reopened by read handler
   MUTEX_SDCARD_GIVE;

   if (status != 0)
   {
      DEBUG_MSG("[clipFileOpen] failed to open file, status: %d\n", status);
      return status;
   }

   // File is available

   clipFilePos_[clipNumber] = 0;
   clipFileLen_[clipNumber] = clipFileFi_[clipNumber].fsize;
   clipFileAvailable_[clipNumber] = 1;

   DEBUG_MSG("[clipFileOpen] opened '%s' of length %u\n", filepath, clipFileLen_[clipNumber]);

   // Parse header...
   u8 chunk_type[4];
   u32 chunk_len;

   DEBUG_MSG("[clipFileOpen] reading file\n\r");

   // reset file position
   clipFileSeek(clipNumber, 0);
   u32 file_pos = 0;

   u16 num_tracks = 0;

   // read chunks
   while (!clipFileEOF(clipNumber))
   {
      file_pos += clipFileRead(clipNumber, chunk_type, 4);

      if (clipFileEOF(clipNumber))
         break; // unexpected: end of file reached

      chunk_len = clipReadWord(clipNumber, 4);
      file_pos += 4;

      if (clipFileEOF(clipNumber))
         break; // unexpected: end of file reached

      if (memcmp(chunk_type, "MThd", 4) == 0)
      {
         DEBUG_MSG("[clipFileOpen] Found Header with size: %u\n\r", chunk_len);
         if( chunk_len != 6 )
         {
            DEBUG_MSG("[clipFileOpen] invalid header size - skip!\n\r");
         }
         else
         {
            clipMidiFileFormat_[clipNumber] = (u16)clipReadWord(clipNumber, 2);
            u16 tempTracksNum = (u16)clipReadWord(clipNumber, 2);
            clipMidiFilePPQN_[clipNumber] = (u16)clipReadWord(clipNumber, 2);
            file_pos += 6;
            DEBUG_MSG("[clipFileOpen] MIDI file format: %u\n\r", clipMidiFileFormat_[clipNumber]);
            DEBUG_MSG("[clipFileOpen] Number of tracks: %u\n\r", tempTracksNum);
            DEBUG_MSG("[clipFileOpen] ppqn (n per quarter note): %u\n\r", clipMidiFilePPQN_[clipNumber]);
         }
      }
      else if (memcmp(chunk_type, "MTrk", 4) == 0)
      {
         if (num_tracks >= 1)
         {
            DEBUG_MSG("[clipFileOpen] Found Track with size: %u must be ignored due to MID_PARSER_MAX_TRACKS\n\r", chunk_len);
         }
         else
         {
            u32 num_bytes = 0;
            u32 delta = (u32)clipReadVarLen(clipNumber, &num_bytes);
            file_pos += num_bytes;
            chunk_len -= num_bytes;

            midi_track_t *mt = &clipMidiTrack_[clipNumber];
            mt->initial_file_pos = file_pos;
            mt->initial_tick = delta;
            mt->file_pos = file_pos;
            mt->chunk_end = file_pos + chunk_len - 1;
            mt->tick = delta;
            mt->running_status = 0x80;
            ++num_tracks;

            DEBUG_MSG("[clipFileOpen] Found Track %d with size: %u, starting at tick: %u\n\r", num_tracks, chunk_len + num_bytes, mt->tick);
         }

         // switch to next track
         file_pos += chunk_len;
         clipFileSeek(clipNumber, file_pos);
      }
      else
      {
         DEBUG_MSG("[clipFileOpen] Found unknown chunk '%c%c%c%c' of size %u\n\r",
                   chunk_type[0], chunk_type[1], chunk_type[2], chunk_type[3],
                   chunk_len);

         if (num_tracks >= 1)
         {
            // Found at least one track, all is good
            break;
         }
         else
         {
            return -1;
         }
      }
   }

   clipFileAvailable_[clipNumber] = 1;



   return 0; // no error
}
// -------------------------------------------------------------------------------------------------



/**
 * Callback method to count a loaded play event in memory.
 * Triggered by MID_PARSER_FetchEvents during loadClip
 *
 */
static s32 countPlayEvent(u8 clipNumber, mios32_midi_package_t midi_package, u32 tick)
{
   clipEvents_[clipNumber]++;

   if (tick > clipTicks_[clipNumber])
      clipTicks_[clipNumber] = tick;

   return 0;
}
// -------------------------------------------------------------------------------------------------


/**
 * Prefetches MIDI events from the given clip number file for a given number of MIDI ticks
 *
 *
 * returns < 0 on errors
 * returns > 0 if tracks are still playing
 * returns 0 if song is finished
 *
 */
s32 clipFetchEvents(u8 clipNumber, u32 tick_offset, u32 num_ticks)
{
   if (clipFileAvailable_ == 0)
      return 1; // fake for compatibility reasons

   midi_track_t *mt = &clipMidiTrack_[clipNumber];

   while (mt->file_pos < mt->chunk_end)
   {
      // exit if next tick is not within given timeframe
      if (mt->tick >= (tick_offset + num_ticks))
         break;

      // set file pos
      clipFileSeek(clipNumber, mt->file_pos);

      // get event
      u8 event;
      mt->file_pos += clipFileRead(clipNumber, &event, 1);

      if (event == 0xf0)
      {
         // SysEx event
         u32 length = (u32)clipReadVarLen(clipNumber, &mt->file_pos);
         #if DEBUG_VERBOSE_LEVEL >= 3
         DEBUG_MSG("[MID_PARSER:%d:%u] SysEx event with %u bytes\n\r", track, mt->tick, length);
         #endif

         // TODO: use optimized packages for SysEx!
         mios32_midi_package_t midi_package;
         midi_package.type = 0xf; // single bytes will be transmitted

         // initial 0xf0
         midi_package.evnt0 = 0xf0;
         if (clipPlayEventCallback != NULL)
            clipPlayEventCallback(clipNumber, midi_package, mt->tick);

         // remaining bytes
         int i;
         for (i=0; i<length; ++i)
         {
            u8 evnt0;
            mt->file_pos += clipFileRead(clipNumber, &evnt0, 1);
            midi_package.evnt0 = evnt0;
            if (clipPlayEventCallback != NULL)
               clipPlayEventCallback(clipNumber, midi_package, mt->tick);
         }
      }
      else if (event == 0xf7)
      {
         // "Escaped" event (allows to send any MIDI data)
         u32 length = (u32)clipReadVarLen(clipNumber, &mt->file_pos);
         #if DEBUG_VERBOSE_LEVEL >= 3
         DEBUG_MSG("[MID_PARSER:%d:%u] Escaped event with %u bytes\n\r", track, mt->tick, length);
         #endif

         mios32_midi_package_t midi_package;
         midi_package.type = 0xf; // single bytes will be transmitted
         int i;
         for (i=0; i<length; ++i)
         {
            u8 evnt0;
            mt->file_pos += clipFileRead(clipNumber, &evnt0, 1);
            midi_package.evnt0 = evnt0;
            if (clipPlayEventCallback != NULL)
               clipPlayEventCallback(clipNumber, midi_package, mt->tick);
         }
      }
      else if (event == 0xff)
      {
         // Meta Event
         u8 meta;
         mt->file_pos += clipFileRead(clipNumber, &meta, 1);
         clipReadVarLen(clipNumber, &mt->file_pos);

         // We ignore meta events for now
      }
      else
      {
         // common MIDI event
         mios32_midi_package_t midi_package;

         if (event & 0x80)
         {
            mt->running_status = event;
            midi_package.evnt0 = event;
            u8 evnt1;
            mt->file_pos += clipFileRead(clipNumber, &evnt1, 1);
            midi_package.evnt1 = evnt1;
         }
         else
         {
            midi_package.evnt0 = mt->running_status;
            midi_package.evnt1 = event;
         }
         midi_package.type = midi_package.event;

         switch (midi_package.event)
         {
         case NoteOff:
         case NoteOn:
         case PolyPressure:
         case CC:
         case PitchBend:
            {
               u8 evnt2;
               mt->file_pos += clipFileRead(clipNumber, &evnt2, 1);
               midi_package.evnt2 = evnt2;

               if (clipPlayEventCallback != NULL)
                  clipPlayEventCallback(clipNumber, midi_package, mt->tick);

               #if DEBUG_VERBOSE_LEVEL >= 3
               DEBUG_MSG("[MID_PARSER:%d:%u] %02x%02x%02x\n\r", track, mt->tick, midi_package.evnt0, midi_package.evnt1, midi_package.evnt2);
               #endif
            }
            break;
         case ProgramChange:
         case Aftertouch:
            if (clipPlayEventCallback != NULL)
               clipPlayEventCallback(clipNumber, midi_package, mt->tick);

            #if DEBUG_VERBOSE_LEVEL >= 3
            DEBUG_MSG("[MID_PARSER:%d:%u] %02x%02x\n\r", track, mt->tick, midi_package.evnt0, midi_package.evnt1);
            #endif
            break;

         default:
            #if DEBUG_VERBOSE_LEVEL >= 1
            DEBUG_MSG("[MID_PARSER:%d:%u] ooops? got 0xf0 status in MIDI event stream!\n\r", track, mt->tick);
            #endif
            break;
         }
      }

      // get delta length to next event if end of track hasn't been reached yet
      if (mt->file_pos < mt->chunk_end)
      {
         u32 delta = (u32)clipReadVarLen(clipNumber, &mt->file_pos);
         mt->tick += delta;
      }
   }

   return 1;
}
// -------------------------------------------------------------------------------------------------


/**
 * Scan a clip and read the number of events and midi ticks contained
 *
 */
void clipScan(u8 clipNumber)
{
   clipEvents_[clipNumber] = 0;
   clipTicks_[clipNumber] = 0;

   // Install "count-only" callback
   clipPlayEventCallback = countPlayEvent;

   clipFetchEvents(clipNumber, 0, 0xFFFFFFF);
}
// -------------------------------------------------------------------------------------------------


/**
 * (Re)load a session clip. It might have changed after recording
 * If no clip was found, the local clipData will be empty
 *
 */
void loadClip(u8 clipNumber)
{
   sprintf(screenMode, "Loading");
   sprintf(screenFile, "CLIP%d.MID", clipNumber);

   clipFileOpen(clipNumber);
   clipScan(clipNumber); // XXX: Overhead! Necessary only, if we have no clip information in our session.txt

   // Goal: After this routine, the clip should be ready to play in the
   //       loopa.c based sequencer code, muted or not, it should loop on demand

   screenSetClipSelected(1);
   screenSetClipLooped(clipNumber, clipNumber % 2);
   screenSetClipPosition(clipNumber, 0, tickToStep(clipTicks_[clipNumber]));

   DEBUG_MSG("loadClip(): counted %d events and %d ticks for clip %d.", clipEvents_[clipNumber], clipTicks_[clipNumber], clipNumber);
}
// -------------------------------------------------------------------------------------------------


/**
 * Load new session data
 * also load all stored clips into memory
 *
 */
void loadSession(u16 newSessionNumber)
{
   sessionNumber_ = newSessionNumber;

   int clip;
   for (clip = 0; clip < 8; clip++)
   {
      loadClip(clip);
   }
}
// -------------------------------------------------------------------------------------------------


/**
 * First callback from app - render Loopa Startup logo on screen
 *
 */
void loopaStartup()
{
   screenShowLoopaLogo(1);
}
// -------------------------------------------------------------------------------------------------


/**
 * SD Card Available, initialize LOOPA, load session, prepare screen and menus
 *
 */
void loopaSDCardAvailable()
{
   loadSession(1); // -> XXX: load latest session with a free clip slot instead
   screenShowLoopaLogo(0); // Disable logo, normal operations started
}
// -------------------------------------------------------------------------------------------------


/**
 * A buttonpress has occured
 *
 */
void loopaButtonPressed(s32 pin)
{
   if (pin == sw_startstop)
   {
      voxelClearNotes();
      MIOS32_MIDI_SendDebugMessage("Button: start/stop\n");
      SEQ_PlayStopButton();
      MIOS32_DOUT_PinSet(led_startstop, 1);
   }

   if (pin == sw_armrecord)
   {
      voxelClearNotes();
      voxelClearField();
      MIOS32_MIDI_SendDebugMessage("Button: arm for record\n");
      SEQ_RecStopButton();
      MIOS32_DOUT_PinSet(led_armrecord, 1);
   }

   if (pin == sw_looprange)
   {
      MIOS32_DOUT_PinSet(led_looprange, 1);

      voxelClearNotes();
      voxelClearField();
      MIOS32_MIDI_SendDebugMessage("Button: loop range\n");
      SEQ_PlayNextFile(-1);
   }

   if (pin == sw_editclip)
   {
      MIOS32_DOUT_PinSet(led_editclip, 1);

      voxelClearNotes();
      voxelClearField();
      MIOS32_MIDI_SendDebugMessage("Button: edit clip\n");
      SEQ_PlayNextFile(1);
   }
}
// -------------------------------------------------------------------------------------------------


/**
 * An encoder has been turned
 *
 */
void loopaEncoderTurned(s32 encoder, s32 incrementer)
{
   if (encoder == enc_clipswitch)
   {
      selectedClipNumber_ += incrementer;
      selectedClipNumber_ %= 8;
      screenSetClipSelected(selectedClipNumber_);
   }
}
// -------------------------------------------------------------------------------------------------
