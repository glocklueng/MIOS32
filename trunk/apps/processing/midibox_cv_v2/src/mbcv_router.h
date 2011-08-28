// $Id$
/*
 * MIDI Router functions for MIDIbox CV V2
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _MBCV_ROUTER_H
#define _MBCV_ROUTER_H

/////////////////////////////////////////////////////////////////////////////
// global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Type definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 MBCV_ROUTER_Init(u32 mode);

extern s32 MBCV_ROUTER_Receive(mios32_midi_port_t port, mios32_midi_package_t midi_package);
extern s32 MBCV_ROUTER_ReceiveSysEx(mios32_midi_port_t port, u8 midi_in);

extern s32 MBCV_ROUTER_MIDIClockInGet(mios32_midi_port_t port);
extern s32 MBCV_ROUTER_MIDIClockInSet(mios32_midi_port_t port, u8 enable);

extern s32 MBCV_ROUTER_MIDIClockOutGet(mios32_midi_port_t port);
extern s32 MBCV_ROUTER_MIDIClockOutSet(mios32_midi_port_t port, u8 enable);

extern s32 MBCV_ROUTER_SendMIDIClockEvent(u8 evnt0, u32 bpm_tick);

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////


#endif /* _MBCV_ROUTER_H */