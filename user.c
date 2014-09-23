/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>             /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <string.h>
#include <delays.h>
#include <stdio.h>
#include "globals.h"
#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/**********************************
* InitApp: Initialize ports as I/O for LEDS, LCD, encoder. Light up LEDS to show
 * initialization on pic.
***********************************/
void InitApp(void)
{
    //------------
    // Setup I/O on ports
    TRISA = 0b11100001;		// Set I/O for PORTA (LEDS)
    TRISBbits.RB4 = 1;		// Set bit 4 as input on PORTB	(to read from encoder)
    TRISBbits.RB5 = 1;		// Set bit 5 as input on PORTB	(to read from encoder)
    TRISBbits.RB1 = 0;		// Set bit 1 as output on PORTB	(to power encoder)
    PORTBbits.RB1 = 1;          // Output power to encoder
    TRISCbits.RC2 = 0;          // Set C2 as output for PWM to motor
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

}

/**********************************
* WaitHalfSec: Delay for 0.5 seconds
***********************************/
void WaitHalfSec(void)
{
    int Halfcount = 50;			// Load count for 0.5 sec using 10 ms delay
    while (Halfcount != 0)
    {
            Delay1KTCYx(25);		// Use delay function for 10 ms count
            Halfcount = Halfcount-1;	// Decrement 0.5 sec count
    }
}

/*******************************
 * DisplayLCD(char * tempPtr, int init):
 * This subroutine is called with a string to be displayed on the LCD
 * It sends the bytes of the string to the LCD.  The first
 * byte sets the cursor position.  The remaining bytes are displayed, beginning
 * at that position.
 * This subroutine expects a normal one-byte cursor-positioning code, 0xhh, or
 * an occasionally used two-byte cursor-positioning code of the form 0x00hh. The
 * init variable defines if the LCD is being initialized (init = 1) or if just
 * displaying a message on the LCD (init = 0)
 *******************************/
void DisplayLCD(char * tempPtr, int init)
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
}

/*******************************
 * InitInterrupts(void)
 *
 * This subroutine initializes interrupts. One interrupt is
 * from Port B (encoder at B4, B5) and will be implement as a low priority to
 * determine wheel speed. Timer0 overflow is also used as an interrupt to sample
 * rev/s of the encoder as a low priority interrupt.
 *******************************/
void InitInterrupts(void)
{
    //-------------------
    // General Interrupt
    RCONbits.IPEN = 1;                      // Enable priority levels
    INTCONbits.GIEH = 1;                    // Enable high priority interrupts
    INTCONbits.GIEL = 1;                    // Enable low priority interrupts

    //-------------------
    // PortB interrupts
    INTCON2bits.NOT_RBPU = 0;               // All portB pullups enabled
    INTCON2bits.RBIP = 0;                   // PortB interrupt as low priority
    INTCONbits.RBIF = 0;                    // Clear portB interrupt flag
    INTCONbits.RBIE = 1;                    // Enable portB interrupts

    //------------------
    // PWM setup
    CCP1CONbits.CCP1M3 = 1;                 // Enable CCP1 in PWM mode
    CCP1CONbits.CCP1M2 = 1;                 // ...
    CCP1CONbits.CCP1M1 = 0;
    CCP1CONbits.CCP1M0 = 0;

    //-------------------
    // Timer0 interrupt setup
    INTCONbits.TMR0IF = 0;                  // Clear Timer0 interrupt flag
    INTCON2bits.TMR0IP = 0;                 // Timer0 overflow as low priority
    INTCONbits.TMR0IE = 1;                  // Enable Timer0 interrupt

    //-------------------
    // Timer0 setup
    T0CONbits.T08BIT = 0;                   // Timer0 as 16 bit timer
    T0CONbits.T0CS = 0;                     // Timer0 clock source as internal
    T0CONbits.PSA = 1;                      // No prescaler
    T0CONbits.TMR0ON = 1;                   // Turn on Timer0

    //------------------
    // Timer2 setup
    T2CONbits.T2CKPS1 = 1;                  // Timer2 with 1 prescaler
    T2CONbits.T2CKPS0 = 1;                  // ...
    T2CONbits.TMR2ON = 1;                   // Turn on Timer2

    

    
    
}

