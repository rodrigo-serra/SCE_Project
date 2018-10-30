/* 
 * File:   sleepWakeUp.h
 * Author: Rui
 *
 * Created on October 29, 2018, 2:56 PM
 */

#ifndef SLEEPWAKEUP_H
#define	SLEEPWAKEUP_H

#ifdef	__cplusplus
extern "C" {
#endif


#define WDT_Enable()        (WDTCON0bits.SWDTEN = 1)
#define WDT_Disable()       (WDTCON0bits.SWDTEN = 0)


void GoToSleep(void);
void WakeUp(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SLEEPWAKEUP_H */

