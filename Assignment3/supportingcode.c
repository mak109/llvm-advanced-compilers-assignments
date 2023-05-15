#include<stdio.h>
unsigned int GA;
FILE *mfp; 
int firsttimeOpen=0;
void printL(){
//void printLL(unsigned int paramA){
	if (firsttimeOpen==0) {
		mfp=fopen("mytrace.txt","w");
		firsttimeOpen=1;
	}
	fprintf(mfp,"\n%u",GA);
	//printf(mfp,“\n%u”,paramA);
}
void printS(){
//void printSS(unsigned int paramA){
	if (firsttimeOpen==0) {
		mfp=fopen("mytrace.txt","w");
		firsttimeOpen=1;
	}
	fprintf(mfp,"\n%u",GA);
	//fprintf(mfp,“\n%u”,paramA);
}
/*
int main(){
	return 0;
}
*/
