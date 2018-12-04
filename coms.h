/* 
 * File:   coms.h
 * Author: Rodrigo
 *
 * Created on 23 de Novembro de 2018, 16:11
 */

#ifndef COMS_H
#define	COMS_H

#ifdef	__cplusplus
extern "C" {
#endif


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
    

volatile int msg_array[20];
volatile int pointer_array=0;

void readbytes(void);
void writebytes(uint8_t data);
void interp_msg(void);


#ifdef	__cplusplus
}
#endif

#endif	/* COMS_H */

