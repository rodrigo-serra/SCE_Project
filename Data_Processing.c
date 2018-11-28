#include <stdio.h>
#include <malloc.h>

int ** vector;
int nmbr_reg;

 struct cloc_{
	int hours;
	int minutes;
	int seconds;
	};
typedef struct cloc_ * cloc;

void checker(int * minL,int * maxL, int * minT, int * maxT, int vector[6]);
int date_calculator(cloc start, cloc end,cloc to_check);

void calc(cloc start_time,cloc end_time,int results[6]){
	int maxL=0,minL=0,maxT=0,minT=0,mediaL=0,mediaT=0,values_to_mean=0;
	cloc curr_sample_time=malloc(sizeof(cloc));
		for (int i =0;i<nmbr_reg;i++){
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
	
