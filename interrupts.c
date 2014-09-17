/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>             /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "interrupts.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

void interrupt low_priority TimerStopped()
{
    //check if the interrupt is caused by the pin RB0
    if(INTCONbits.INT0F == 1)
    {
        LATB4 = ~LATB4;
        INTCONbits.INT0F = 0;
    }

}