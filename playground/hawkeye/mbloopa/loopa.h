// MBLoopa Core Logic
// (c) Hawkeye 2015

#define DEBUG_MSG MIOS32_MIDI_SendDebugMessage

// Current "menu" position in app

extern u8 baseView;             // if not in baseView, we are in single clipView
extern u8 clipNumber;           // currently active or last active clip number (1-8)
extern u8 displayMode2d;        // if not in 2d display mode, we are rendering voxelspace
extern u16 sessionNumber;       // currently active session number (directories will be auto-created)


// First callback from app - render Loopa Startup logo on screen
void loopaStartup();

// SD Card Available, initialize
void loopaSDCardAvailable();

// A buttonpress has occured
void loopaButtonPressed(s32 pin);

// An encoder has been turned
void loopaEncoderTurned(s32 encoder, s32 incrementer);






