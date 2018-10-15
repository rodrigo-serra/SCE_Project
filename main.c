/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC16F18875
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/pin_manager.h"

/*
                         Main application
 */
volatile int hrs = 0;
volatile int mins = 0;
volatile int secs = 0;
const int PMON = 3;

void changeleds(void){
    
    //Change LED
  
    
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
    
    if(LED_D2_GetValue() == LOW)
    {
        LED_D2_SetHigh();
    }else{
        LED_D2_SetLow();
    }
}

void s1Pressed(){
    if(LED_D4_GetValue() == LOW)
    {
        LED_D4_SetHigh();
        TMR1_StartTimer();
    }else{
        LED_D4_SetLow();
        TMR1_StopTimer();
    }
    
    __delay_ms(5);
}

void sensor_timer();

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
 
    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();
   
    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    TMR1_SetInterruptHandler(&changeleds);
    
    INT_SetInterruptHandler(&s1Pressed);

    while (1)
    {
        
        LED_D3_SetHigh();
        
        
        
        if(secs%PMON == 0){
            //get luminosity lvl
            
        }
        
        if(secs%PMON == 1){
            //get temperature
        }
        
        if(secs%PMON == 2){
            //call function to save (it checks if its a new value or not --
            //                       or max or min -- checks thresholds)
        
            //if alarm is on call function to change brightness
        }
        
        
        
        
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