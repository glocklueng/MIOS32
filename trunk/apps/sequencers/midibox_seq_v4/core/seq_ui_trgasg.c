// $Id$
/*
 * Trigger assignment page
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
#include "seq_cc.h"


/////////////////////////////////////////////////////////////////////////////
// Local definitions
/////////////////////////////////////////////////////////////////////////////

#define NUM_OF_ITEMS       8
#define ITEM_GXTY          0
#define ITEM_GATE          1
#define ITEM_SKIP          2
#define ITEM_ACCENT        3
#define ITEM_GLIDE         4
#define ITEM_ROLL          5
#define ITEM_RANDOM_GATE   6
#define ITEM_RANDOM_VALUE  7


/////////////////////////////////////////////////////////////////////////////
// Local LED handler function
/////////////////////////////////////////////////////////////////////////////
static s32 LED_Handler(u16 *gp_leds)
{
  if( ui_cursor_flash ) // if flashing flag active: no LED flag set
    return 0;

  switch( ui_selected_item ) {
    case ITEM_GXTY: *gp_leds = 0x0001; break;
    case ITEM_GATE: *gp_leds = 0x0002; break;
    case ITEM_SKIP: *gp_leds = 0x0004; break;
    case ITEM_ACCENT: *gp_leds = 0x0008; break;
    case ITEM_GLIDE: *gp_leds = 0x0010; break;
    case ITEM_ROLL: *gp_leds = 0x0020; break;
    case ITEM_RANDOM_GATE: *gp_leds = 0x0040; break;
    case ITEM_RANDOM_VALUE: *gp_leds = 0x0080; break;
  }

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
  switch( encoder ) {
    case SEQ_UI_ENCODER_GP1:
      ui_selected_item = ITEM_GXTY;
      break;

    case SEQ_UI_ENCODER_GP2:
      ui_selected_item = ITEM_GATE;
      break;

    case SEQ_UI_ENCODER_GP3:
      ui_selected_item = ITEM_SKIP;
      break;

    case SEQ_UI_ENCODER_GP4:
      ui_selected_item = ITEM_ACCENT;
      break;

    case SEQ_UI_ENCODER_GP5:
      ui_selected_item = ITEM_GLIDE;
      break;

    case SEQ_UI_ENCODER_GP6:
      ui_selected_item = ITEM_ROLL;
      break;

    case SEQ_UI_ENCODER_GP7:
      ui_selected_item = ITEM_RANDOM_GATE;
      break;

    case SEQ_UI_ENCODER_GP8:
      ui_selected_item = ITEM_RANDOM_VALUE;
      break;

    case SEQ_UI_ENCODER_GP9:
    case SEQ_UI_ENCODER_GP10:
    case SEQ_UI_ENCODER_GP11:
    case SEQ_UI_ENCODER_GP12:
    case SEQ_UI_ENCODER_GP13:
    case SEQ_UI_ENCODER_GP14:
    case SEQ_UI_ENCODER_GP15:
    case SEQ_UI_ENCODER_GP16:
      return -1; // not mapped
      break;
  }

  // for GP encoders and Datawheel
  switch( ui_selected_item ) {
    case ITEM_GXTY:         return SEQ_UI_GxTyInc(incrementer);
    case ITEM_GATE:         return SEQ_UI_CC_Inc(SEQ_CC_ASG_GATE, 0, 3, incrementer);
    case ITEM_SKIP:         return SEQ_UI_CC_Inc(SEQ_CC_ASG_SKIP, 0, 3, incrementer);
    case ITEM_ACCENT:       return SEQ_UI_CC_Inc(SEQ_CC_ASG_ACCENT, 0, 3, incrementer);
    case ITEM_GLIDE:        return SEQ_UI_CC_Inc(SEQ_CC_ASG_GLIDE, 0, 3, incrementer);
    case ITEM_ROLL:         return SEQ_UI_CC_Inc(SEQ_CC_ASG_ROLL, 0, 3, incrementer);
    case ITEM_RANDOM_GATE:  return SEQ_UI_CC_Inc(SEQ_CC_ASG_RANDOM_GATE, 0, 3, incrementer);
    case ITEM_RANDOM_VALUE: return SEQ_UI_CC_Inc(SEQ_CC_ASG_RANDOM_VALUE, 0, 3, incrementer);
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

#if 0
  // leads to: comparison is always true due to limited range of data type
  if( button >= SEQ_UI_BUTTON_GP1 && button <= SEQ_UI_BUTTON_GP16 ) {
#else
  if( button <= SEQ_UI_BUTTON_GP16 ) {
#endif
    // re-use encoder handler - only select UI item, don't increment
    return Encoder_Handler((int)button, 0);
  }

  // remaining buttons:
  switch( button ) {
    case SEQ_UI_BUTTON_Select:
    case SEQ_UI_BUTTON_Right:
      if( ++ui_selected_item >= NUM_OF_ITEMS )
	ui_selected_item = 0;
      return 1; // value always changed

    case SEQ_UI_BUTTON_Left:
      if( ui_selected_item == 0 )
	ui_selected_item = NUM_OF_ITEMS-1;
      return 1; // value always changed

    case SEQ_UI_BUTTON_Up:
      return Encoder_Handler(SEQ_UI_ENCODER_Datawheel, 1);

    case SEQ_UI_BUTTON_Down:
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
  if( high_prio )
    return 0; // there are no high-priority updates

  // layout:
  // 00000000001111111111222222222233333333330000000000111111111122222222223333333333
  // 01234567890123456789012345678901234567890123456789012345678901234567890123456789
  // <--------------------------------------><-------------------------------------->
  // Trk. Gate Skip Acc. Glide Roll R.G  R.V                                         
  // G1T1   A    -    B    -     C   -    -                                         

  u8 visible_track = SEQ_UI_VisibleTrackGet();

  ///////////////////////////////////////////////////////////////////////////
  MIOS32_LCD_DeviceSet(0);
  MIOS32_LCD_CursorSet(0, 0);
  MIOS32_LCD_PrintString("Trk. ");

  int i;
  for(i=0; i<7; ++i)
    MIOS32_LCD_PrintString(SEQ_TRG_TypeStr(i));
 
  MIOS32_LCD_DeviceSet(1);
  MIOS32_LCD_CursorSet(0, 0);
  SEQ_LCD_PrintSpaces(40);


  ///////////////////////////////////////////////////////////////////////////
  MIOS32_LCD_DeviceSet(0);
  MIOS32_LCD_CursorSet(0, 1);

  if( ui_selected_item == ITEM_GXTY && ui_cursor_flash ) {
    SEQ_LCD_PrintSpaces(5);
  } else {
    SEQ_LCD_PrintGxTy(ui_selected_group, ui_selected_tracks);
    SEQ_LCD_PrintSpaces(1);
  }

  ///////////////////////////////////////////////////////////////////////////

  for(i=0; i<7; ++i) {
    MIOS32_LCD_PrintChar(' ');
    MIOS32_LCD_PrintChar(((i+1) == ui_selected_item) ? '>' : ' ');

    if( (i+1) == ui_selected_item && ui_cursor_flash )
      MIOS32_LCD_PrintChar(' ');
    else {
      u8 value = SEQ_CC_Get(visible_track, SEQ_CC_ASG_GATE+i);
      MIOS32_LCD_PrintChar(value ? ('A'-1+value) : '-');
    }

    MIOS32_LCD_PrintChar(((i+1) == ui_selected_item) ? '<' : ' ');
    MIOS32_LCD_PrintChar(' ');
  }


  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_UI_TRGASG_Init(u32 mode)
{
  // install callback routines
  SEQ_UI_InstallButtonCallback(Button_Handler);
  SEQ_UI_InstallEncoderCallback(Encoder_Handler);
  SEQ_UI_InstallLEDCallback(LED_Handler);
  SEQ_UI_InstallLCDCallback(LCD_Handler);

  return 0; // no error
}
