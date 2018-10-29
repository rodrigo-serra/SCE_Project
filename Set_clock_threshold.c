#include "memory.h"
#include "interrupt_manager.h"
#include "pin_manager.h"

int incrementor(uint16_t t_addr,int decimal,int mode){
	int remainder, top_val;
	switch (mode){
		case 0:top_val=	max_hour; break;
		case 1:top_val=	max_min; break;
		case 2:top_val=	max_temp; break;
		case 3:top_val=	max_lum; break;
	}
	
	if(decimal){
		remainder = DATAEE_ReadByte(t_addr)+10;
		if(remainder >=top_val)
			remainder = 0 + remainder % 10 
		DATAEE_WriteByte(t_addr, remainder);
		return remainder;
	}
	else{		
		remainder = DATAEE_ReadByte(t_addr)+1;
		if(remainder >=top_val)
			remainder = 0 
		DATAEE_WriteByte(t_addr, remainder);
		return remainder;
	}
	
}

//

const int  max_hour = 24;
const int  max_min = 60;
const int  max_temp =50;
const int  max_lum = 10;

volatile int  mode_s;



void s2Pressed(){		
INTERRUPT_GlobalInterruptDisable();
int remainder;

				switch (mode_s){
				//mudar horas e minutos
				case 1: 
						remainder=incrementor(HOUR,1,0);
						update_lights(remainder);
					break;
				case 2:	
						remainder=incrementor(HOUR,0,0);
						update_lights(remainder);
						break;
				case 3:	
						remainder=incrementor(MINUTE,1,1);
						update_lights(remainder);
						break;			
				case 4:	
						remainder=incrementor(MINUTE,0,1);
						update_lights(remainder);
						break;
                //Mudar alarme
                    case 5: if(ALAF){
                        			LED_D2_SetHigh();

                    }
                    else {
                        			LED_D2_SetLow();

                    }
                        break;
                        
					case 6: //TODO, tem que mudar o valor de ALAF e 
                            //mudar o LED como tal
                        break;	
				// mudar temp threshold
				case 7:	
						LED_D3_blink_timer_stop();
						remainder=DATAEE_ReadByte(THRESHTEMP);
						update_lights(remainder);
						LED_D2_blink_timer_stop();
						mode_s=8;
						break;
								
				case 8:	
						remainder=incrementor(THRESHTEMP,1,2);
						update_lights(remainder);
						break;
				case 9:	
						remainder=incrementor(THRESHTEMP,0,2);
						update_lights(remainder);
						break;
				//lum threshold
				case 10:	
						LED_D2_blink_timer_stop();
						remainder=DATAEE_ReadByte(THRESHLUM,0,3);
						update_lights(remainder);
						mode_s=11;
						break;
				case 11: 
						remainder=incrementor(THRESHLUM,0,2);
						update_lights(remainder);
						break;	
						
				//iddle state
				case -1: 
						
						break;
			}
			
			INTERRUPT_GlobalInterruptEnable();
}

void update_lights(int number ){
		if(number & 1)
			LED_D2_SetHigh();
		if(number & 2)
			LED_D3_SetHigh();
		if(number & 4)
			LED_D4_SetHigh();
		if(number & 8)
			LED_D5_SetHigh();
}


void s1Pressed(){
	if(Alarm_is_on){
		//TODO turn off alarm
	}
	else{
		INTERRUPT_GlobalInterruptDisable();
		int remainder;
		
			switch (mode_s){
                case 0: LED_D4_blink_timer_start();
                        mode_s=5;
                        break;
				case 1: 
						remainder=DATAEE_ReadByte(HOUR);
						update_lights(remainder);
						mode_s=2;
					break;
				case 2:	
						remainder=DATAEE_ReadByte(MINUTE)%10;
						update_lights(remainder);
						mode_s=3;
						break;
				case 3:	
						remainder=DATAEE_ReadByte(MINUTE);
						update_lights(remainder);
						mode_s=4;
						break;			
				case 4:	
						update_lights(remainder);
						mode_s=5;
						break;
				//MUDAR ALAF
                case 5: break;// TODO
                case 6: break;// TODO
				// mudar temp threshold
				case 7:	
						LED_D2_blink_timer_start();
						mode_s=10;
						break;
								
				case 8:	
						LED_D3_blink_timer_stop();
						remainder=DATAEE_ReadByte(THRESHTEMP);
						update_lights(remainder);						
						mode_s=9;
						break;
				case 9:	
						mode_s=10;
						break;
				//mudar lum threshold
				
				case 10:	
						LED_D2_blink_timer_stop();			
						mode_s=-1;

						break;
				case 11:	
						LED_D2_blink_timer_stop();				
						mode_s=-1;
						break;
				case -1: 
						LED_D3_blink_timer_start();
						break;
                        mode_s=0;
			}
		__delay_ms(50);
		INTERRUPT_GlobalInterruptEnable();

    }
}

