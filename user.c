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
   // T3CONbits.T3CCP1 = 1;                   // Set Ti

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
    T2CONbits.T2CKPS1 = 0;                  // Timer2 with 1 prescaler
    T2CONbits.T2CKPS0 = 0;                  // ...
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
 * LCDstart tells the LCD where to place the desired message (hex), and dispLength
 * dictates how long the message is that will be displayed. In order to display
 * the variable, this subroutine performs divisions by 10 to separate each tens place
 * into the message string, Msg3. The subroutine also takes care of decimal places
 * if needed. 
 *******************************/
void WriteLCD( int LCDstart, int dispLength, double var )
{
    int ii = 1;             // Used to store digits of var into Msg3
    int firstDec = 0;       // Determines if decimal point has been placed
    Msg3[0] = LCDstart;     // Set where message will be placed on LCD
    while (ii<dispLength)   // Iterate over length of message for digits of var
    {
        if (var >= 10)      // If var > 10, divide by 10 to get to single digit
        {
            var = var/10.0;
        }
        else if ( (var < 10) && (var >= 1) ) // If var is between 1 and 10, take the
        {                                   // integer value, this is the digit to
            Msg3[ii] = (int)var + 0x30;     // be displayed. Subtract the var by this
            var = (var - (int)var)*10.0;    // integer value and multiply by 10 to get
            ii++;                           // the next digit. Move to next location in string
        }
        else if ( var < 1)  // If var is a decimal, multiply it by 10 until
        {                   // single digit is reached.
            var = var*10.0;
            if (firstDec == 0)  // If its the first time the decimal is
            {                   // encountered, place a decimal point
                Msg3[ii] = '.';
                firstDec = 1;
                ii++;
            }
            else                // Add zero to hold decimal place and move
            {                   // to next element in string
                Msg3[ii] = '0';
                ii++;
            }
        }
    }
    Msg3[dispLength] = '\0';    // Finally add termination to string message
    DisplayLCD(Msg3,0);         // Display the message on the LCD
}