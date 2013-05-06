// $Id$
/*
 * Preset handling
 *
 * ==========================================================================
 *
 *  Copyright (C) 2009 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _PRESETS_H
#define _PRESETS_H

#include <eeprom.h>

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

// EEPROM locations
#define PRESETS_EEPROM_SIZE    EEPROM_EMULATED_SIZE // prepared for up to 128 entries

#define PRESETS_ADDR_RESERVED  0x00 // should not be written
#define PRESETS_ADDR_MAGIC01   0x01
#define PRESETS_ADDR_MAGIC23   0x02

#define PRESETS_ADDR_MIDIMON   0x04
#define PRESETS_ADDR_NUM_SRIO  0x05

#define PRESETS_ADDR_UIP_USE_DHCP  0x08
#define PRESETS_ADDR_UIP_IP01      0x12
#define PRESETS_ADDR_UIP_IP23      0x13
#define PRESETS_ADDR_UIP_NETMASK01 0x14
#define PRESETS_ADDR_UIP_NETMASK23 0x15
#define PRESETS_ADDR_UIP_GATEWAY01 0x16
#define PRESETS_ADDR_UIP_GATEWAY23 0x17

#define PRESETS_NUM_OSC_RECORDS            4
#define PRESETS_OFFSET_BETWEEN_OSC_RECORDS 8
#define PRESETS_ADDR_OSC0_REMOTE01    (0x20 + 0*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x20
#define PRESETS_ADDR_OSC0_REMOTE23    (0x21 + 0*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x21
#define PRESETS_ADDR_OSC0_REMOTE_PORT (0x22 + 0*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x22
#define PRESETS_ADDR_OSC0_LOCAL_PORT  (0x23 + 0*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x23

#define PRESETS_ADDR_OSC1_REMOTE01    (0x20 + 1*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x28
#define PRESETS_ADDR_OSC1_REMOTE23    (0x21 + 1*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x29
#define PRESETS_ADDR_OSC1_REMOTE_PORT (0x22 + 1*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x2a
#define PRESETS_ADDR_OSC1_LOCAL_PORT  (0x23 + 1*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x2b

#define PRESETS_ADDR_OSC2_REMOTE01    (0x20 + 2*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x30
#define PRESETS_ADDR_OSC2_REMOTE23    (0x21 + 2*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x31
#define PRESETS_ADDR_OSC2_REMOTE_PORT (0x22 + 2*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x32
#define PRESETS_ADDR_OSC2_LOCAL_PORT  (0x23 + 2*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x33

#define PRESETS_ADDR_OSC3_REMOTE01    (0x20 + 3*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x38
#define PRESETS_ADDR_OSC3_REMOTE23    (0x21 + 3*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x39
#define PRESETS_ADDR_OSC3_REMOTE_PORT (0x22 + 3*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x3a
#define PRESETS_ADDR_OSC3_LOCAL_PORT  (0x23 + 3*PRESETS_OFFSET_BETWEEN_OSC_RECORDS) // 0x3b


#define PRESETS_ADDR_SRIO_NUM          0x80

#define PRESETS_NUM_KB_RECORDS            2
#define PRESETS_OFFSET_BETWEEN_KB_RECORDS 0x20
#define PRESETS_ADDR_KB1_MIDI_PORTS      (0xc0 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xc0
#define PRESETS_ADDR_KB1_MIDI_CHN        (0xc1 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xc1
#define PRESETS_ADDR_KB1_NOTE_OFFSET     (0xc2 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xc2
#define PRESETS_ADDR_KB1_ROWS            (0xc3 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xc3
#define PRESETS_ADDR_KB1_DOUT_SR1        (0xc4 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xc4
#define PRESETS_ADDR_KB1_DOUT_SR2        (0xc5 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xc5
#define PRESETS_ADDR_KB1_DIN_SR1         (0xc6 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xc6
#define PRESETS_ADDR_KB1_DIN_SR2         (0xc7 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xc7
#define PRESETS_ADDR_KB1_MISC            (0xc8 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xc8
#define PRESETS_ADDR_KB1_DELAY_FASTEST   (0xc9 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xc9
#define PRESETS_ADDR_KB1_DELAY_SLOWEST   (0xca + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xca
#define PRESETS_ADDR_KB1_AIN_CFG1_1      (0xcb + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xcb
#define PRESETS_ADDR_KB1_AIN_CFG1_2      (0xcc + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xcc
#define PRESETS_ADDR_KB1_AIN_CFG2_1      (0xcd + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xcd
#define PRESETS_ADDR_KB1_AIN_CFG2_2      (0xce + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xce
#define PRESETS_ADDR_KB1_AIN_CFG3_1      (0xcf + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xcf
#define PRESETS_ADDR_KB1_AIN_CFG3_2      (0xd0 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xd0
#define PRESETS_ADDR_KB1_DELAY_FASTEST_BLACK_KEYS         (0xd1 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xd1
#define PRESETS_ADDR_KB1_DELAY_FASTEST_RELEASE            (0xd2 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xd2
#define PRESETS_ADDR_KB1_DELAY_FASTEST_RELEASE_BLACK_KEYS (0xd3 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xd3
#define PRESETS_ADDR_KB1_DELAY_SLOWEST_RELEASE            (0xd4 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xd4
#define PRESETS_ADDR_KB1_AIN_CFG4        (0xd5 + 0*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xd5


#define PRESETS_ADDR_KB2_MIDI_PORTS      (0xc0 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xe0
#define PRESETS_ADDR_KB2_MIDI_CHN        (0xc1 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xe1
#define PRESETS_ADDR_KB2_NOTE_OFFSET     (0xc2 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xe2
#define PRESETS_ADDR_KB2_ROWS            (0xc3 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xe3
#define PRESETS_ADDR_KB2_DOUT_SR1        (0xc4 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xe4
#define PRESETS_ADDR_KB2_DOUT_SR2        (0xc5 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xe5
#define PRESETS_ADDR_KB2_DIN_SR1         (0xc6 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xe6
#define PRESETS_ADDR_KB2_DIN_SR2         (0xc7 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xe7
#define PRESETS_ADDR_KB2_MISC            (0xc8 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xe8
#define PRESETS_ADDR_KB2_DELAY_FASTEST   (0xc9 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xe9
#define PRESETS_ADDR_KB2_DELAY_SLOWEST   (0xca + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xea
#define PRESETS_ADDR_KB2_AIN_CFG1_1      (0xcb + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xeb
#define PRESETS_ADDR_KB2_AIN_CFG1_2      (0xcc + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xec
#define PRESETS_ADDR_KB2_AIN_CFG2_1      (0xcd + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xed
#define PRESETS_ADDR_KB2_AIN_CFG2_2      (0xce + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xee
#define PRESETS_ADDR_KB2_AIN_CFG3_1      (0xcf + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xef
#define PRESETS_ADDR_KB2_AIN_CFG3_2      (0xd0 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xf0
#define PRESETS_ADDR_KB2_DELAY_FASTEST_BLACK_KEYS         (0xd1 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xf1
#define PRESETS_ADDR_KB2_DELAY_FASTEST_RELEASE            (0xd2 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xf2
#define PRESETS_ADDR_KB2_DELAY_FASTEST_RELEASE_BLACK_KEYS (0xd3 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xf3
#define PRESETS_ADDR_KB2_DELAY_SLOWEST_RELEASE            (0xd4 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xf4
#define PRESETS_ADDR_KB2_AIN_CFG4        (0xd5 + 1*PRESETS_OFFSET_BETWEEN_KB_RECORDS) // 0xf5


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 PRESETS_Init(u32 mode);

extern u16 PRESETS_Read16(u8 addr);
extern u32 PRESETS_Read32(u8 addr);

extern s32 PRESETS_Write16(u8 addr, u16 value);
extern s32 PRESETS_Write32(u8 addr, u32 value);

extern s32 PRESETS_StoreAll(void);


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

#endif /* _PRESETS_H */
