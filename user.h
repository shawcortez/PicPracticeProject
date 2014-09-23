/* 
 * File:   user.h
 * Author: wenceslaoshaw-cortezshaw-cortez
 *
 * Created on September 11, 2014, 2:31 PM
 */

#ifndef MYUSER_H
#define	MYUSER_H

/*----------------------------------------------------------------------------*/
/*Function Prototypes*/
/*----------------------------------------------------------------------------*/

void InitApp(void);

void WaitHalfSec(void);

void DisplayLCD(char * tempPtr, int init);

void InitInterrupts(void);

void ReadEncoder(void);

void WriteLCD(int LCDstart, int len, double var, char Msg[]);




#endif	/* MYUSER_H */



