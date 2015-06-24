// $Id: screen.c 1223 2011-06-23 21:26:52Z hawkeye $

#include <mios32.h>

#include <stdarg.h>

#include <glcd_font.h>
#include <app_lcd.h>

#include "gfx_resources.h"
#include "seq.h"
#include "voxelspace.h"


// -------------------------------------------------------------------------------------------
// SSD 1322 Screen routines by Hawkeye

u8 screen[64][128];             // Screen buffer [y][x]

char screenMode[16] = "";
char screenFile[16] = "";
u16 screenPosBar = 0;
u16 screenPosStep = 0;

unsigned char* fontptr = (unsigned char*) fontsmall_pixdata;

/*
// Set normal font (default)
//
void setNormalFont()
{
   fontptr = (unsigned char*) fontnormal_pixdata;
}
// ----------------------------------------------------------------------------------------


// Set bold font
void setBoldFont()
{
   fontptr = (unsigned char*) fontbold_pixdata;
}
// ----------------------------------------------------------------------------------------
*/

// Display the given string at the given pixel coordinates
// output to screen output buffer, the next display() will render it to hardware
// provides clipping support, coordinates can be offscreen/negative for scrolling fx
//
void printString(int xPixCoord /* even! */, int yPixCoord, const char *str)
{
   unsigned stringpos = 0;

   while (*str != '\0')
   {
      int c = *str++;
      unsigned x;

      // in-font coordinates
      unsigned f_y = 0;
      unsigned f_x = (c-32) * fontchar_bytewidth;

      // screenbuf target coordinates
      unsigned s_y = yPixCoord;
      unsigned s_x = xPixCoord / 2 + stringpos * fontchar_bytewidth; // 2 pixels per byte, require even xPixCoord start coordinate

      while (f_y < fontchar_height)
      {
         if (s_y >= 0 && s_y < 64) // clip y offscreen
         {
            unsigned char* sdata = (unsigned char*) screen + s_y * 128 + s_x;
            unsigned char* fdata = (unsigned char*) fontptr + f_y * fontline_bytewidth + f_x;
            unsigned c_s_x = s_x;

            for (x = 0; x <fontchar_bytewidth; x++)
            {
               if (c_s_x >= 0 && c_s_x < 128)
                  if (*fdata)
                     *sdata = *fdata;  // inner loop: copy 2 pixels, if onscreen

               c_s_x++;
               fdata++;
               sdata++;
            }
         }

         f_y++;
         s_y++;
      }

      stringpos++;
   }
}
// ----------------------------------------------------------------------------------------


// Display the given formatted string at the given pixel coordinates
// output to screen output buffer, the next display() will render it to hardware
// provides clipping support, coordinates can be offscreen/negative for scrolling fx
//
void printFormattedString(int xPixCoord /* even! */, int yPixCoord, const char* format, ...)
{
   char buffer[64]; // TODO: tmp!!! Provide a streamed COM method later!
   va_list args;

   va_start(args, format);
   vsprintf((char *)buffer, format, args);
   return printString(xPixCoord, yPixCoord, buffer);
}
// ----------------------------------------------------------------------------------------


// Display the current screen buffer
//
void display(void)
{
   u8 i, j;

   printFormattedString(0, 51, "%s %s %u:%u", screenMode, screenFile, screenPosBar, screenPosStep % 16);

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
