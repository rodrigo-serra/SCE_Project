#include <stdio.h>
#include <stdlib.h>
#include "globalvariables.h"


void checker(int * minL,int * maxL, int * minT, int * maxT, int i);

int date_calculator(cloc start, cloc end,cloc to_check);

/*Funçao responsavel pelo calculo da media, valor maximo e minimo*/
void calc(cloc start_time,cloc end_time,int *results){
		
	int maxL=0,minL=5,maxT=0,minT=51,mediaL=0,mediaT=0,values_to_mean=0;
	cloc curr_sample_time=malloc(sizeof(cloc));
	int i = 0, belongsToTime = 0;
	
	//para todos os registos
	for (i =0;i<nr;i++){
		curr_sample_time->hours=registers[i][0];
		curr_sample_time->minutes=registers[i][1];
		curr_sample_time->seconds=registers[i][2];

		//se esta variavel tiver igual a CMD_ERROR significa que
		//nenhum argumento foi utilizado e o cálculo é efetuado sobre
		//todos os registos. Caso contrario verifica se se encontra
		//dentro do periodo de tempo definido
		if(start_time->hours != CMD_ERROR)
			belongsToTime = date_calculator(start_time,end_time,curr_sample_time);
		else
			belongsToTime = 1;

		//caso o registo seja incluido nas contas
		if(belongsToTime != 0){
			values_to_mean++;
			mediaT+=registers[i][3];
			mediaL+=registers[i][4];
			checker(&minL,&maxL,&minT,&maxT,i);
		}
	}
	if(values_to_mean != 0){
		mediaL/=values_to_mean;
		mediaT/=values_to_mean;
	}
	//resultados guardados no vetor que será o resultado
	results[0]=maxT;
	results[1]=minT;
	results[2]=mediaT;
	results[3]=maxL;
	results[4]=minL;
	results[5]=mediaL;
	
	free(curr_sample_time);

}

/*Função que determina caso um registo se encontra ou nao no intervalo pretendido*/
int date_calculator(cloc start, cloc end,cloc to_check){
		
	if(to_check->hours < start->hours || to_check->hours > end->hours)
		return 0;
	
	if(to_check->hours != start-> hours && to_check->hours != end->hours)		
		return 1;
	
	if(to_check->hours == start->hours && to_check->hours == end->hours){

		if(to_check->minutes < start->minutes || to_check->minutes > end->minutes)
			return 0;
		if( to_check->minutes > start->minutes && to_check->minutes < end->minutes)			
			return 2;
		else{
			if(to_check->seconds < start->seconds || to_check->seconds > end->seconds)
				return 0;
			else
				return 3;
		}
	}
	if(to_check->hours == start->hours){
		if(to_check->minutes < start->minutes)
			return 0;
		if( to_check->minutes > start->minutes)
			return 4;
		else{
			if(to_check->seconds < start->seconds)
				return 0;
			else
				return 5;
		}
	}
	if(to_check->hours == end->hours){
		if(to_check->minutes > end->minutes)
			return 0;
		
		if( to_check->minutes < end->minutes){
			return 6;		
		}else{
			if(to_check->seconds > end->seconds)
				return 0;
			else
				return 7;
		}
	}
	
	return 0;
}

/*Checks if a certain register i is max or min*/
void checker(int * minL,int * maxL, int * minT, int * maxT, int i){
	if(registers[i][3]>=*maxT)		//temp
		* maxT=registers[i][3];
	
	if(registers[i][3]<=*minT)			//temp
		* minT=registers[i][3];
	
	if(registers[i][4]>=*maxL)			//lum
		* maxL=registers[i][4];
	
	if(registers[i][4]<=*minL)			//lum
		* minL=registers[i][4];
	
}

/*Check for the newly added registers if they go over the threasholds*/
void checkThresholds(int n){
	int i = 0;
	for(i=n-1; i >= 0; i--)
	{
		if(registers[iwrite-i][3] > TempThreshold)
		{
			cyg_mutex_lock(&cliblock);
			printf("%d:%d:%d - Temp excedeu %d. Temperatura = %d\n",registers[iwrite-i][0],registers[iwrite-i][1],registers[iwrite-i][2],TempThreshold,registers[iwrite-i][3]);
			cyg_mutex_unlock(&cliblock);
		}
		
		if(registers[iwrite-i][4] > LumThreshold)
		{
			cyg_mutex_lock(&cliblock);
			printf("%d:%d:%d - Lum excedeu %d. Luminosidade = %d\n",registers[iwrite-i][0],registers[iwrite-i][1],registers[iwrite-i][2],LumThreshold,registers[iwrite-i][4]);
			cyg_mutex_unlock(&cliblock);
		}
	}
	
}

/*function in charge of saving a register in memory*/
int saveRegister(int registo[5]){

	int i = 0;
	if(iwrite == -1){
		for(i = 0; i < 5 ; i++)
			registers[0][i] = registo[i];
		iwrite = 0;
		nr = 1;
		return 1;
	}else if(/*registo[1]>=registers[iwrite][1] && registo[2]>registers[iwrite][2] &&*/ registo[0] >= 0 &&  registo[0] < 24 && registo[1]>= 0 && registo[1] < 60 &&  registo[2] < 60 && registo[2]>= 0){
		if(iwrite < NRBUF){
			iwrite += 1;
		}else{
			iwrite = 0;
		}
		nr++;
		for(i = 0; i < 5 ; i++)
			registers[iwrite][i] = registo[i];
		return 1;
	}
	return 0;
}
