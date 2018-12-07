/***************************************************************************
| File: comando.c  -  Concretizacao de comandos (exemplo)
|
| Autor: Carlos Almeida (IST)
| Data:  Maio 2008
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cyg/io/io.h>
#include "globalvariables.h"

//Cyg_ErrNo err;
//cyg_io_handle_t serH;


/*-------------------------------------------------------------------------+
| Function: cmd_sair - termina a aplicacao
+--------------------------------------------------------------------------*/ 
void cmd_sair (int argc, char **argv)
{
	exitControl = 1;
	exit(0);
}

/*-------------------------------------------------------------------------+
| Function: cmd_ems - enviar mensagem (string)
+--------------------------------------------------------------------------*/ 
void cmd_ems (int argc, char **argv)
{
  unsigned int n;

  if (argc > 1) {
    n = strlen(argv[1]) + 1;
    err = cyg_io_write(serH, argv[1], &n);
    printf("io_write err=%x, n=%d str=%s\n", err, n, argv[1]);
  }
  else {
    n = 10;
    err = cyg_io_write(serH, "123456789", &n);
    printf("io_write err=%x, n=%d str=%s\n", err, n, "123456789");
  }
}

/*-------------------------------------------------------------------------+
| Function: cmd_emh - enviar mensagem (hexadecimal)
+--------------------------------------------------------------------------*/ 
void cmd_emh (int argc, char **argv)
{
  unsigned int n, i;
  unsigned char bufw[50];

  if ((n=argc) > 1) {
    n--;
    if (n > 50) n = 50;
    for (i=0; i<n; i++)
      //      sscanf(argv[i+1], "%hhx", &bufw[i]);
      {unsigned int x; sscanf(argv[i+1], "%x", &x); bufw[i]=(unsigned char)x;}
    err = cyg_io_write(serH, bufw, &n);
    printf("io_write err=%x, n=%d\n", err, n);
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  }
  else {
    printf("nenhuma mensagem!!!\n");
  }
}

/*-------------------------------------------------------------------------+
| Function: cmd_rms - receber mensagem (string)
+--------------------------------------------------------------------------*/ 
void cmd_rms (int argc, char **argv)
{
  unsigned int n;
  char bufr[50];

  if (argc > 1) n = atoi(argv[1]);
  if (n > 50) n = 50;
  err = cyg_io_read(serH, bufr, &n);
  printf("io_read err=%x, n=%d buf=%s\n", err, n, bufr);
}

/*-------------------------------------------------------------------------+
| Function: cmd_rmh - receber mensagem (hexadecimal)
+--------------------------------------------------------------------------*/ 
void cmd_rmh (int argc, char **argv)
{
  unsigned int n, i;
  unsigned char bufr[50];

  if (argc > 1) n = atoi(argv[1]);
  if (n > 50) n = 50;
  err = cyg_io_read(serH, bufr, &n);
  printf("io_read err=%x, n=%d\n", err, n);
  for (i=0; i<n; i++)
    printf("buf[%d]=%x\n", i, bufr[i]);
}


/*-------------------------------------------------------------------------+
| Function: cmd_ini - inicializar dispositivo
+--------------------------------------------------------------------------*/ 
void cmd_ini(int argc, char **argv)
{
  printf("io_lookup\n");
  if ((argc > 1) && (argv[1][0] = '1'))
    err = cyg_io_lookup("/dev/ser1", &serH);
  else err = cyg_io_lookup("/dev/ser0", &serH);
  printf("lookup err=%x\n", err);
}

/*-------------------------------------------------------------------------+
| Function: cmd_rc - read clock
+--------------------------------------------------------------------------*/
void cmd_rc (int argc, char** argv)
{
	unsigned int n=3, i;
	unsigned char bufw[3];
	unsigned char *bufr;
 
	bufw[0]=SOM;
	bufw[1]=RCLK;
	bufw[2]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
	
    cyg_mutex_lock(&cliblock);
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
	cyg_mutex_unlock(&cliblock);
	
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter horas do buffer e fazer display
	cyg_mutex_lock(&cliblock);
	printf("%d:%d:%d",bufr[1],bufr[2],bufr[3]);
	cyg_mutex_unlock(&cliblock);
}

