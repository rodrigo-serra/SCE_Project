#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "globalvariables.h"

/* now declare (and allocate space for) some kernel objects,
   like the two threads we will use */
cyg_thread thread_s[4];		/* space for two thread objects */

char stack[4][4096];		/* space for two 4K stacks */

/* now the handles for the threads */
cyg_handle_t interface_thread, sender_thread, receiver_thread, processing_thread;

/* and now variables for the procedure which is the thread */
cyg_thread_entry_t interface_program;
cyg_thread_entry_t sender_program;
cyg_thread_entry_t receiver_program;
cyg_thread_entry_t processing_program;

/*declare external functions*/
extern void monitor (void);
extern void cmd_ini(int argc, char **argv);
extern void checkThresholds(int n);
extern int saveRegister(int registo[5]);

//Variáveis globais de controlo
volatile int registerRequest = 0;
volatile int makeCalculations = 0;
volatile int LumThreshold = 2, TempThreshold = 30;
volatile int exitControl = 0;
volatile int p = 10;

//Variáveis dos registos
volatile int iread = 0, iwrite = -1, nr = 0;
int NRBUF = 100;
volatile int registers[100][5]; 

/*error variable*/
volatile Cyg_ErrNo err;
volatile cyg_io_handle_t serH;

/*Mail boxes handlers and objects for receiver and sender*/
cyg_handle_t mbxSh, mbxRh, mbxIPh, mbxPIh;
cyg_mbox mbxS, mbxR, mbxIP, mbxPI;

/* and now a mutex to protect calls to the C library */
cyg_mutex_t cliblock;

/* we install our own startup routine which sets up threads */
void cyg_user_start(void)
{
	printf("Start Server\n");

	cyg_mutex_init(&cliblock);
	
	cyg_mbox_create( &mbxSh, &mbxS);
	cyg_mbox_create( &mbxRh, &mbxR);
	cyg_mbox_create( &mbxIPh, &mbxIP);
	cyg_mbox_create( &mbxPIh, &mbxPI);

	cyg_thread_create(5, interface_program, (cyg_addrword_t) 0,
		"Interface Thread", (void *) stack[0], 4096,
		&interface_thread, &thread_s[0]);
	cyg_thread_create(4, sender_program, (cyg_addrword_t) 1,
		"Sender Thread", (void *) stack[1], 4096,
		&sender_thread, &thread_s[1]);
	cyg_thread_create(3, receiver_program, (cyg_addrword_t) 2,
		"Receiver Thread", (void *) stack[2], 4096,
		&receiver_thread, &thread_s[2]);
	cyg_thread_create(6, processing_program, (cyg_addrword_t) 3,
		"Processing Thread", (void *) stack[3], 4096,
		&processing_thread, &thread_s[3]);

	cyg_thread_resume(interface_thread);
	cyg_thread_resume(sender_thread);
	cyg_thread_resume(receiver_thread);
	cyg_thread_resume(processing_thread);
}

/* this is the interface thread */
void interface_program(cyg_addrword_t data)
{
	cmd_ini(0, NULL);
	monitor();
}

/* this is the thread that handles sending data*/
void sender_program(cyg_addrword_t data)
{
	unsigned char *bufw;
	int n = 0;
	int i = 0;
	while (exitControl == 0) {
		bufw = (unsigned char *)cyg_mbox_get( mbxSh ); // wait for message		
		for(i = 0; bufw[i]!=EOM; i++)
		{
			n=i;
		}
		n=i+1;
		err = cyg_io_write(serH, bufw, &n);
		cyg_mutex_lock(&cliblock);
		printf("io_write err=%x, n=%d\n", err, n);
		cyg_mutex_unlock(&cliblock);
	}
}

