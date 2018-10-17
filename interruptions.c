#include "measureAndSaveFunctions.h"
#include "mcc_generated_files/pin_manager.h"
#include "globalvariables.h"
#include "mcc_generated_files/mcc.h"

void changeleds(void){
    
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

void s1Pressed(){
    if(LED_D4_GetValue() == LOW)
    {
        LED_D4_SetHigh();
        
    }else{
        LED_D4_SetLow();
        
    }
    
    __delay_ms(5);
}
