#include "mcc_generated_files/memory.h"
#include "mcc_generated_files/interrupt_manager.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/mcc.h"
#include "stateModifiers.h"
#include "globalvariables.h"

void update_lights(int number ){
    clearLeds();
		if(number & 1)
			LED_D2_SetHigh();
        else 
            LED_D2_SetLow();
		if(number & 2)
			LED_D3_SetHigh();
        else
            LED_D3_SetLow();
		if(number & 4)
            PWM6_LoadDutyValue(1023);
		else
            PWM6_LoadDutyValue(0);
        if(number & 8)
			LED_D5_SetHigh();
        else 
            LED_D5_SetLow();
}
int incrementor(uint16_t t_addr,int decimal,int mode){
	int remainder, top_val, lowvals, highvals;
	switch (mode){
		case 0:top_val=	max_hour; break;
		case 1:top_val=	max_min; break;
		case 2:top_val=	max_temp; break;
		case 3:top_val=	max_lum; break;
	}
	
	if(decimal){
        remainder = DATAEE_ReadByte(t_addr) + 10;
		if(mode!= 3){   
            if((((remainder/10) >= (top_val/10)) && (mode == 1 || mode == 2)) || (((remainder/10) > (top_val/10)) && (mode == 0))){
                remainder =  0 + remainder%10;
            }
            
            DATAEE_WriteByte(t_addr, remainder);
        }
        
		return remainder;
	}
	else{		
		remainder = DATAEE_ReadByte(t_addr);        
        highvals = remainder/10;
        lowvals = remainder % 10;
		if((lowvals == 9 && highvals+1<top_val/10 && (mode == 1 || mode == 2)) || (lowvals == 9 && highvals+1<3 && mode == 0) || (lowvals == 3 && highvals==2 && mode == 0)){
			lowvals = 0 ;
            remainder = highvals*10 + lowvals;
        }else if(lowvals == 3 && mode == 3){
            remainder = 0;
        }else
            remainder += 1;
		DATAEE_WriteByte(t_addr, remainder);
		return remainder;
	}
	
}

//





void s2Pressed(){		

    int remainder;

    switch (mode_s){
    //mudar horas e minutos
        case 0: 
            clearLeds();
            remainder=DATAEE_ReadByte(HOUR)/10;
            update_lights(remainder);
            mode_s=1;
            LED_to_blink=-1;          
            break;
        case 1: 
            remainder=incrementor(HOUR,1,0);
            update_lights(remainder/10);
            break;
        case 2:	
            remainder=incrementor(HOUR,0,0);
            update_lights(remainder%10);
            break;
        case 3:	
            remainder=incrementor(MINUTE,1,1);
            update_lights(remainder/10);
            break;			
        case 4:	
            remainder=incrementor(MINUTE,0,1);
            update_lights(remainder%10);
            break;
        //Mudar alarme
        case 5: 
            clearLeds();
            LED_to_blink=-1;
            if(ALAF){
                LED_D2_SetHigh();
            }
            else {
                LED_D2_SetLow();
            }
            mode_s =6;
            break;

        case 6: 
            if(ALAF == 1){
                ALAF=0;
                LED_D2_SetLow();
            }else{
                ALAF=1;
                LED_D2_SetHigh();                       
            }
            DATAEE_WriteByte(ALAFvalue,ALAF);
            break;	
        // mudar temp threshold
        case 7:	
            clearLeds();
            remainder=DATAEE_ReadByte(THRESHTEMP)/10;
            update_lights(remainder);
            LED_to_blink=-1;
            mode_s=8;
            break;
        case 8:	
            remainder=incrementor(THRESHTEMP,1,2);
            update_lights(remainder/10);
            break;
        case 9:	
            remainder=incrementor(THRESHTEMP,0,2);
            update_lights(remainder%10);
            break;
        //lum threshold
        case 10:	
            //LED_D2_blink_timer_stop();
            clearLeds();
            LED_to_blink=-1;
            remainder=DATAEE_ReadByte(THRESHLUM);
            update_lights(remainder);
            mode_s=11;
            break;
        case 11: 
            remainder=incrementor(THRESHLUM,0,3);
            update_lights(remainder);
            break;	

        //iddle state
        case -1: 

            break;
    }
}




void s1Pressed(){
    int remainder;

    switch (mode_s){
        case 0: //LED_D4_blink_timer_start();
            clearLeds();
            LED_to_blink=4;
            mode_s=5;
            break;
        case 1: 
            remainder=DATAEE_ReadByte(HOUR)%10;
            update_lights(remainder);
            mode_s=2;
            break;
        case 2:	
            remainder=DATAEE_ReadByte(MINUTE);
            update_lights(remainder/10);
            mode_s=3;
            break;
        case 3:	
            remainder=DATAEE_ReadByte(MINUTE)%10;
            update_lights(remainder);
            mode_s=4;
            break;			
        case 4:	
            
            /*update_lights(remainder);
            mode_s=5;*/
            mode_s=5;
            clearLeds();
            LED_to_blink=4;
            break;
        //MUDAR ALAF
        case 5: 
            mode_s=7;
            clearLeds();
            LED_to_blink=3;
            break;
        case 6: 
            mode_s =7;
            clearLeds();
            LED_to_blink=3;
            break;
        // mudar temp threshold
        case 7:	
            clearLeds();
            LED_to_blink=2;
            mode_s=10;
            break;

        case 8:	
            //LED_D3_blink_timer_stop();
            clearLeds();
            LED_to_blink=-1;
            remainder=DATAEE_ReadByte(THRESHTEMP)%10;
            update_lights(remainder);						
            mode_s=9;
            break;
        case 9:	
            clearLeds();
            LED_to_blink=2;
            mode_s=10;
            break;
        //mudar lum threshold

        case 10:	
            //LED_D2_blink_timer_stop();
            clearLeds();
            LED_to_blink=-1;			
            mode_s=-1;

            break;
        case 11:	
            //LED_D2_blink_timer_stop();
            clearLeds();
            LED_to_blink=-1;
            mode_s=-1;
            break;
        case -1: 
            //LED_D3_blink_timer_start();
            clearLeds();
            LED_to_blink=3;
            break;
        mode_s=0;
    }
}

void clearLeds(void){
    LED_D2_SetLow();
    LED_D3_SetLow();
    PWM6_LoadDutyValue(0);
    LED_D5_SetLow();
}

void updateGlobalVariables(void){
    hrs = DATAEE_ReadByte(HOUR);
    mins = DATAEE_ReadByte(MINUTE);
    LumThreshold = DATAEE_ReadByte(THRESHLUM);
    TempThreshold = DATAEE_ReadByte(THRESHTEMP);
}