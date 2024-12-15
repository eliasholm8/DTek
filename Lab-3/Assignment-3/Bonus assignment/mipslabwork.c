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

/* Global variables */
int mytime = 0x5957; // Initial time value (BCD format: HHMMSS)
int prime = 1234567; // Initial prime number

char textstring[] = "text, more text, and even more text!"; // Sample text for display

int timeoutcount; // Counter for timer interrupts

volatile int *tris_E; // Pointer to TRIS register for Port E (controls direction of pins)
volatile int *port_E; // Pointer to PORT register for Port E (controls output values)

int port_E_value = 0; // Counter for Port E value (incremented by SW1)

/* Interrupt Service Routine */
void user_isr( void ) {
  if (IFS(0) & 0x100) // Check if Timer 2 interrupt flag is set
  {
    IFSCLR(0) = 0x100; // Clear the Timer 2 interrupt flag
    timeoutcount++; // Increment timeout counter

    if (timeoutcount == 10) // Every 10 interrupts (1 second)
    {
      time2string( textstring, mytime ); // Convert current time to string
      display_string( 3, textstring ); // Display time string on the last line
      display_update(); // Update the display
      tick( &mytime ); // Increment the time value

      timeoutcount = 0; // Reset timeout counter
    }    
  }  

  if (IFS(0) & 0x80) // Check if External Interrupt 1 (INT1) flag is set
  {
    IFSCLR(0) = 0x80; // Clear INT1 interrupt flag
    (*port_E)++;
  }
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  // IO initialization
  port_E = (volatile int *) 0xbf886110; // Address for PORT E output register
  tris_E = (volatile int *) 0xbf886100; // Address for TRIS E direction register
  *port_E = 0x0; // Clear all outputs on Port E
  *tris_E = *tris_E & 0xffffff00; // Set bits 0-7 of Port E to output mode

  TRISD &= 0x0fe0; // Configure specific Port D bits for input/output

  // Timer initialization
  // Configure Timer 2
  T2CON = 0x0; // Disable Timer 2 during setup
  T2CONSET = 0x70; // Set prescaler to 1:256 (See Table 14-8 in the PIC32 manual)

  PR2 = 31250; // Timer 2 period register (10 Hz interrupt frequency)
  TMR2 = 0x0; // Clear Timer 2 counter
  IFSCLR(0) = 0x100; // Clear Timer 2 interrupt flag
  T2CONSET = 0x8000; // Enable Timer 2

  IECSET(0) = 0x100; // Enable Timer 2 interrupts
  IPCSET(2) = 0x1F; // Set priority and sub-priority for Timer 2

  // External Interrupt 1 (SW1) initialization
  IECSET(0) = 0x80; // Enable External Interrupt 1 (INT1)
  IPCSET(1) = 0x1C; // Set priority and sub-priority for INT1
  IFSCLR(0) = 0x80; // Clear INT1 interrupt flag

  enable_interrupt();

  return;
}

/* This function is called repetitively from the main program */
void labwork( void ) {
  prime = nextprime( prime ); // Find the next prime number
  display_string( 0, itoaconv( prime ) ); // Convert the prime number to string and display it
  display_update(); // Update the display
}
