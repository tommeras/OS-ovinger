#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include "main.h"
#include <sys/wait.h>	







typedef struct Alarm {
	time_t time;
	int pid;
} alarmKlokke;

alarmKlokke alarmer[10];
alarmKlokke *addr[10];
int status;



time_t epoch;
time_t na;


void setAlarm(){
	char dato[11], tidsPunkt[9];
	printf("What date is the alarm for?\nPLease use YYYY-MM-DD format\n");
	scanf("%10s", dato);
	printf("What time should the alarm be?\nUse HH-MM-SS format\n");
	scanf("%8s", tidsPunkt);
	struct tm tid; 
	char oppDeling[6][5];
	updateAlarms();



   char * datoSplitt = strtok(dato, "-");
   int i = 0;
   // loop through the string to extract all other tokens
   while( datoSplitt != NULL ) {
	  strcpy(oppDeling[i], datoSplitt);
      datoSplitt = strtok(NULL, "-");
	  i++;
   }
   char * tidSplitt = strtok(tidsPunkt, "-");
   // loop through the string to extract all other tokens
   while( tidSplitt != NULL ) {
	  strcpy(oppDeling[i], tidSplitt);
      tidSplitt = strtok(NULL, "-");
	  i++;
   }


   tid.tm_year = (atoi(oppDeling[0])-1900);
   tid.tm_mon = (atoi(oppDeling[1])-1);
   tid.tm_mday = atoi(oppDeling[2]);
   tid.tm_hour = (atoi(oppDeling[3])+1);
   tid.tm_min = (atoi(oppDeling[4]));
   tid.tm_sec = (atoi(oppDeling[5]));



   time_t alarmTid;
   alarmTid = mktime(&tid);

   time_t naa;
   time(&naa);


   long venteTid;
   venteTid = alarmTid-naa;


    printf("The alarm is set for: %s\n", asctime(&tid));





		int pid = fork();


	 	if(pid==0){
			
	 		printf("Venter på ringing");
			sleep(venteTid);
			printf("\nRING\nRING\nRING\n");

			int lydPid = fork();
			if (lydPid==0)
			{
				execlp("mpg123", "mpg123", "alarm_evolved.mp3", NULL);
			}
			else{
				wait(NULL);
			}
			printf("Please enter 's' (schedule), 'l' (list), 'c' (cancel), 'x' (exit)\n");	
			exit(0);
			
		}
		else{
			alarmKlokke ny;
			ny.time = alarmTid;
			ny.pid = pid;

			for (size_t i = 0; i < 10; i++)
			{
				double forskjell = difftime (alarmer[i].time, epoch);
				int dobbelkon = (int) forskjell;
				if(dobbelkon == 0){
					alarmer[i]=ny;
					time_t vis;
					vis = alarmer[i].time;
					struct tm *visTid;
					visTid = localtime(&vis);


					break;

				}
			}
		}
};

void updateAlarms(){
	na = time(NULL);
	for (size_t i = 0; i < 10; i++){
		double forskjell = difftime (alarmer[i].time, na);
		int dobbelkon = (int) forskjell;
			if (dobbelkon<=0)
			{
				waitpid(alarmer[i].pid, &status ,WNOHANG);
				alarmer[i].time = 0;
				alarmer[i].pid = 0;
			}
	}
}



void listAlarm(){
	updateAlarms();
	int j = 1;
	for (size_t i = 0; i < 10; i++){
		double forskjell = difftime (alarmer[i].time, epoch);
		int dobbelkon = (int) forskjell;
			if (dobbelkon!=0)
			{
				time_t vis;
				vis = alarmer[i].time;
				struct tm *visTid;
				visTid = localtime(&vis);

				printf("Alarm #%i at %s", (j), asctime(visTid));
				j++;
			}
			

	}

}

void cancelAlarm(){
	
	printf("What alarm do you want to cancel?");
	int q;
	scanf("%i", &q);
	updateAlarms();

	int j = 1;
	for (size_t i = 0; i < 10; i++){
		double forskjell = difftime (alarmer[i].time, epoch);
		int dobbelkon = (int) forskjell;
			if (dobbelkon!=0)
			{
				if(j==q){
					time_t vis;
					vis = alarmer[i].time;
					struct tm *visTid;
					visTid = localtime(&vis);
					int drep;
					drep = alarmer[i].pid;
					int suk;
					suk = kill(drep, 1);
					if(suk == 0){
						alarmer[i].time = 0;
						printf("Alarm #%i at %s is now cancelled.\n", (j), asctime(visTid));
					}else{
						printf("Failed to remove alarm.\n");
					}
					break;
				}
				else{
					j++;
				}
			}
			

	}
	

	

	

}

void exitProgram(){

	printf("\nGod Dag videre:)\n");
	exit(0);

}



int main() {	
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

    printf("Welcome to the alarm clock! It is currently %s", asctime(timeinfo));


	for (size_t i = 0; i < 10; i++)
	{
		addr[i] = &alarmer[i];
	}
	while(1){

	char inputValue;
	printf("Please enter 's' (schedule), 'l' (list), 'c' (cancel), 'x' (exit)\n");
	scanf(" %c", &inputValue);
	printf("\nThe picked character is: %c \n", inputValue);
	updateAlarms();



	switch (inputValue)
	{
	case 's':
		setAlarm();
		break;
	case 'l':
		listAlarm();
		break;		
	case 'c':
		cancelAlarm();
		break;
	case 'x':
		exitProgram();
		break;			
	default:
		break;
	}
	}


	return 0;
}


