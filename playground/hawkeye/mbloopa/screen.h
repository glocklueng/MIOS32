/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern u8 screen[64][128];             // Screen buffer [y][x]

extern char screenMode[16];
extern char screenFile[16];
extern u16 screenPosBar;
extern u16 screenPosStep;


// If showLogo is true, draw the MBLoopa Logo (usually during unit startup)
void screenShowLoopaLogo(u8 showLogo);

// Set the currently selected clip
void screenSetClipSelected(u8 clipNumber);

// Set, if a clip is looped
void screenSetClipLooped(u8 clipNumber, u8 isLooped);

// Set the position info of a clip
void screenSetClipPosition(u8 clipNumber, u8 stepPosition, u8 stepLength);

// Display the current screen buffer
void display();

// Render test screen, one half is "full on" for flicker tests
void testScreen();
