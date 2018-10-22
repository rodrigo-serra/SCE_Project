

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

constant int max_hour = 24;
constant int max_min = 60;
constant int max_temp =50;
constant int max_lum = 10;
LED_D4_SetHigh
DATAEE_WriteByte(uint16_t bAdd, uint8_t bData);
DATAEE_ReadByte(uint16_t bAdd);

void s2Pressed(){		
INTERRUPT_GlobalInterruptDisable();

				switch (mode_s1){
				//mudar horas e minutos
				case 0: 
						remainder=incrementor(hour_addr,1,0);
						update_lights(remainder);
					break;
				case 1:	
						remainder=incrementor(hour_addr,0,0);
						update_lights(remainder);
						break;
				case 2:	
						remainder=incrementor(min_addr,1,1);
						update_lights(remainder);
						break;			
				case 3:	
						remainder=incrementor(min_addr,0,1);
						update_lights(remainder);
						break;
						
						
				// mudar temp threshold
				case 4:	
						LED_D3_blink_timer_sop();
						remainder=DATAEE_ReadByte(temp_threshold);
						update_lights(remainder);
						LED_D2_blink_timer_sop();
						mode_s1=5;
						break;
								
				case 5:	
						remainder=incrementor(temp_threshold,1,2);
						update_lights(remainder);
						break;
				case 6:	
						remainder=incrementor(temp_threshold,0,2);
						update_lights(remainder);
						break;
				//lum threshold
				case 7:	
						LED_D2_blink_timer_sop();
						remainder=DATAEE_ReadByte(lum_threshold,0,3);
						update_lights(remainder);
						break;
				case 8: 
						remainder=incrementor(lum_threshold,0,2);
						update_lights(remainder);
						break;	
						
				//iddle state
				case -1: 
						remainder=DATAEE_ReadByte(hour_addr)%10;
						update_lights(remainder);
						mode_s1=0; 
				
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

dynamic int  mode_s1;

void s1Pressed(){
	if(Allarm_is_on){
		//TODO turn off alarm
	}
	else{
		INTERRUPT_GlobalInterruptDisable();
		int remainder;
		mode =0;
		
			switch (mode_s1){
				case 0: 
						remainder=DATAEE_ReadByte(hour_addr);
						update_lights(remainder);
						mode_s1=1;
					break;
				case 1:	
						remainder=DATAEE_ReadByte(min_addr)%10;
						update_lights(remainder);
						mode_s=2;
						break;
				case 2:	
						remainder=DATAEE_ReadByte(min_addr);
						update_lights(remainder);
						mode_s=3;
						break;			
				case 3:	
						update_lights(remainder);
						mode_s=-1;
						break;
				
				
				// mudar temp threshold
				case 4:	
						LED_D2_blink_timer_start();
						mode_s=7;
						break;
								
				case 5:	
						LED_D3_blink_timer_sop();
						remainder=DATAEE_ReadByte(temp_threshold);
						update_lights(remainder);						
						mode_s=6;
						break;
				case 6:	
						mode_s=7;
						break;
				//mudar lum threshold
				
				case 7:	
						LED_D2_blink_timer_sop();			
						mode_s=-1;

						break;
				case 8:	
						LED_D2_blink_timer_sop();				
						mode_s=-1;
						break;
				case -1: 
						LED_D3_blink_timer_start();
						break;
			}
		__delay_ms(50);
		INTERRUPT_GlobalInterruptEnable();

    }
}