/*-------------------------------------------------------------------------+
| Function: cmd_sc - set clock
+--------------------------------------------------------------------------*/
void cmd_sc (int argc, char** argv)
{
	unsigned int n= 5, i;
	unsigned char bufw[6];
	unsigned char *bufr;
	
	do{
		bufw[0]=SOM;
		bufw[1]=SCLK;
		if ((n=argc) == 4) {
			unsigned int x;
			sscanf(argv[1], "%x", &x); 
			bufw[2]=(unsigned char)x;
			sscanf(argv[2], "%x", &x); 
			bufw[3]=(unsigned char)x;
			sscanf(argv[3], "%x", &x); 
			bufw[4]=(unsigned char)x;
		}else{
			cyg_mutex_lock(&cliblock);
			printf("Wrong arguments for sc");
			cyg_mutex_unlock(&cliblock);
			return;
		}
		bufw[5]=EOM;
		
		cyg_mbox_put( mbxSh, bufw );
		
		cyg_mutex_lock(&cliblock);
		for (i=0; i<n; i++)
		  printf("buf[%d]=%x\n", i, bufw[i]);
		cyg_mutex_unlock(&cliblock);
		
		bufr = (unsigned char *)cyg_mbox_get(mbxRh);
		//obter error message
	}while(bufr[1]!=CMD_OK);
  
}

/*-------------------------------------------------------------------------+
| Function: cmd_rtl - read temperature and luminosity
+--------------------------------------------------------------------------*/
void cmd_rtl (int argc, char** argv)
{
	unsigned int n=3, i;
	unsigned char bufw[3];
	unsigned char *bufr;
 
	bufw[0]=SOM;
	bufw[1]=RTL;
	bufw[2]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
    
	cyg_mutex_lock(&cliblock);
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
	cyg_mutex_unlock(&cliblock);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter horas do buffer e fazer display
	cyg_mutex_lock(&cliblock);
	printf("temperature: %d | luminosity : %d",bufr[1],bufr[2]);
	cyg_mutex_unlock(&cliblock);
}

/*-------------------------------------------------------------------------+
| Function: cmd_rp - read parameters (PMON, TALA)
+--------------------------------------------------------------------------*/
void cmd_rp (int argc, char** argv)
{
	unsigned int n=3, i;
	unsigned char bufw[3];
	unsigned char *bufr;
 
	bufw[0]=SOM;
	bufw[1]=RPAR;
	bufw[2]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
    
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter horas do buffer e fazer display
	printf("PMON: %d | TALA : %d",bufr[1],bufr[2]);
}

/*-------------------------------------------------------------------------+
| Function: cmd_mmp - modify monitoring period (seconds - 0 deactivate)
+--------------------------------------------------------------------------*/
void cmd_mmp (int argc, char** argv)
{
	unsigned int n = 4, i;
	unsigned char bufw[4];
	unsigned char *bufr;
	
	bufw[0]=SOM;
	bufw[1]=MMP;
	if ((n=argc) == 2) {
		unsigned int x;
		sscanf(argv[1], "%x", &x); 
		bufw[2]=(unsigned char)x;
	}else{
		printf("Wrong arguments for MMP");
		return;
	}
	bufw[3]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
    
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter error message
	printf("%d",bufr[1]);
}

/*-------------------------------------------------------------------------+
| Function: cmd_mta - modify time alarm (seconds)
+--------------------------------------------------------------------------*/
void cmd_mta (int argc, char** argv)
{
	unsigned int n = 4, i;
	unsigned char bufw[4];
	unsigned char *bufr;
	
	bufw[0]=SOM;
	bufw[1]=MTA;
	if ((n=argc) == 2) {
		unsigned int x;
		sscanf(argv[1], "%x", &x); 
		bufw[2]=(unsigned char)x;
	}else{
		printf("Wrong arguments for mta");
		return;
	}
	bufw[3]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
    
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter error message
	printf("%d",bufr[1]);
}

/*-------------------------------------------------------------------------+
| Function: cmd_ra - read alarms (temperature, luminosity, active/inactive-1/0)
+--------------------------------------------------------------------------*/
void cmd_ra (int argc, char** argv)
{
	unsigned int n = 3, i;
	unsigned char bufw[3];
	unsigned char *bufr;
	
	bufw[0]=SOM;
	bufw[1]=RALA;
	bufw[2]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
    
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter threasholds da temperature e luminosity, além do ALAF
	printf("Temperature Threshold: %d/nLuminosity Threshold: %d/nALAF: %d",bufr[1],bufr[2],bufr[3]);
}

