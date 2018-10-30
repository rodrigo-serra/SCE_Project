#include "measureAndSaveFunctions.h"
#include "mcc_generated_files/pin_manager.h"
#include "globalvariables.h"
#include "mcc_generated_files/mcc.h"
#include "stateModifiers.h"

void changeleds(void){
    //WakeUp();
    secs +=1;
    if(secs == 60)
    {
        mins += 1;
        secs = 0;
    }
    if(mins == 60)
    {
        hrs += 1;
        mins = 0;
    }
    
    if(LED_D5_GetValue() == LOW)
    {
        LED_D5_SetHigh();
    }else{
        LED_D5_SetLow();
    }
}

void s1PressedInterruptHandler(void){
   
    if(alarm == 1){
        alarm = 0;
        PWM6_LoadDutyValue(0);
    }else{
        if(mode_s == -1){
            clearLeds();
            LED_to_blink=5;
            mode_s = 0;
        }else{
            s1_pressed = 1;
        }
    }
    
    
    __delay_ms(5);
}

void LED_blink_function(void){
    if (LED_to_blink!=-1){
        if (!blink){
            switch(LED_to_blink){
                case 2:	LED_D2_SetHigh(); break;
                case 3:	LED_D3_SetHigh(); break;
                case 4:	PWM6_LoadDutyValue(1020); break;
                case 5:	LED_D5_SetHigh(); break;
            }
            blink=1;
        }
        else{            
            switch(LED_to_blink){
                case 2:	LED_D2_SetLow(); break;
                case 3:	LED_D3_SetLow(); break;
                case 4:	PWM6_LoadDutyValue(0); break;
                case 5:	LED_D5_SetLow(); break;
            }
            blink=0;
        }
    }
}