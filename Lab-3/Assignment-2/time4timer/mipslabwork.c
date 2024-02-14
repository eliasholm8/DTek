/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}


/* Lab-specific initialization goes here */
void labinit( void )
{

  // IO stuff
  volatile int *port_E = (volatile int *) 0xbf886100;
  *port_E &= 0x00FF;  
  
  TRISD &= 0x0fe0;

  // Timer stuff
  // http://ww1.microchip.com/downloads/en/devicedoc/61105f.pdf
  T2CON = 0x0;
  T2CONSET = 0x70; // 1:256, See 14-8 in manual

  //  80Mhz / 10 = 8M, 8M / 256 = 31250
  PR2 = 31250;
  TMR2 = 0x0;
  IFS(0) = 0x0; // Set interrupt flag to 0
  T2CONSET = 0x8000;  // Start timer

  return;
}

int timeoutcount;
/* This function is called repetitively from the main program */
void labwork( void )
{
  volatile unsigned int *portE = (volatile unsigned int *)0xbf886110;

  int sw = getsw();
  int btns = getbtns();

  if (btns & 0b0100) {
    mytime &= 0x0fff;
    mytime |= (sw << 12);
  }
  if (btns & 0b0010) {
    mytime &= 0xf0ff;
    mytime |= (sw << 8);
  }
  if (btns & 0b0001) {
    mytime &= 0xff0f;
    mytime |= (sw << 4);
  }
  
  if (IFS(0) & 0x100)
  {
    IFSCLR(0) = 0x100;
    timeoutcount++;
    if (timeoutcount == 10)
    {
      time2string( textstring, mytime );
      display_string( 3, textstring );
      display_update();
      tick( &mytime );
      display_image(96, icon);
      (*portE)=btns;   

      timeoutcount = 0;
      
    }       
  }  
}
