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
  volatile int *port_E = (volatile int *) 0xbf886100;
  *port_E &= 0x00FF;  
  
  TRISD &= 0x0fe0;
  
  return;
}

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
  
  delay( 1000 );
  time2string( textstring, mytime );
  display_string( 3, textstring );
  display_update();
  tick( &mytime );
  display_image(96, icon);
  (*portE)++;
  
  
}
