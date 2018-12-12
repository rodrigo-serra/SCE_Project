#include "mcc_generated_files/pin_manager.h"
#include "globalvariables.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/adcc.h"
#include "mcc_generated_files/memory.h"
#include "mcc_generated_files/eusart.h"
#include "coms.h"

//Reads luminosity from potenciometer
int get_luminosity (void){
    
    //READ FOR POTENCIOMETER
    int lum = 2;
    
    //Ler do potenciometro
    adcResult = ADCC_GetSingleConversion(POT);
    if(adcResult < 256){
        lum = 0;
    }else if(adcResult < 512){
        lum = 1;
    } else if(adcResult < 768){
        lum = 2;
    }else{
        lum = 3;
    }
    
    return lum;
    
}

//Update Leds according to the luminosity level
void setLedLuminosity(int lum){
    if(lum > 1){
        LED_D3_SetHigh();
    }else{
        LED_D3_SetLow();
    }
    
    if(lum%2 == 1)
    {
        LED_D2_SetHigh();
    }else{
        LED_D2_SetLow();
    }
}

//sensor timer interrupt
// time info, max temp, min temp, data vector and max/min registers
void sensor_timer(int lum, int temp){
    //get the values from the registers of how many regs are saved and who was the last one 
    int numRegsSaved = DATAEE_ReadByte(NUM_REGS_SAVED);
    int lastRegSaved = DATAEE_ReadByte(LAST_WRITTEN);
    
    //check if it connected for the first time
    if( numRegsSaved == 0 || ((numRegsSaved != lastRegSaved) && numRegsSaved != 20) || numRegsSaved > 20 || lastRegSaved > 20){
        numRegsSaved = 0;
        lastRegSaved = 0;
        DATAEE_WriteByte(NUM_REGS_SAVED, numRegsSaved);
        DATAEE_WriteByte(LAST_WRITTEN, lastRegSaved);
        DATAEE_WriteByte(MAXTEMP + TEMP, 0);
        DATAEE_WriteByte(MAXLUM + LUM, 0);
        DATAEE_WriteByte(MINTEMP + TEMP, 200);
        DATAEE_WriteByte(MINLUM + LUM, 5);
    }
    
    //get the time to save interrupting the timer for the time we do it to prevent corrupted values
    TMR1_StopTimer();
    int localHour = hrs;
    int localMin = mins;
    int localSec = secs;
    TMR1_StartTimer();
    
    //Save time
    DATAEE_WriteByte(HOUR, localHour);
    DATAEE_WriteByte(MINUTE, localMin);
    
    int lastLumEntry = 0;
    int lastTempEntry = 0;
    
    if(numRegsSaved != 0){
        if(lastRegSaved == 0){
            lastLumEntry = DATAEE_ReadByte(START_INDEX + 19*5 + LUM);
            lastTempEntry = DATAEE_ReadByte(START_INDEX + 19*5 + TEMP);
        }else{
            lastLumEntry = DATAEE_ReadByte(START_INDEX + (lastRegSaved-1)*5 + LUM);
            lastTempEntry = DATAEE_ReadByte(START_INDEX + (lastRegSaved-1)*5 + TEMP);
        }
    }
    
    if(((lum != lastLumEntry || temp != lastTempEntry) && numRegsSaved != 0) || numRegsSaved == 0){
        //change the register
        int newRegIndex = 0;
        if(lastRegSaved >= 19){
            newRegIndex = 0;
        }else{
            newRegIndex = lastRegSaved + 1;
        }
        
        //change the number of regs
        int updateNumRegsSaved = 0;
        if(numRegsSaved < 20)
        {
            updateNumRegsSaved = numRegsSaved + 1;
            if(updateNumRegsSaved == 10){
                writebytes(SOM);
                
                writebytes(NMFL);
               //NREG
                writebytes(20);
                //Registos validos
                writebytes(DATAEE_ReadByte(NUM_REGS_SAVED));
                //iread
                writebytes(DATAEE_ReadByte(LAST_READ));
                //iwrite
                writebytes(DATAEE_ReadByte(LAST_WRITTEN));
                
                writebytes(EOM);
            }
        }else{
            updateNumRegsSaved = numRegsSaved;
        }
        
        //Now the values are saved in the registers
        DATAEE_WriteByte(LAST_WRITTEN, newRegIndex);
        DATAEE_WriteByte(NUM_REGS_SAVED, updateNumRegsSaved);
        
        DATAEE_WriteByte(START_INDEX + lastRegSaved*5 + HOURS, localHour);
        DATAEE_WriteByte(START_INDEX + lastRegSaved*5 + MINUTES, localMin);
        DATAEE_WriteByte(START_INDEX + lastRegSaved*5 + SECONDS, localSec);
        DATAEE_WriteByte(START_INDEX + lastRegSaved*5 + TEMP, temp);
        DATAEE_WriteByte(START_INDEX + lastRegSaved*5 + LUM, lum);
    }
    
    //Check if it's the max or the min and if so, save it
    int maxTemp = DATAEE_ReadByte(MAXTEMP + TEMP);
    int maxLum = DATAEE_ReadByte(MAXLUM + LUM);
    int minTemp = DATAEE_ReadByte(MINTEMP + TEMP);
    int minLum = DATAEE_ReadByte(MINLUM + LUM);

    if(temp >= maxTemp){
        DATAEE_WriteByte(MAXTEMP + HOURS, localHour);
        DATAEE_WriteByte(MAXTEMP + MINUTES, localMin);
        DATAEE_WriteByte(MAXTEMP + SECONDS, localSec);
        DATAEE_WriteByte(MAXTEMP + TEMP, temp);
        DATAEE_WriteByte(MAXTEMP + LUM, lum);
    }

    if(lum >= maxLum){
        DATAEE_WriteByte(MAXLUM + HOURS, localHour);
        DATAEE_WriteByte(MAXLUM + MINUTES, localMin);
        DATAEE_WriteByte(MAXLUM + SECONDS, localSec);
        DATAEE_WriteByte(MAXLUM + TEMP, temp);
        DATAEE_WriteByte(MAXLUM + LUM, lum);
    }

    if(temp <= minTemp){
        DATAEE_WriteByte(MINTEMP + HOURS, localHour);
        DATAEE_WriteByte(MINTEMP + MINUTES, localMin);
        DATAEE_WriteByte(MINTEMP + SECONDS, localSec);
        DATAEE_WriteByte(MINTEMP + TEMP, temp);
        DATAEE_WriteByte(MINTEMP + LUM, lum);
    }

    if(lum <= minLum){
        DATAEE_WriteByte(MINLUM + HOURS, localHour);
        DATAEE_WriteByte(MINLUM + MINUTES, localMin);
        DATAEE_WriteByte(MINLUM + SECONDS, localSec);
        DATAEE_WriteByte(MINLUM + TEMP, temp);
        DATAEE_WriteByte(MINLUM + LUM, lum);
    }
    
}


