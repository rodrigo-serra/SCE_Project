/* 
 * File:   globalvariables.h
 */

#ifndef GLOBALVARIABLES_H
#define	GLOBALVARIABLES_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>

#define SOM 0xFD /* start of message */
#define EOM 0xFE /* end of message */
#define RCLK 0xC0 /* read clock */
#define SCLK 0XC1 /* set clock */
#define RTL 0XC2 /* read temperature and luminosity */
#define RPAR 0XC3 /* read parameters */
#define MMP 0XC4 /* modify monitoring period */
#define MTA 0XC5 /* modify time alarm */
#define RALA 0XC6 /* read alarms (temperature, luminosity, active/inactive) */
#define DATL 0XC7 /* define alarm temperature and luminosity */
#define AALA 0XC8 /* activate/deactivate alarms */
#define RMM 0XC9 /* read maximums and minimus registers */
#define CMM 0XCA /* clear maximums and minimus registers */
#define IREG 0XCB /* information about registers (NREG, nr, iread, iwrite)*/
#define TRGC 0XCC /* transfer registers (curr. position)*/
#define TRGI 0XCD /* transfer registers (index) */
#define NMFL 0XCE /* notification memory (half) full */
#define CMD_OK 0 /* command successful */
#define CMD_ERROR 0xFF /* error in command */

//Variáveis globais de controlo
extern volatile int registerRequest;
extern volatile int makeCalculations;
extern volatile int LumThreshold, TempThreshold;
extern volatile int exitControl;
extern volatile int p;

//Variáveis dos registos
extern volatile int iread, iwrite, nr;
extern int NRBUF;
extern volatile int registers[100][5]; 

/*error variable*/
extern volatile Cyg_ErrNo err;
extern volatile cyg_io_handle_t serH;

/*Mail boxes handlers and objects for receiver and sender*/
extern cyg_handle_t mbxSh, mbxRh, mbxIPh, mbxPIh;
extern cyg_mbox mbxS, mbxR, mbxIP, mbxPI;

/* and now a mutex to protect calls to the C library */
extern cyg_mutex_t cliblock;

 struct cloc_{
	int hours;
	int minutes;
	int seconds;
	};
typedef struct cloc_ * cloc;


#endif	/* GLOBALVARIABLES_H */

