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
int prime = 1234567;

char textstring[] = "text, more text, and even more text!";

int timeoutcount;

/* Interrupt Service Routine */
void user_isr( void ) {
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

      timeoutcount = 0;
    }    
  }  
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
  IFSCLR(0) = 0x100;  // Clear timer interrupt flag
  IECSET(0) = 0x100;  // Enable interrupts for timer 2
  T2CONSET = 0x8000;  // Start timer

  enable_interrupt();

  return;
}

/* This function is called repetitively from the main program */
void labwork( void ) {
prime = nextprime( prime );
display_string( 0, itoaconv( prime ) );
display_update();
}