// $Id$
/*
 * Manual Trigger Page
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>
#include "seq_lcd.h"
#include "seq_ui.h"

#include "seq_core.h"
#include "seq_trg.h"


/////////////////////////////////////////////////////////////////////////////
// Local LED handler function
/////////////////////////////////////////////////////////////////////////////
static s32 LED_Handler(u16 *gp_leds)
{
  // branch to edit page if requested
  if( ui_hold_msg_ctr )
    return SEQ_UI_EDIT_LED_Handler(gp_leds);

  if( ui_cursor_flash ) // if flashing flag active: no LED flag set
    return 0;

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Local encoder callback function
// Should return:
//   1 if value has been changed
//   0 if value hasn't been changed
//  -1 if invalid or unsupported encoder
/////////////////////////////////////////////////////////////////////////////
static s32 Encoder_Handler(seq_ui_encoder_t encoder, s32 incrementer)
{
  // ensure that original screen will be print immediately
  ui_hold_msg_ctr = 0;

#if 0
  // leads to: comparison is always true due to limited range of data type
  if( (encoder >= SEQ_UI_ENCODER_GP1 && encoder <= SEQ_UI_ENCODER_GP16) ) {
#else
  if( encoder <= SEQ_UI_ENCODER_GP16 ) {
#endif
    // select step within view
    ui_selected_step = (ui_selected_step_view << 4) | (u8)encoder;

    // show edit page for 2 seconds
    ui_hold_msg_ctr = 2000;

    // forward manual trigger to SEQ_CORE
    SEQ_CORE_ManualTrigger(ui_selected_step);

    return 1; // value changed
  }

  return -1; // invalid or unsupported encoder
}


/////////////////////////////////////////////////////////////////////////////
// Local button callback function
// Should return:
//   1 if value has been changed
//   0 if value hasn't been changed
//  -1 if invalid or unsupported button
/////////////////////////////////////////////////////////////////////////////
static s32 Button_Handler(seq_ui_button_t button, s32 depressed)
{
  if( depressed ) return 0; // ignore when button depressed

  // ensure that original screen will be print immediately
  ui_hold_msg_ctr = 0;

#if 0
  // leads to: comparison is always true due to limited range of data type
  if( button >= SEQ_UI_BUTTON_GP1 && button <= SEQ_UI_BUTTON_GP16 ) {
#else
  if( button <= SEQ_UI_BUTTON_GP16 ) {
#endif
    // -> same handling like for encoders
    return Encoder_Handler(button, 0);
  }

  switch( button ) {
    case SEQ_UI_BUTTON_Select: {
      // Request synch to measure for all selected tracks
      SEQ_CORE_ManualSynchToMeasure(ui_selected_tracks);
      return 1;
    } break;

    case SEQ_UI_BUTTON_Right:
    case SEQ_UI_BUTTON_Up:
      if( depressed ) return 0; // ignore when button depressed
      return Encoder_Handler(SEQ_UI_ENCODER_Datawheel, 1);

    case SEQ_UI_BUTTON_Left:
    case SEQ_UI_BUTTON_Down:
      if( depressed ) return 0; // ignore when button depressed
      return Encoder_Handler(SEQ_UI_ENCODER_Datawheel, -1);
  }

  return -1; // invalid or unsupported button
}


/////////////////////////////////////////////////////////////////////////////
// Local Display Handler function
// IN: <high_prio>: if set, a high-priority LCD update is requested
/////////////////////////////////////////////////////////////////////////////
static s32 LCD_Handler(u8 high_prio)
{
  // branch to edit page if requested
  if( ui_hold_msg_ctr )
    return SEQ_UI_EDIT_LCD_Handler(high_prio, SEQ_UI_EDIT_MODE_MANUAL);


  // layout:
  // 00000000001111111111222222222233333333330000000000111111111122222222223333333333
  // 01234567890123456789012345678901234567890123456789012345678901234567890123456789
  // <--------------------------------------><-------------------------------------->
  //   Selected Tracks: 1234 1234 1234 1234     Press SELECT to Synch to Measure!    
  // lower line: print step 1..16/17..32/...

  ///////////////////////////////////////////////////////////////////////////
  SEQ_LCD_CursorSet(0, 0);
  SEQ_LCD_PrintString("  Selected Tracks: ");

  u8 track;
  for(track=0; track<16; ++track) {
    if( seq_core_trk[track].state.SYNC_MEASURE )
      SEQ_LCD_PrintChar('S');
    else if( SEQ_UI_IsSelectedTrack(track) )
      SEQ_LCD_PrintChar('*');
    else
      SEQ_LCD_PrintChar('1' + (track&3));

    if( (track&3) == 3 )
      SEQ_LCD_PrintChar(' ');
  }
  SEQ_LCD_PrintSpaces(1);

  SEQ_LCD_PrintString("   Press SELECT to Synch to Measure!    ");


  ///////////////////////////////////////////////////////////////////////////
  SEQ_LCD_CursorSet(0, 1);

  u8 step;
  for(step=0; step<16; ++step)
    SEQ_LCD_PrintFormattedString(" %2d  ", 16*ui_selected_step_view + step + 1);

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_UI_MANUAL_Init(u32 mode)
{
  // install callback routines
  SEQ_UI_InstallButtonCallback(Button_Handler);
  SEQ_UI_InstallEncoderCallback(Encoder_Handler);
  SEQ_UI_InstallLEDCallback(LED_Handler);
  SEQ_UI_InstallLCDCallback(LCD_Handler);

  return 0; // no error
}
