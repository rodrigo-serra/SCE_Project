	#include <cyg/kernel/kapi.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

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

/* and now a mutex to protect calls to the C library */
cyg_mutex_t cliblock;

/* we install our own startup routine which sets up threads */
void cyg_user_start(void)
{
	printf("Start Server\n");

	cyg_mutex_init(&cliblock);

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
  int message = (int) data;
  int delay;

  printf("Beginning execution; thread data is %d\n", message);

  cyg_thread_delay(200);

  for (;;) {
    delay = 200 + (rand() % 50);

    /* note: printf() must be protected by a
       call to cyg_mutex_lock() */
    cyg_mutex_lock(&cliblock); {
      printf("Thread %d: and now a delay of %d clock ticks\n",
	     message, delay);
    }
    cyg_mutex_unlock(&cliblock);
    cyg_thread_delay(delay);
  }
}

/* this is the thread that handles sending data*/
void sender_program(cyg_addrword_t data)
{
	int message = (int) data;
  int delay;

  printf("Beginning execution; thread data is %d\n", message);

  cyg_thread_delay(100);

  for (;;) {
    delay = 100 + (rand() % 50);

    /* note: printf() must be protected by a
       call to cyg_mutex_lock() */
    cyg_mutex_lock(&cliblock); {
      printf("Thread %d: and now a delay of %d clock ticks\n",
	     message, delay);
    }
    cyg_mutex_unlock(&cliblock);
    cyg_thread_delay(delay);
  }
}

/* this is the thread that receives data*/
void receiver_program(cyg_addrword_t data)
{
	int message = (int) data;
  int delay;

  printf("Beginning execution; thread data is %d\n", message);

  cyg_thread_delay(300);

  for (;;) {
    delay = 300 + (rand() % 50);

    /* note: printf() must be protected by a
       call to cyg_mutex_lock() */
    cyg_mutex_lock(&cliblock); {
      printf("Thread %d: and now a delay of %d clock ticks\n",
	     message, delay);
    }
    cyg_mutex_unlock(&cliblock);
    cyg_thread_delay(delay);
  }
}

/* this is the processing thread */
void processing_program(cyg_addrword_t data)
{
	int message = (int) data;
  int delay;

  printf("Beginning execution; thread data is %d\n", message);

  cyg_thread_delay(400);

  for (;;) {
    delay = 400 + (rand() % 50);

    /* note: printf() must be protected by a
       call to cyg_mutex_lock() */
    cyg_mutex_lock(&cliblock); {
      printf("Thread %d: and now a delay of %d clock ticks\n",
	     message, delay);
    }
    cyg_mutex_unlock(&cliblock);
    cyg_thread_delay(delay);
  }
}
