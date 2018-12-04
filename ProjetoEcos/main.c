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
extern void checkThresholds(void);

/* we install our own startup routine which sets up threads */
void cyg_user_start(void)
{
	printf("Start Server\n");

	cyg_mutex_init(&cliblock);
	
	cyg_mbox_create( &mbxSh, &mbxS);
	cyg_mbox_create( &mbxRh, &mbxR);
	cyg_mbox_create( &mbxIPh, &mbxIP);
	cyg_mbox_create( &mbxPIh, &mbxPI);

	cyg_thread_create(4, interface_program, (cyg_addrword_t) 0,
		"Interface Thread", (void *) stack[0], 4096,
		&interface_thread, &thread_s[0]);
	cyg_thread_create(5, sender_program, (cyg_addrword_t) 1,
		"Sender Thread", (void *) stack[1], 4096,
		&sender_thread, &thread_s[1]);
	cyg_thread_create(6, receiver_program, (cyg_addrword_t) 2,
		"Receiver Thread", (void *) stack[2], 4096,
		&receiver_thread, &thread_s[2]);
	cyg_thread_create(3, processing_program, (cyg_addrword_t) 3,
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
	n=1;
	i=0;
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
			i=0;
		}else{
			if(buffer[0] != SOM)
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
	unsigned char bufw[5];
	unsigned char *bufr;
	time_t seconds = 0, time_ref;
	time_ref = time(NULL);
	
	while(exitControl == 0){
		seconds = time(NULL)-time_ref;
		if(registerRequest == 1 && seconds >= p ) //add time
		{
			//request thresholds
			bufw[0]=SOM;
			bufw[1]=RALA;
			bufw[2]=EOM;
	
			cyg_mbox_put( mbxSh, bufw );
			//mail box get
			bufr = (unsigned char *)cyg_mbox_get(mbxRh);
			//save thresholds
			TempThreshold = bufr[1];
			LumThreshold = bufr[2];
			
			//request registers
			bufw[0]=SOM;
			bufw[1]=TRI;
			bufw[2]=20;
			bufw[3]=0;
			bufw[2]=EOM;
			
			cyg_mbox_put( mbxSh, bufw );
			//get mail box
			bufr = (unsigned char *)cyg_mbox_get(mbxRh);
			if(bufr[1]==CMD_OK)
			{
				checkThresholds();
				registerRequest = 0;
				//update time
				time_ref = time(NULL);
			}
		}
		
		if(makeCalculations == 1)
		{
			//get from mail box IP
			bufr = (unsigned char *)cyg_mbox_get(mbxIPh);
			//make calcs 
			
			//put calcs in mail box PI
			
			makeCalculations = 0;
		}
	}
}
