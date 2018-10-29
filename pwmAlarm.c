#include "pwmAlarm.h"
#include "mcc_generated_files/pwm6.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/tmr3.h"
#include "mcc_generated_files/pin_manager.h"

void PWM_Enable(void){
    PPSLOCK = 0x55;
        PPSLOCK = 0xAA;
        PPSLOCKbits.PPSLOCKED = 0x00;
        
        RA6PPS = 0x0E;    
        
        PPSLOCK = 0x55;
        PPSLOCK = 0xAA;
        PPSLOCKbits.PPSLOCKED = 0x01;
}

void PWM_Disable(void){
    PPSLOCK = 0x55;
        PPSLOCK = 0xAA;
        PPSLOCKbits.PPSLOCKED = 0x00;
        
        RA6PPS = 0x00;    
        
        PPSLOCK = 0x55;
        PPSLOCK = 0xAA;
        PPSLOCKbits.PPSLOCKED = 0x01;
}

void change_PWM(void){

    if(pwm_value == 0)
        PWM_Enable();
    
    pwm_value += 100;
    PWM6_LoadDutyValue(pwm_value);
    if(pwm_value == 1000){
        TMR3_StopTimer();
        pwm_value = 0;
        PWM6_LoadDutyValue(1020);
    }
}