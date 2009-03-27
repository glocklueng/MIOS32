/* $Id$ *//*vX32 pre-alphanot for any use whatsoevercopyright stryd_onebite me corp 2008big props to nILS for being my fourth eye and TK for obvious reasons
stay tuned for UI prototyping courtesy of lucem!
*//////////////////////////////////////////////////////////////////////////////// Include files/////////////////////////////////////////////////////////////////////////////#include <FreeRTOS.h>#include <portmacro.h>#include <mios32.h>
#include "tasks.h"
#include "app.h"#include "graph.h"#include "mclock.h"#include "modules.h"#include "patterns.h"#include "utils.h"#include "ui.h"#include <seq_midi_out.h>/////////////////////////////////////////////////////////////////////////////// Global Variables/////////////////////////////////////////////////////////////////////////////const mod_senddata_t mod_SendData_Type[MAX_BUFFERTYPES] = {    &Mod_Send_MIDI, 4,                                                          // names of functions to send outbuffers according to type    &Mod_Send_COM, 1,                                                           // and size of those buffers (in bytes)    &Mod_Send_DUMMY, 0                                                          // must be MAX_BUFFERTYPES of elements};void (*mod_Send_Type[MAX_MODULETYPES]) (unsigned char nodeID);                  // array of buffer send functions for module types/////////////////////////////////////////////////////////////////////////////// Global prototypes//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// sends the buffers with the appropriate sender function/////////////////////////////////////////////////////////////////////////////void Mod_Send_Buffer(unsigned char nodeID) {    if (mod_Send_Type[(node[nodeID].moduletype)] != NULL) {        mod_Send_Type[(node[nodeID].moduletype)](nodeID);           // send buffers according to the moduletype    }    }/////////////////////////////////////////////////////////////////////////////// sends the buffers by MIDI/////////////////////////////////////////////////////////////////////////////void Mod_Send_MIDI(unsigned char nodeID) {    unsigned char i;    unsigned char count = (node[nodeID].outbuffer_req);    unsigned char size = (node[nodeID].outbuffer_size);            for (i = 0; i < count; (i += size)) {                unsigned int length;        length = (node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_LENH]) << 8;        length |= (node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_LENL]);                node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_LENH] = DEAD_VALUE;        node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_LENL] = DEAD_VALUE;                if ((node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_CHAN] != DEAD_VALUE) &&           // Paranoia            (node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_NOTE] != DEAD_VALUE) &&            (node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_VEL] != DEAD_VALUE) &&            length > 0) {                        u8 chan = (node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_CHAN])%16;            node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_CHAN] = DEAD_VALUE;                        u8 note = util_s8tou7(node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_NOTE]);            node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_NOTE] = DEAD_VALUE;                        u8 velocity = util_s8tou7(node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_VEL]);            node[nodeID].outbuffer[i+MOD_SEND_MIDI_NOTE0_VEL] = DEAD_VALUE;                        if( length ) {                                                      // put note into queue if length is not 0 (this needs to go)                mios32_midi_package_t midi_package;                                midi_package.type     = NoteOn;                                 // package type must match with event!                midi_package.event    = NoteOn;                midi_package.chn      = chan;                midi_package.note     = note;                midi_package.velocity = velocity;                                SEQ_MIDI_OUT_Send(USB0, midi_package, SEQ_MIDI_OUT_OnOffEvent, mod_Tick_Timestamp, length);                            }                    }            }        (node[nodeID].outbuffer_req) = 0;}/////////////////////////////////////////////////////////////////////////////// sends the buffers by COM (serial)/////////////////////////////////////////////////////////////////////////////void Mod_Send_COM(unsigned char nodeID) {    unsigned char i;    unsigned char count = (node[nodeID].outbuffer_req);    unsigned char size = (node[nodeID].outbuffer_size);        for (i = 0; i < count; i += size) {        if (node[nodeID].outbuffer[i] != DEAD_VALUE) {            node[nodeID].outbuffer[i] = DEAD_VALUE;            (node[nodeID].outbuffer_req)--;        }            }    }/////////////////////////////////////////////////////////////////////////////// sends the buffers by nothing - it's a dummy/////////////////////////////////////////////////////////////////////////////void Mod_Send_DUMMY(unsigned char nodeID) {    unsigned char i;    unsigned char count = (node[nodeID].outbuffer_req);    unsigned char size = (node[nodeID].outbuffer_size);        for (i = 0; i < count; i += size) {        if (node[nodeID].outbuffer[i] != DEAD_VALUE) {            // it's a dummy, dummy             // MIOS32_COM_SendChar(COM_USB0, util_s8tou7(node[nodeID].outbuffer[i]));            node[nodeID].outbuffer[i] = DEAD_VALUE;            (node[nodeID].outbuffer_req)--;        }            }    }