/* this is the thread that receives data*/
void receiver_program(cyg_addrword_t data)
{
	unsigned int n, i;
	unsigned char bufr[50], buffer[10];
	int registo[5];
	n=1;
	int nregs = 0, iregs = 0, regs_saved = 0;
	i=0;
	int nmfl_received = 0, trgc_received = 0, trgi_received = 0;
	while(exitControl == 0)
	{
		err = cyg_io_read(serH, buffer, &n);
		cyg_mutex_lock(&cliblock);
		printf("io_read err=%x, n=%d\n", err, n);
		cyg_mutex_lock(&cliblock);
		
		if(buffer[0] == EOM)
		{
			cyg_mbox_put( mbxRh, bufr );
			//clean variables
			if(nmfl_received == 1)
				nmfl_received = 0;
			if(trgc_received == 1)
				trgc_received = 0;
			if(trgi_received == 1)
				trgi_received = 0;
			regs_saved = 0;
			i=0;			
		}else if(buffer[0] == NMFL){
			//se for o comando de memoria meia cheia, ativa o controlo para começar a ler registos periodicamente
			registerRequest = 1;
			nmfl_received = 1;
		}else if(buffer[0] == TRGC || trgc_received == 1){
			//se for transferencia de resgistos, nao vai guardar no bufr, mas na variavel global
			if(trgc_received == 0){
				trgc_received = 1;
				bufr[0] = buffer[0];
				bufr[1] = CMD_OK;
				i=13; //i a 13 para nao ler registos, mas guardar o valor de n
			}else{
				if(i < 5){
					registo[i]=buffer[0];
					i++;
				}
				if(i == 13){ // guarda o n e coloca o i a 0 para poder começar o if anterior
					nregs = buffer[0];
					i = 0;
				}
				if(i == 5){//a full register has been obtained, save and reset i
					regs_saved+=saveRegister(registo);
					bufr[2]=regs_saved;
					i=0;
				}
			}
		}else if(buffer[0] == TRGI || trgi_received == 1){
			if(trgc_received == 0){
				trgi_received = 1;
				bufr[0] = buffer[0];
			}else{
				if(i < 5){
					registo[i]=buffer[0];		
					i++;
				}
				if(i == 12){
					iregs = buffer[0];
					i = 0;
				}
				if(i == 13){ // guarda o n e coloca o i a 12 para ler o i (posiçao que acabou de ler)
					nregs = buffer[0];
					i = 12;
				}
				if(i == 5){//a full register has been obtained, save and reset i
					regs_saved+=saveRegister(registo);
					bufr[2]=regs_saved;
					i=0;
				}
			}
		}else{
			if(buffer[0] != SOM && nmfl_received == 0)
			{
				//add register if to store in memory rather than in bufr
				bufr[i] = buffer[0];
				i++;
			}
		}
	}
}

/* this is the processing thread */
void processing_program(cyg_addrword_t data)
{
	unsigned char bufw[6];
	unsigned char *bufr;
	int results[6];
	int i = 0;
	time_t seconds = 0, time_ref;
	time_ref = time(NULL);
	
	cloc init_time=malloc(sizeof(cloc));
	cloc end_time=malloc(sizeof(cloc));
	
	while(exitControl == 0){
		seconds = time(NULL)-time_ref;
		if(registerRequest == 1 && seconds >= p ) //add time
		{	
			//request registers
			bufw[0]=SOM;
			bufw[1]=TRGC;
			bufw[2]=10;
			bufw[3]=EOM;
			
			cyg_mbox_put( mbxSh, bufw );
			//get mail box
			bufr = (unsigned char *)cyg_mbox_get(mbxRh);
			if(bufr[1]==CMD_OK)
			{
				checkThresholds(bufr[2]);
				//update time
				time_ref = time(NULL);
			}
		}
		
		if(makeCalculations == 1)
		{
			//get from mail box IP
			bufr = (unsigned char *)cyg_mbox_get(mbxIPh);
			//make calcs 
			init_time->hours = bufr[0];
			init_time->minutes = bufr[1];
			init_time->seconds = bufr[2];
			end_time->hours = bufr[3];
			end_time->minutes = bufr[4];
			end_time->seconds = bufr[5];
			
			calc(init_time,end_time,results);
			//put calcs in mail box PI
			for(i=0; i<6; i++)
				bufw[i]=results[i];
			
			cyg_mbox_put(mbxPIh, bufw);
			
			makeCalculations = 0;
		}
	}
}