/*******************************
 * ReadEncoder(void):
 *
 * This subroutine is called by the low priority ISR to sample the encoder. This
 * subroutine checks the current quadrature output of the encoder on PortB RB5
 * (channelA) and PortB RB4 (channelB) and compares with the previous states,
 * CHA and CHB. Based on the quadrature table (stored as a vector in QEM), the
 * new and old quadrature outputs are compared to determine CW,CCW, or encoder
 * bouncing. When NEW_ROT = 1: CW turn, NEW_ROT = -1: CCW turn, NEW_ROT = 0, no
 * turn, NEW_ROT = 2 encoder is bouncing and we don't trust the current output.
 * When a bounce occurs the routine uses the previous rotation as the current
 * rotation and does not adjust CHA,CHB. A bounce can also occur where it apears
 * as though the encoder is rotating in the opposite direction. To deal with this,
 * global variables CCWTurn and CWTurn are used to count how many CCW or CW turns
 * occur over a defined number of counts (EncoderPoll). When EncoderPoll is reached
 * the variable (CCWTurn or CWTurn) with the most counts dictates the rotation of
 * the encoder. Finally if no bouncing occurs, the CHA, CHB, OLD_ROT variables are
 * updated for the next reading.
 *******************************/
void ReadEncoder(void)
{
    //int QEM[] = {0,-1,1,2,1,0,2,-1,-1,2,0,1,2,1,-1,0};
    int Anew = PORTBbits.RB5;           // Read new channel A
    int Bnew = PORTBbits.RB4;           // Read new channel B

    int Old = CHA*2 + CHB;              // Convert previous CHA,CHB into integer value
    int New = Anew*2 + Bnew;            // Convert new A,B into integer value
    int NEW_ROT = QEM[Old*4+New];       // Determine rotation of encoder

    if (NEW_ROT == 2)                   // Shits fucked so don't trust it
    {
        NEW_ROT = OLD_ROT;              // Use previous rotation either +1:CW, -1:CCW, 0
    }
    else
    {
        CHA = Anew;                     // Update state of channel A
        CHB = Bnew;                     // Update state of channel B
    }

    if (NEW_ROT == 1)                   // Update tally of CW turns if NEW_ROT = 1
    {
        CWTurn += 1;
    }
    else if (NEW_ROT == -1)             // Update tally of CCW turns if NEW_ROT = -1
    {
        CCWTurn += 1;
    }

    if (CWTurn + CCWTurn == EncoderPoll)// Once total count is reached determine rotation direction
    {
        if (CWTurn > CCWTurn)           // If majority is CWturning
        {
            PORTAbits.RA3 = 1;
            PORTAbits.RA2 = 0;
            PORTAbits.RA1 = 0;
            PartialRot -= ((double)EncoderPoll)/CountPerRev; // Negative partial rotation
        }
        else if (CWTurn < CCWTurn)      // If majority is CCW turning
        {
            PORTAbits.RA3 = 0;
            PORTAbits.RA2 = 1;
            PORTAbits.RA1 = 0;
            PartialRot += ((double)EncoderPoll)/CountPerRev; // Positive partial rotation
        }
        else
        {
            PORTAbits.RA3 = 0;
            PORTAbits.RA2 = 0;
            PORTAbits.RA1 = 0;
        }

        CWTurn = 0;                     // Reset CW counter
        CCWTurn = 0;                    // Reset CCW counter
    }
    

    OLD_ROT = NEW_ROT;
    
}


/*******************************
 * WriteLCD(int LCDstart, int dispLength, double var)
 *
 * This subroutine takes a numeric variable var, and outputs it to the LCD. Here
 * LCDstart tells the LCD where to place the desired message (hex), and len
 * dictates how long the message is that will be displayed. In order to display
 * the variable, this subroutine uses sprintf to convert the variable into a
 * string with the specified width and #sig figs. Warning: be careful if var
 * has several decimal places of precision. May overwrite other data in memory
 * passed the size of Msg. The if statement prevents displaying anything larger
 * than len+1 to the LCD
 *******************************/
void WriteLCD( int LCDstart, int len, double var, char Msg[] )
{
    char *msgptr = Msg;     // Point to message string
    int width = sprintf(msgptr+1,"%*.*g",len,len-1,var);    // Define minimum # characters as
                            // len, #significant digits len-1
    Msg[0] = LCDstart;      // Set LCD start at beginning of array
    if (width > len)        // If string is larger than desired length, len
    {                       // then output warning symbol '!' to LCD 
        Msg[len] = '!';     // and terminate string at len+1
        Msg[len+1] = '\0';
    }
    DisplayLCD(Msg,0);      // Display the message on the LCD
}