#include <stdio.h>
#include <stdlib.h>
#include "globalvariables.h"

int ** vector;
int nmbr_reg;

void checker(int * minL,int * maxL, int * minT, int * maxT, int vector[6]);

int date_calculator(cloc start, cloc end,cloc to_check);

void calc(cloc start_time,cloc end_time,int *results){
	int maxL=0,minL=0,maxT=0,minT=0,mediaL=0,mediaT=0,values_to_mean=0;
	cloc curr_sample_time=malloc(sizeof(cloc_));
	int i = 0;
	
	for (i =0;i<nmbr_reg;i++){
		if(vector[i][6]!=0){//new entry
			checker(&minL,&maxL,&minT,&maxT,vector[i]);
			vector[i][6]=0;
		}

		curr_sample_time->hours=vector[i][0];
		curr_sample_time->minutes=vector[i][1];
		curr_sample_time->seconds=vector[i][2];

		if(date_calculator(start_time,end_time,curr_sample_time)){
			values_to_mean++;
			mediaL+=vector[i][4];
			mediaT+=vector[i][5];
		}
	}
	mediaL/=values_to_mean;
	mediaT/=values_to_mean;
	results[0]=maxT;
	results[1]=minT;
	results[2]=mediaT;
	results[3]=maxL;
	results[4]=minL;
	results[5]=mediaL;
	
	free(curr_sample_time);

}

int date_calculator(cloc start, cloc end,cloc to_check){
	
	if(to_check->hours < start->hours || to_check->hours > end->hours)
		return 0;
	
	if(to_check->hours != start-> hours && to_check->hours != end->hours)
		return 1;
	
	if(to_check->hours == start->hours){
		if(to_check->minutes < start->minutes)
			return 0;
		if( to_check->minutes > start->minutes)
			return 1;
		else{
			if(to_check->seconds < start->seconds)
				return 0;
			else 
				return 1;
		}
	}
	if(to_check->hours == end->hours){
		if(to_check->minutes > end->minutes)
			return 0;
		
		if( to_check->minutes < end->minutes)
			return 1;
		
		else{
			if(to_check->seconds > end->seconds)
				return 0;
			else 
				return 1;
		}
	}
	
	return 0;
}

void checker(int * minL,int * maxL, int * minT, int * maxT, int vector[6]){
	if(vector[5]>=*maxT)						//temp
		* maxT=vector[5];
	
	if(vector[5]<=*minT)						//temp
		* maxT=vector[5];
	
	if(vector[4]>=*maxL)						//lum
		* maxL=vector[4];
	
	if(vector[4]<=*maxL)						//lum
		* minL=vector[4];
	
}

void checkThresholds(int n){
	int i = 0;
	for(i=n-1; i >= 0; i--)
	{
		if(registers[iwrite-i][3] > TempThreshold)
		{
			cyg_mutex_lock(&cliblock);
			printf("%d:%d:%d excedeu %d. Temperatura = %d",registers[iwrite-i][0],registers[iwrite-i][1],registers[iwrite-i][2],TempThreshold,registers[iwrite-i][3]);
			cyg_mutex_unlock(&cliblock);
		}
		
		if(registers[iwrite-i][4] > LumThreshold)
		{
			cyg_mutex_lock(&cliblock);
			printf("%d:%d:%d excedeu %d. Luminosidade = %d",registers[iwrite-i][0],registers[iwrite-i][1],registers[iwrite-i][2],LumThreshold,registers[iwrite-i][4]);
			cyg_mutex_unlock(&cliblock);
		}
	}
	
}

int saveRegister(int registo[5]){
	//Verificar se a hora do registo é mais atual que a ultima hora guardada
	int i = 0;
	if(iwrite == -1){
		for(i = 0; i < 5 ; i++)
			registers[0][i] = registo[i];
		iwrite = 0;
		return 1;
	}else if(registo[1]>=registers[iwrite][1] && registo[2]>registers[iwrite][2]){
		if(iwrite < NRBUF){
			iwrite += 1;
		}else{
			iwrite = 0;
		}
		
		for(i = 0; i < 5 ; i++)
			registers[iwrite][i] = registo[i];
		return 1;
	}
	return 0;
}
