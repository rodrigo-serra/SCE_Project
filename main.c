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
#include "interruptions.h"
#include "globalvariables.h"
#include "measureAndSaveFunctions.h"
#include "mcc_generated_files/adcc.h"
#include "pwmAlarm.h"
#include "i2c.h"
#include "stateModifiers.h"
#include "coms.h"

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
 
    //initialize I2C
    i2c1_driver_open();
    I2C_SCL = 1;
    I2C_SDA = 1;
    WPUC3 = 1;
    WPUC4 = 1;
    
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
    
    TMR4_SetInterruptHandler(&LED_blink_function);
    
    INT_SetInterruptHandler(&s1PressedInterruptHandler);
    
    TMR3_SetInterruptHandler(&change_PWM);
    TMR3_StopTimer();
    TMR4_StopTimer();
    PWM_Enable();
    PWM6_LoadDutyValue(0);
    
   
    unsigned char c;
    TempThreshold = 25;
    
    //checks if it's a reset to resume the tim
    getValuesFromPreviousSession();
    
    while (1)
    {   
        if(mode_s == -1){
            if(secs%PMON == 0){
                //get luminosity lvl
                luminosity = get_luminosity();
                setLedLuminosity(luminosity); 
                
                checkVariablesForAlarm(0,luminosity);

                //if(alarm == 0)
                    //SLEEP();
             
            }

            if(secs%PMON == 1){
                //get temperature
                NOP();
                c = tsttc();       	
                temperature = c;
                NOP();
                 
                checkVariablesForAlarm(temperature,0);                        
                
                //if(alarm == 0)
                   //SLEEP();
                 
            }

            if(secs%PMON == 2){
                //call function to save (it checks if its a new value or not --
                //                       or max or min -- checks thresholds)
                sensor_timer(luminosity, temperature);
                //check if the alarm has been turned off
                checkVariablesForAlarm(0,0);
            
                //SLEEP();   
            }
            //Leitura eCos
            while(EUSART_is_rx_ready() != 0){
                readbytes();
            }
            
        }else{
            //call function to modify states
           /* TMR1_StopTimer();
            TMR4_StartTimer();
            int previous = HIGH;
            while(mode_s != -1){
                if(S2_GetValue() == LOW && previous == HIGH){
                    s2Pressed();
                    __delay_ms(50);
                    previous = LOW;
                }else {
                    if(S2_GetValue() == HIGH && previous == LOW)
                        previous = HIGH;
                }
                if(s1_pressed == 1){
                    s1_pressed = 0;
                    s1Pressed();
                    __delay_ms(50);
                }
            }
            TMR4_StopTimer();
            TMR1_StartTimer();
            updateGlobalVariables();*/
        }    
    }
}

