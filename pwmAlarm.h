/* 
 * File:   pwmAlarm.h
 * Author: SCE
 *
 * Created on October 23, 2018, 2:42 PM
 */

#ifndef PWMALARM_H
#define	PWMALARM_H

#ifdef	__cplusplus
extern "C" {
#endif

volatile int pwm_value = 0;



#ifdef	__cplusplus
}
#endif

#endif	/* PWMALARM_H */

void PWM_Enable(void);

void PWM_Disable(void);
void change_PWM(void);
void checkVariablesForAlarm(int temperature, int luminosity);