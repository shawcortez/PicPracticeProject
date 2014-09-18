/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>             /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <string.h>
#include <delays.h>
#include "globals.h"
#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/


int InitApp(void)
{
    //------------
    // Setup I/O on ports
    TRISA = 0b11100001;		// Set I/O for PORTA (LEDS)
    TRISBbits.RB4 = 1;		// Set bit 4 as input on PORTB	(to read from encoder)
    TRISBbits.RB5 = 1;		// Set bit 5 as input on PORTB	(to read from encoder)
    TRISBbits.RB1 = 0;		// Set bit 1 as output on PORTB	(to power encoder)
    PORTBbits.RB1 = 1;          // Output power to encoder
    TRISD = 0b00001111;		// Set I/O for PORTD (LCD, switch 3, RPG)
    TRISE = 0b00000100;		// Set I/O for PORTE (LCD)
    ADCON1 = 0b10000111;	// Configure for using LCD
    
    //------------
    // Light up leds
    PORTA  = 0b00010000;	// Turn all LEDs off to initialize
    PORTAbits.RA3 = 1;		// Turn on D4 LED
    WaitHalfSec();		// Wait 0.5 sec with D4 on
    PORTAbits.RA3 = 0;		// Turn off D4 LED
    PORTAbits.RA2 = 1;		// Turn on D5 LED
    WaitHalfSec();		// Wait 0.5 sec with D5 on
    PORTAbits.RA2 = 0;		// Turn off D5 LED
    PORTAbits.RA1 = 1;          // Turn on D6 LED
    WaitHalfSec();		// Wait 0.5 sec with D6 on
    PORTAbits.RA1 = 0;          // Turn off D6 LED

    return 0;
}

/**********************************
* WaitHalfSec: Time for 0.5 seconds
***********************************/
int WaitHalfSec(void)
{
    int Halfcount = 50;			// Load count for 0.5 sec using 10 ms delay
    while (Halfcount != 0)
    {
            Delay1KTCYx(25);		// Use delay function for 10 ms count
            Halfcount = Halfcount-1;	// Decrement 0.5 sec count
    }
    return 0;
}

/*******************************
 * DisplayLCD(const char *)
 *
 * This subroutine is called with the passing in of an array of a constant
 * display string.  It sends the bytes of the string to the LCD.  The first
 * byte sets the cursor position.  The remaining bytes are displayed, beginning
 * at that position.
 * This subroutine expects a normal one-byte cursor-positioning code, 0xhh, or
 * an occasionally used two-byte cursor-positioning code of the form 0x00hh.
 *******************************/
int DisplayLCD(char * tempPtr, int init)
{
        char currentChar;
        
        

        if (init == 1)
        {
            Delay1KTCYx(250);               // Wait 0.1 s to bypass LCD startup
            PORTEbits.RE0 = 0;              // Drive RS pin low for cursor-positioning code
            while (*tempPtr != 0)           // if the byte is not zero
            {
              currentChar = *tempPtr;
              PORTEbits.RE1 = 1;            // Drive E pin high
              PORTD = currentChar;          // Send upper nibble
              PORTEbits.RE1 = 0;            // Drive E pin low so LCD will accept nibble
              Delay100TCYx(250);			// Wait 10 ms
              currentChar <<= 4;            // Shift lower nibble to upper nibble
              PORTEbits.RE1 = 1;            // Drive E pin high again
              PORTD = currentChar;          // Write lower nibble
              PORTEbits.RE1 = 0;            // Drive E pin low so LCD will process byte
              Delay100TCYx(250);            // Wait 10 ms
              tempPtr++;                    // Increment pointerto next character
            }
        }
        else
        {
            PORTEbits.RE0 = 0;              // Drive RS pin low for cursor-positioning code
            while (*tempPtr)                // if the byte is not zero
            {
              currentChar = *tempPtr;
              PORTEbits.RE1 = 1;            // Drive E pin high
              PORTD = currentChar;          // Send upper nibble
              PORTEbits.RE1 = 0;            // Drive E pin low so LCD will accept nibble
              currentChar <<= 4;            // Shift lower nibble to upper nibble
              PORTEbits.RE1 = 1;            // Drive E pin high again
              PORTD = currentChar;          // Write lower nibble
              PORTEbits.RE1 = 0;            // Drive E pin low so LCD will process byte
              Delay10TCYx(10);              // Wait 40 usec
              PORTEbits.RE0 = 1;            // Drive RS pin high for displayable characters
              tempPtr++;                    // Increment pointerto next character
            }

        }
        return 0;
}

/*******************************
 * InitInterrupts(void)
 *
 * This subroutine initializes interrupts. At the moment the only interrupt is
 * from Port B (encoder at B4, B5) and will be implement as a high priority to
 * determine wheel speed.
 *******************************/
int InitInterrupts(void)
{
    RCONbits.IPEN = 1;                      // Enable priority levels
    INTCON2 = 0b00000000;                   // Set port b as high priority
    INTCON = 0b11001000;                    // Enable port b interrupt
    /*T0CONbits.TMR0ON = 1;
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 1;

    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    INTCONbits.TMR0IF = 0;
    INTCON2bits.TMR0IP = 0;
    INTCONbits.TMR0IE = 1; */
    //ei();
    return 0;
}


void ReadEncoder(void)
{
    //int QEM[] = {0,-1,1,2,1,0,2,-1,-1,2,0,1,2,1,-1,0};
    int Anew = PORTBbits.RB5;
    int Bnew = PORTBbits.RB4;

    int Old = CHA*2 + CHB;
    int New = Anew*2 + Bnew;
    int NEW_ROT = QEM[Old*4+New];

    if (NEW_ROT == 2)                   // Shits fucked so don't trust it
    {
        NEW_ROT = OLD_ROT;              // Use previous rotation either 1:CW, -1:CCW, 0
        Anew = CHA;                     // Here use Anew as old CHA
        Bnew = CHB;                     // Here use Bnew as old CHB
        if (NEW_ROT == 1)               // Simulate CW rotation
        {
            CHB = Anew;
            CHA = Bnew^1;
        }
        else if (NEW_ROT == -1)         // Simulate CCW rotation
        {
            CHB = Anew^1;
            CHA = Bnew;
        }
        else                            // No rotation so don't change anything
        {

        }
    }
    else
    {
        CHA = Anew;                         // Update state of channel A
        CHB = Bnew;                         // Update state of channel B
    }

    if (NEW_ROT == 1)
    {
        PORTAbits.RA3 = 1;
        PORTAbits.RA2 = 0;
        PORTAbits.RA1 = 0;
    }
    else if (NEW_ROT == -1)
    {
        PORTAbits.RA3 = 0;
        PORTAbits.RA2 = 1;
        PORTAbits.RA1 = 0;
    }
    else
    {
        PORTAbits.RA3 = 0;
        PORTAbits.RA2 = 0;
        PORTAbits.RA1 = 0;
    }

    OLD_ROT = NEW_ROT;
    
}