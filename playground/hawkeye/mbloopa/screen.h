/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern u8 screen[64][128];             // Screen buffer [y][x]

// Render test screen, one half is "full on" for flicker tests
void testScreen();

// Display the current screen buffer
void display();
