// $Id$
/*
 * Header file for parameter layer routines
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _SEQ_PAR_H
#define _SEQ_PAR_H

#include "seq_core.h"


/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

#define SEQ_PAR_NUM_LAYERS  3


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 SEQ_PAR_Init(u32 mode);

extern s32 SEQ_PAR_Set(u8 track, u8 step, u8 par_layer, u8 value);
extern s32 SEQ_PAR_Get(u8 track, u8 step, u8 par_layer);


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

// should only be directly accessed by SEQ_FILE_B, remaining functions should
// use SEQ_PAR_Get/Set
extern u8 par_layer_value[SEQ_CORE_NUM_TRACKS][SEQ_PAR_NUM_LAYERS][SEQ_CORE_NUM_STEPS];

#endif /* _SEQ_PAR_H */
