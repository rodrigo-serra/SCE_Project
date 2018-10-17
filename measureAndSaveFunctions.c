#include "mcc_generated_files/pin_manager.h"
#include "globalvariables.h"
#include "mcc_generated_files/mcc.h"

int get_luminosity (void){
    
    //READ FOR POTENCIOMETER
    int lum = 2;
    
    //Ler do potenciometro
    
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
