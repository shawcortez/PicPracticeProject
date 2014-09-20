/* 
 * File:   main.c
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
#include <math.h>
#include "system.h"
#include "user.h"
#include "globals.h"        // Holds global variables
#include "timers.h"
#include "config.h"

#define USE_OR_MASKS        // For using peripheral library
#define EncoderCount  0x0000// Load value to count up to 65536 overflow (26ms)
#define EncoderTS 0.026     // Time for Timer to count EncoderCount (s)

int CHA;                    // Preserve state of Channel A from encoder
int CHB;                    // Preserve state of Channel B from encoder
int OLD_ROT;                // Preserve state of current rotation in case encoder bounces
int CCWTurn;                // Counts how many CCW turns have occured on the encoder
int CWTurn;                 // Counts how many CW turns have occured on the encoder
const int CountPerRev = 1024;// Total counts per revolution (based on encoder specs)
const int EncoderPoll = 50; // Total number of counts until direction of encoder rotation is determined
double PartialRot;          // Holds fraction of revolution that has occured
double RPS;                 // Holds rev/s value 

int main(void) {

    //--------------
    // Initialization 
    char LCDinit[] = {0x33,0x32,0x28,0x01,0x0c,0x06,0x00}; //Array holding initialization string for LCD
    char Msg1[] = {0x84,'C','U','N','T','\0'};
    char Msg2[] = {0xC5,'R','P','S','\0'};
    InitApp();              // Initialize Ports
    DisplayLCD(LCDinit,1);  // Initialize LCD
    DisplayLCD(Msg1,0);     // Display message on LCD
    DisplayLCD(Msg2,0);     // Display message 2
    CHA = PORTBbits.RB5;    // Initialize channel A
    CHB = PORTBbits.RB4;    // Initialize channel B
    OLD_ROT = 0;            // Initialize state of rotation
    CCWTurn = 0;            // Initialize CCW count
    CWTurn = 0;             // Initialize CW count
    RPS = 0.0;              // Initialize RPS value
    InitInterrupts();       // Initialize timer interrupts for Port B encoder
    WriteTimer0(EncoderCount);// Load Timer0

    //--------------
    // Loop phase: Display RPS on LCD
    while(1)
    {
        Delay1KTCYx(25);
        WriteLCD(0xC0,5,RPS);      // Display RPS on LCD
    }

    CloseTimer0();
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
        ReadEncoder();          // Sample encoder
        INTCONbits.RBIF = 0;    // Clear Interrupt Flag
      }

    else if (INTCONbits.TMR0IF == 1)
      {
        RPS = (fabs(PartialRot))/EncoderTS;    // Compute rps by #rotations/sampletime = rev/sec
        PartialRot = 0;         // Clear revolution value
        WriteTimer0(EncoderCount);  // Reload timer0
        INTCONbits.TMR0IF = 0;  // Clear Interrupt Flag
      }

}