/*-------------------------------------------------------------------------+
| Function: cmd_dtl - define alarm thresholds for temperature and luminosity
+--------------------------------------------------------------------------*/
void cmd_dtl (int argc, char** argv)
{
	unsigned int n = 5, i;
	unsigned char bufw[5];
	unsigned char *bufr;
	
	bufw[0]=SOM;
	bufw[1]=DATL;
	if ((n=argc) == 3) {
		unsigned int x;
		sscanf(argv[1], "%x", &x); 
		bufw[2]=(unsigned char)x;
		sscanf(argv[2], "%x", &x); 
		bufw[3]=(unsigned char)x;
	}else{
		printf("Wrong arguments for dtl");
		return;
	}
	bufw[4]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
    
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter error message
	printf("%d",bufr[1]);
}

/*-------------------------------------------------------------------------+
| Function: cmd_aa - activate/deactivate alarms (1/0)
+--------------------------------------------------------------------------*/
void cmd_aa (int argc, char** argv)
{
	unsigned int n = 3, i;
	unsigned char bufw[4];
	unsigned char *bufr;
	
	bufw[0]=SOM;
	bufw[1]=AALA;
	if ((n=argc) == 3) {
		unsigned int x;
		sscanf(argv[1], "%x", &x); 
		bufw[2]=(unsigned char)x;
	}else{
		printf("Wrong arguments for dtl");
		return;
	}
	bufw[3]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
    
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter error message
	printf("%d",bufr[1]);
}

/*-------------------------------------------------------------------------+
| Function: cmd_rmm - read maximums and minimums registers
+--------------------------------------------------------------------------*/
void cmd_rmm (int argc, char** argv)
{
	unsigned int n= 3, i;
	unsigned char bufw[3];
	unsigned char *bufr;
	
	bufw[0]=SOM;
	bufw[1]=RMM;
	bufw[2]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
    
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter os registos para maximo e minimo
	printf("%d",bufr[1]);
	/*************************************
	*
	*		TO DOOOOOOOOOOOOOOO
	*
	*
	**************************************/
}

/*-------------------------------------------------------------------------+
| Function: cmd_cmm - clear maximums and minimums registers
+--------------------------------------------------------------------------*/
void cmd_cmm (int argc, char** argv)
{
	unsigned int n=3, i;
	unsigned char bufw[3];
	unsigned char *bufr;
	
	bufw[0]=SOM;
	bufw[1]=CMM;
	bufw[2]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
    
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter error message
	printf("%d",bufr[1]);
}

/*-------------------------------------------------------------------------+
| Function: cmd_ir - information about registers (NREG, nr, iread, iwrite)
+--------------------------------------------------------------------------*/
void cmd_ir (int argc, char** argv)
{
	unsigned int n=3, i;
	unsigned char bufw[3];
	unsigned char *bufr;
	
	bufw[0]=SOM;
	bufw[1]=IREG;
	bufw[2]=EOM;
	
	cyg_mbox_put( mbxSh, bufw );
    
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxRh);
	//obter valores
	printf("Num de regs total: %d/nRegs usados: %d/nÚltimo registo lido: %d/nÚltimo registo escrito: %d",
		bufr[1],bufr[2],bufr[3],bufr[4]);
}

/*-------------------------------------------------------------------------+
| Function: cmd_trc - transfer n registers from current iread position
+--------------------------------------------------------------------------*/
void cmd_trc (int argc, char** argv)
{
	
}

/*-------------------------------------------------------------------------+
| Function: cmd_tri - transfer n registers from index i (0 - oldest)"},
+--------------------------------------------------------------------------*/
void cmd_tri (int argc, char** argv)
{
	
}

void cmd_irl (int argc, char** argv)
{
	
}

void cmd_lr (int argc, char** argv)
{
	
}

void cmd_dr (int argc, char** argv)
{
	
}

void cmd_cpt (int argc, char** argv)
{
	
}

void cmd_mpt (int argc, char** argv)
{
	
}

void cmd_dttl (int argc, char** argv)
{
	
}

void cmd_pr (int argc, char** argv)
{
	
}
