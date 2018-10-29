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
const int PMON = 3;
volatile int ALAF = 0;
volatile int control_ALAF = 0;
const int TALA = 2;
adc_result_t adcResult = 0;
volatile int LumThreshold = 2;
volatile int TempThreshold = 30;

#endif	/* GLOBALVARIABLES_H */