void getValuesFromPreviousSession(){
    //check if it is the first session
    int numRegsSaved = DATAEE_ReadByte(NUM_REGS_SAVED);
    int lastRegSaved = DATAEE_ReadByte(LAST_WRITTEN);

    //check if it connected for the first time
    if( (numRegsSaved == 20 || ((numRegsSaved == lastRegSaved) && numRegsSaved != 20)) && numRegsSaved <= 20 && lastRegSaved <= 20){
    //update time if not
        hrs = DATAEE_ReadByte(HOUR);
        mins = DATAEE_ReadByte(MINUTE);
        LumThreshold = DATAEE_ReadByte(THRESHLUM);
        TempThreshold = DATAEE_ReadByte(THRESHTEMP);
        PMON = DATAEE_ReadByte(PMONvalue);
        TALA = DATAEE_ReadByte(TALAvalue); 
        ALAF = DATAEE_ReadByte(ALAFvalue); 
    }else{
        DATAEE_WriteByte(THRESHLUM, LumThreshold);
        DATAEE_WriteByte(THRESHTEMP, TempThreshold);
        DATAEE_WriteByte(PMONvalue, PMON);
        DATAEE_WriteByte(TALAvalue, TALA);
        DATAEE_WriteByte(TALAvalue, ALAF);
    }
}