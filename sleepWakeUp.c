#include "sleepWakeUp.h"
#include "mcc_generated_files/mcc.h"

void GoToSleep(void) {

    WDT_Enable();

    SLEEP();

}

void WakeUp(void){
    WDT_Disable();
}
        
