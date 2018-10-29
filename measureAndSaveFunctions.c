#include "mcc_generated_files/pin_manager.h"
#include "globalvariables.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/adcc.h"
#include "mcc_generated_files/memory.h"

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
        lastLumEntry = DATAEE_ReadByte(START_INDEX + lastRegSaved-1 + LUM);
        lastTempEntry = DATAEE_ReadByte(START_INDEX + lastRegSaved-1 + TEMP);
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
        }else{
            updateNumRegsSaved = numRegsSaved;
        }
        
        //Now the values are saved in the registers
        DATAEE_WriteByte(LAST_WRITTEN, newRegIndex);
        DATAEE_WriteByte(NUM_REGS_SAVED, updateNumRegsSaved);
        
        DATAEE_WriteByte(START_INDEX + lastRegSaved + HOURS, localHour);
        DATAEE_WriteByte(START_INDEX + lastRegSaved + MINUTES, localMin);
        DATAEE_WriteByte(START_INDEX + lastRegSaved + SECONDS, localSec);
        DATAEE_WriteByte(START_INDEX + lastRegSaved + TEMP, temp);
        DATAEE_WriteByte(START_INDEX + lastRegSaved + LUM, lum);
    }
    
    //Check if it's the max or the min and if so, save it
    int maxTemp = DATAEE_ReadByte(MAXTEMP + TEMP);
    int maxLum = DATAEE_ReadByte(MAXLUM + LUM);
    int minTemp = DATAEE_ReadByte(MINTEMP + TEMP);
    int minLum = DATAEE_ReadByte(MINLUM + LUM);

    if(temp >= maxTemp){
        DATAEE_WriteByte(MAXTEMP + HOUR, localHour);
        DATAEE_WriteByte(MAXTEMP + MINUTES, localMin);
        DATAEE_WriteByte(MAXTEMP + SECONDS, localSec);
        DATAEE_WriteByte(MAXTEMP + TEMP, temp);
        DATAEE_WriteByte(MAXTEMP + LUM, lum);
    }

    if(lum >= maxLum){
        DATAEE_WriteByte(MAXLUM + HOUR, localHour);
        DATAEE_WriteByte(MAXLUM + MINUTES, localMin);
        DATAEE_WriteByte(MAXLUM + SECONDS, localSec);
        DATAEE_WriteByte(MAXLUM + TEMP, temp);
        DATAEE_WriteByte(MAXLUM + LUM, lum);
    }

    if(temp <= minTemp){
        DATAEE_WriteByte(MINTEMP + HOUR, localHour);
        DATAEE_WriteByte(MINTEMP + MINUTES, localMin);
        DATAEE_WriteByte(MINTEMP + SECONDS, localSec);
        DATAEE_WriteByte(MINTEMP + TEMP, temp);
        DATAEE_WriteByte(MINTEMP + LUM, lum);
    }

    if(lum <= minLum){
        DATAEE_WriteByte(MINLUM + HOUR, localHour);
        DATAEE_WriteByte(MINLUM + MINUTES, localMin);
        DATAEE_WriteByte(MINLUM + SECONDS, localSec);
        DATAEE_WriteByte(MINLUM + TEMP, temp);
        DATAEE_WriteByte(MINLUM + LUM, lum);
    }


    if(temp >= TempThreshold || lum >= LumThreshold){
        //set alarm control as active                 
        ALAF = 1;
    }
    
}

/*void sensor_timer(){
		int lum, temp;
		
		if(lum!=get_last_entry(vector)[4] || temp !=get_last_entry(vector)[3]){
			char * new_entry=malloc(sizeof(char[5]));
			new_entry[0]=watch->hours;
			new_entry[1]=watch->minutes;
			new_entry[2]=watch->seconds;
			new_entry[3]=temp;
			new_entry[4]=lum;
			free(add_entry(data_vect,new_entry));
			
			if(temp>MAX_TEMP)
				MAX_TEMP=temp;
			if(lum>MAX_LUM)
				MAX_LUM=lum;
			if(temp>MIN_TEMP)
				MIN_TEMP=temp;
			if(lum>MIN_LUM)
				MIN_LUM=lum;
			
			if(temp>=MAX_allowed_temp || temp<=MIN_allowed_temp || lum>MAX_allowed_lum || lum<MIN_allowed_lum){
					start_Danger_timer();
					set_LED_min_ramp_luminosity;
                    //activate alarm variable 
				}
			}
		
	}*/
