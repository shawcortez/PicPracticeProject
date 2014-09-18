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

int InitApp(void);

int WaitHalfSec(void);

int DisplayLCD(char * tempPtr, int init);

int InitInterrupts(void);

void ReadEncoder(void);





#endif	/* MYUSER_H */



