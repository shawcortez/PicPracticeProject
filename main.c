/* 
 * File:   newmain.c
 * Author: wenceslaoshaw-cortezshaw-cortez
 *
 * Created on September 11, 2014, 2:20 PM
 */

/*******************************************************************************
** Files to Include
*******************************************************************************/
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <delays.h>
#include "system.h"
#include "user.h"
#include "globals.h"        // Holds global variables
#include "config.h"
#include "portb.h"


int CHA;                    // Preserve state of Channel A from encoder
int CHB;                    // Preserve state of Channel B from encoder

int main(void) {

    //--------------
    // Initialization Phase
    char LCDinit[] = {0x33,0x32,0x28,0x01,0x0c,0x06,0x00}; //Array holding initialization string for LCD
    char Msg1[] = {0x84,'C','U','N','T','\0'};
    char Msg2[] = {0xC5,'R','P','M','\0'};
    InitApp();              // Initialize Ports
    DisplayLCD(LCDinit,1);  // Initialize LCD
    DisplayLCD(Msg1,0);     // Display message on LCD
    DisplayLCD(Msg2,0);     // Display message 2
    CHA = PORTBbits.RB5;    // Initialize channel A
    CHB = PORTBbits.RB4;    // Initialize channel B
    InitInterrupts();       // Initialize interrupts for Port B encoder

    //--------------
    // Loop phase: Read encoder, Display on LCD
    while(1)
    {

        WaitHalfSec();
        //ReadEncoder();
    }

    
    return (EXIT_SUCCESS);
}

void high_priority interrupt high_isr(void)
{


      /* TODO Add High Priority interrupt routine code here. */

      /* Determine which flag generated the interrupt */
      

}

/* Low-priority interrupt routine */
void low_priority interrupt low_isr(void)
{
    if(INTCONbits.RBIF == 1)
      {
        ReadEncoder();
        INTCONbits.RBIF = 0;  // Clear Interrupt Flag 1
      }

      else
      {
          /* Unhandled interrupts */
      }

}