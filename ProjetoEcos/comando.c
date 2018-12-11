/***************************************************************************
| File: comando.c  -  Concretizacao de comandos (exemplo)
|
| Autor: Carlos Almeida (IST)
| Data:  Maio 2008
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cyg/io/io.h>
#include <string.h>
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

	registers[0][0] = 0;
	registers[0][1] = 1;
	registers[0][2] = 1;
	registers[0][3] = 20;
	registers[0][4] = 1;

	registers[1][0] = 0;
	registers[1][1] = 1;
	registers[1][2] = 10;
	registers[1][3] = 19;
	registers[1][4] = 1;

	registers[2][0] = 0;
	registers[2][1] = 1;
	registers[2][2] = 15;
	registers[2][3] = 21;
	registers[2][4] = 2;

	nr = 3;
	iwrite = 3;
	iread = 0;
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
	
	bufw[0]=SOM;
	bufw[1]=SCLK;
	
	if ((n=argc) == 4) {
		unsigned int x;
		sscanf(argv[1], "%d", &x); 
		bufw[2]=(unsigned char)x;
		sscanf(argv[2], "%d", &x); 
		bufw[3]=(unsigned char)x;
		sscanf(argv[3], "%d", &x); 
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
	if(bufr[1] == CMD_OK){
		cyg_mutex_lock(&cliblock);
		printf("Horas atualizadas com sucesso");
		cyg_mutex_unlock(&cliblock);
	}else{
		cyg_mutex_lock(&cliblock);
		printf("Operação sem sucesso!");
		cyg_mutex_unlock(&cliblock);
	}
  
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
    
	cyg_mutex_lock(&cliblock);
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
	cyg_mutex_unlock(&cliblock);
  
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
		sscanf(argv[1], "%d", &x); 
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
		sscanf(argv[1], "%d", &x); 
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
	printf("Temperature Threshold: %d\nLuminosity Threshold: %d\nALAF: %d",bufr[1],bufr[2],bufr[3]);
}

/*-------------------------------------------------------------------------+
| Function: cmd_dtl - define alarm thresholds for temperature and luminosity
+--------------------------------------------------------------------------*/
void cmd_dtl (int argc, char** argv)
{
	unsigned int n = 5, i,na = 0;
	unsigned char bufw[5];
	unsigned char *bufr;
	
	bufw[0]=SOM;
	bufw[1]=DATL;
	if ((na=argc) == 3) {
		unsigned int x;
		sscanf(argv[1], "%d", &x); 
		bufw[2]=(unsigned char)x;
		sscanf(argv[2], "%d", &x); 
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
	unsigned int n = 4, i, na=0;
	unsigned char bufw[4];
	unsigned char *bufr;
	
	bufw[0]=SOM;
	bufw[1]=AALA;
	if ((na=argc) == 2) {
		unsigned int x;
		sscanf(argv[1], "%d", &x); 
		bufw[2]=(unsigned char)x;
	}else{
		printf("Wrong arguments for aa");
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
	printf("Max Temp: %d:%d:%d - Temp: %d - Lum: %d\n",bufr[1],bufr[2],bufr[3],bufr[4],bufr[5]);
	printf("Max Lum: %d:%d:%d - Temp: %d - Lum: %d\n",bufr[6],bufr[7],bufr[8],bufr[9],bufr[10]);
	printf("Min Temp: %d:%d:%d - Temp: %d - Lum: %d\n",bufr[11],bufr[12],bufr[13],bufr[14],bufr[15]);
	printf("Min Lum: %d:%d:%d - Temp: %d - Lum: %d\n",bufr[16],bufr[17],bufr[18],bufr[19],bufr[20]);
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
	if(bufr[3] == 255)
		bufr[3]=0;
	printf("Num de regs total: %d\nRegs usados: %d\nUltimo registo lido: %d\nUltimo registo escrito: %d",
		bufr[1],bufr[2],bufr[3],bufr[4]);
}

/*-------------------------------------------------------------------------+
| Function: cmd_trc - transfer n registers from current iread position
+--------------------------------------------------------------------------*/
void cmd_trc (int argc, char** argv)
{
	if(argc == 2){
		unsigned int n=4, i;
		unsigned char bufw[4];
		unsigned char *bufr;
		
		bufw[0]=SOM;
		bufw[1]=TRGC;
		bufw[2]= atoi(argv[1]);
		bufw[3]=EOM;
		
		cyg_mbox_put( mbxSh, bufw );
		
		for (i=0; i<n; i++)
		  printf("buf[%d]=%x\n", i, bufw[i]);
	  
		bufr = (unsigned char *)cyg_mbox_get(mbxRh);
		
		if(bufr[1] == CMD_OK){
			
			printf("Succesful transference of regiters");
		
		}else{
		
			printf("Invalid transference of %c registers", argv[1]);
		
		}
	}else{
			
			printf("Invalid arguments for comand cmd_trc");
	
	}
	
}

/*-------------------------------------------------------------------------+
| Function: cmd_tri - transfer n registers from index i (0 - oldest)"},
+--------------------------------------------------------------------------*/
void cmd_tri (int argc, char** argv)
{
	if(argc == 3){
		unsigned int n=5, i;
		unsigned char bufw[5];
		unsigned char *bufr;
		
		bufw[0]=SOM;
		bufw[1]=TRGC;
		bufw[2]= atoi(argv[1]);
		bufw[3]= atoi(argv[2]); 
		bufw[4]=EOM;
		
		cyg_mbox_put( mbxSh, bufw );
		
		for (i=0; i<n; i++)
		  printf("buf[%d]=%x\n", i, bufw[i]);
	  
		bufr = (unsigned char *)cyg_mbox_get(mbxRh);
		
		if(bufr[1] == CMD_OK){
			
			printf("Succesful transference of regiters from index %c", argv[1]);
		
		}else{
		
			printf("Invalid transference of %c registers from index %c", argv[1], argv[2]);
		
		}
	}else{
			
			printf("Invalid arguments for comand cmd_trc");
	
	}
}

/*---------------------------------------------------------------------------------+
| Function: cmd_irl - information about local registers (NRBUF, nr, iread, iwrite)"
+----------------------------------------------------------------------------------*/
void cmd_irl (int argc, char** argv)
{
	printf("Num de regs total: %d\nRegs usados: %d\nUltimo registo lido: %d\nUltimo registo escrito: %d",
		NRBUF,nr,iread,iwrite);
	
}

/*---------------------------------------------------------------------------------+
| Function: cmd_lr - list n registers (local memory) from index i (0 - oldest)
+----------------------------------------------------------------------------------*/
void cmd_lr (int argc, char** argv)
{
	//argc[0] - n registers //argc[1] - index of reading
	
	int n = 0, i = 0;
	
	if(argc == 2 && atoi(argv[1]) <= nr){
		//Read n registers from current iread position
		for(n=0;n < atoi(argv[1]);n++){
			
			printf("Register %d: %d:%d:%d | Temp: %d | Lum: %d\n",iread,registers[iread][0],registers[iread][1],registers[iread][2],registers[iread][3],registers[iread][4]);
			
			iread++;
			
			if(iread == nr)
				iread = 0;
		
		}
	
	}else if(argc == 3 && atoi(argv[1]) <= nr && atoi(argv[2]) < nr){
		//Read n registers from index i position
		iread = atoi(argv[2]);
		
		for(n=0;n < atoi(argv[1]);n++){
			
			printf("Register %d: %d:%d:%d | Temp: %d | Lum: %d\n",iread,registers[iread][0],registers[iread][1],registers[iread][2],registers[iread][3],registers[iread][4]);
			
			iread++;
			
			if(iread == nr)
				iread = 0;
		
		}
	
	}else{
		
		printf("One of the arguments out of range for cmd_rl comand");
	
	}
	
}


/*---------------------------------------------------------------------------------+
| Function: cmd_dr - delete registers (local memory)
+----------------------------------------------------------------------------------*/
void cmd_dr (int argc, char** argv)
{
	int n;
	
	for(n=0;n<nr;n++){
		
		registers[n][0] = 0;
		registers[n][1] = 0;
		registers[n][2] = 0;
		registers[n][3] = 0;
		registers[n][4] = 0;
	}
	iread = 0;
	iwrite = 0;
	nr = 0;
	
}

/*---------------------------------------------------------------------------------+
| Function: cmd_cpt - check period of transference
+----------------------------------------------------------------------------------*/
void cmd_cpt (int argc, char** argv)
{
	printf("Period of transference: %d mins",p);
}

/*---------------------------------------------------------------------------------+
| Function: cmd_mpt - modify period of transference (minutes - 0 deactivate)
+----------------------------------------------------------------------------------*/
void cmd_mpt (int argc, char** argv)
{
	if(atoi(argv[1]) >= 0 && atoi(argv[1]) < 60 && argc == 2){
		
		p = atoi(argv[1]);
		if(p == 0)
			registerRequest = 0;
		else
			registerRequest = 1;
		printf("Value for period of transference successfuly changed");

	}else{
		
		printf("Value for period of transference not accepted");
	
	}
}

/*---------------------------------------------------------------------------------+
| Function: cmd_dttl - define threshold temperature and luminosity for processing
+----------------------------------------------------------------------------------*/
void cmd_dttl (int argc, char** argv)
{
	//argc[0] - temperature //argc[1] - luminosity
	
	if(argc == 3 && atoi(argv[1]) > 0 && atoi(argv[1]) < 50 && atoi(argv[2]) >= 0 && atoi(argv[2]) < 4){
		
		TempThreshold = atoi(argv[1]);
		LumThreshold = atoi(argv[2]);

		cyg_mutex_lock(&cliblock);
		printf("Thresholds updated");
		cyg_mutex_unlock(&cliblock);
		
	}else{
		cyg_mutex_lock(&cliblock);
		printf("Thresholds out of range");
		cyg_mutex_unlock(&cliblock);
	}
	
}

/*---------------------------------------------------------------------------------+
| Function: cmd_pr - process registers (max, min, mean) between instants t1 and t2 (h,m,s)
+----------------------------------------------------------------------------------*/
void cmd_pr (int argc, char** argv)
{
	unsigned int n=6, i;
	unsigned char bufw[6];
	unsigned char *bufr;
	
	makeCalculations = 1;		

	if(argc == 1){
		//all registers
	}else if(argc == 4){
		//de t1 a atual
	}else if(argc == 7){
		//de t1 a t2
		
		
		bufw[0]=atoi(argv[1]);
		bufw[1]=atoi(argv[2]);
		bufw[2]= atoi(argv[3]);
		bufw[3]= atoi(argv[4]);
		bufw[4]=atoi(argv[5]);
		bufw[5]=atoi(argv[6]);
	}else{
		cyg_mutex_lock(&cliblock);
		printf("Arguments invalid");
		cyg_mutex_unlock(&cliblock);
		return;
	}
	cyg_mbox_put( mbxIPh, bufw );
		
	for (i=0; i<n; i++)
	  printf("buf[%d]=%x\n", i, bufw[i]);
  
	bufr = (unsigned char *)cyg_mbox_get(mbxPIh);
	
	if(bufr[1] != CMD_ERROR){
		cyg_mutex_lock(&cliblock);
		printf("Max Temp: %d | Min Temp: %d | Mean Temp %d | Max Lum: %d | Min Lum: %d | Mean Lum %d\n", (int)bufr[0],(int)bufr[1],(int)bufr[2],(int)bufr[3],(int)bufr[4],(int)bufr[5]);
		cyg_mutex_unlock(&cliblock);
	}else{
		cyg_mutex_lock(&cliblock);
		printf("Error in calculations");
		cyg_mutex_unlock(&cliblock);
	}

}
