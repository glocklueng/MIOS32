// $Id$
/*
 * Header file for scale routines
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _SEQ_SCALE_H
#define _SEQ_SCALE_H


/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 SEQ_SCALE_Init(u32 mode);

extern s32 SEQ_SCALE_NumGet(void);
extern char *SEQ_SCALE_NameGet(u8 scale);

extern s32 SEQ_SCALE_NoteValueGet(u8 note, u8 scale, u8 root);
extern s32 SEQ_SCALE_NextNoteInScale(u8 current_note, u8 scale, u8 root);

extern s32 SEQ_SCALE_Note(mios32_midi_package_t *p, u8 scale, u8 root);



/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

#endif /* _SEQ_SCALE_H */
