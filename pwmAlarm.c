#include "pwmAlarm.h"
#include "mcc_generated_files/pwm6.h"
#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/tmr3.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/memory.h"
#include "globalvariables.h"
#include <math.h>

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

//Function associated with Alarm
void change_PWM(void){
    //WakeUp();
    if(pwm_value == 0)
        PWM_Enable();
    
    int incrementor;
    incrementor = ceil(1000/(TALA/0.2));
    pwm_value += incrementor;
    
    if(blink_ctr == 0){
        PWM6_LoadDutyValue(1020);
        blink_ctr = 1;
    }else{
        PWM6_LoadDutyValue(0);
        blink_ctr = 0;
    }
    //PWM6_LoadDutyValue(pwm_value);
    
    if(pwm_value >= 1000){
        TMR3_StopTimer();
        pwm_value = 0;
        PWM6_LoadDutyValue(1020);
    }
}

//Check if temperature/luminosity level is above its threshold
void checkVariablesForAlarm(int temperature, int luminosity){
    LumThreshold = DATAEE_ReadByte(THRESHLUM);
    TempThreshold = DATAEE_ReadByte(THRESHTEMP);
    
    if((luminosity >= LumThreshold || temperature >= TempThreshold) && ALAF == 1){
        //set alarm control as active                 
        alarm = 1;
    }
    
    if(alarm == 1 && control_alarm == 0 && ALAF == 1){
        //change brightness with pwm for TALA duration

        TMR2_StartTimer();
        TMR3_StartTimer();
        control_alarm = 1;
    }else if((alarm == 0 && control_alarm == 1) || (ALAF == 0 && alarm == 1)){
        if(alarm == 1)
            alarm=0;
        PWM6_LoadDutyValue(0);
        control_alarm = 0;
    }
}