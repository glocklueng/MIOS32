// $Id: screen.c 1223 2011-06-23 21:26:52Z hawkeye $

#include <mios32.h>

#include <glcd_font.h>
#include <app_lcd.h>

#include "voxelspace.h"



// -------------------------------------------------------------------------------------------
// SSD 1322 Screen routines by Hawkeye

u8 screen[64][128];             // Screen buffer [y][x]


// Display the current screen buffer
//
void display(void)
{
   u8 i, j;

   // Push screen buffer
   for (j = 0; j < 64; j++)
   {
      APP_LCD_Cmd(0x15);
      APP_LCD_Data(0+0x1c);

      APP_LCD_Cmd(0x75);
      APP_LCD_Data(j);

      APP_LCD_Cmd(0x5c);

      u8 bgcol = j < 6 ? ((j << 4) + j) : 0;
      for (i = 0; i < 128; i++)
      {
         APP_LCD_Data(screen[j][i]);
         screen[j][i] = bgcol;
         i++;
         APP_LCD_Data(screen[j][i]);
         screen[j][i] = bgcol;
      }
   }

}
// ----------------------------------------------------------------------------------------


// Render test screen, one half is "full on" for flicker tests
// one half contains a color gradient pattern
//
void testScreen()
{
  u16 x = 0;
  u16 y = 0;

  for (y = 0; y < 64; y++)
  {
     APP_LCD_Cmd(0x15);
     APP_LCD_Data(0x1c);

     APP_LCD_Cmd(0x75);
     APP_LCD_Data(y);

     APP_LCD_Cmd(0x5c);

     for (x = 0; x < 64; x++)
     {
        if (x < 32)
        {  // half screen pattern

           if (x || 4 == 0 || y || 4 == 0)
           {
              APP_LCD_Data(y & 0x0f);
              APP_LCD_Data(0);
           }
           else
           {
              APP_LCD_Data(0x00);
              APP_LCD_Data(0x00);
           }
        }
        else // half screen "white"
        {
           APP_LCD_Data(0xff);
           APP_LCD_Data(0xff);
        }
     }
  }

  while(1);
}
// -------------------------------------------------------------------------------------------
