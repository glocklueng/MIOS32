/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern u8 screen[64][128];             // Screen buffer [y][x]

extern char screenMode[16];
extern char screenFile[16];
extern u16 screenPosBar;
extern u16 screenPosStep;

// Render test screen, one half is "full on" for flicker tests
void testScreen();

// Display the current screen buffer
void display();
