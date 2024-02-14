#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw( void );
int getbtns( void );

int getsw(){
    return (PORTD >> 8) & 0x000f;
}

int getbtns(){
    return (PORTD >> 5) & 0x0007;
}