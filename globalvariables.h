/* 
 * File:   globalvariables.h
 * Author: Rui
 *
 * Created on 17 de Outubro de 2018, 13:05
 */

#ifndef GLOBALVARIABLES_H
#define	GLOBALVARIABLES_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#include "mcc_generated_files/adcc.h"

volatile int hrs = 0;
volatile int mins = 0;
volatile int secs = 0;
volatile int PMON = 3;
volatile int ALAF = 0;
volatile int alarm = 0;
volatile int control_alarm = 0;
volatile int TALA = 2;
adc_result_t adcResult = 0;
volatile int LumThreshold = 2;
volatile int TempThreshold = 25;
volatile int temperature = 0 ;
volatile int luminosity = 0;

volatile int s1_pressed = 0;
volatile int mode_s = -1;

const int  max_hour = 24;
const int  max_min = 60;
const int  max_temp =50;
const int  max_lum = 4;


volatile int LED_to_blink;
volatile int blink = 0;

volatile int blink_ctr = 0;


#endif	/* GLOBALVARIABLES_H */

