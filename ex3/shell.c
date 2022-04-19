#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define	MAX_SIZE_CMD	256
#define PATH_MAX 1024
#define	MAX_SIZE_ARG	16

char cwd[PATH_MAX];
char cmd[MAX_SIZE_CMD];		
char *argv[MAX_SIZE_ARG];			
pid_t pid;										
char i;	
FILE *fp;
char retVal[21];



void takeInput(){
    
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
    } 

    printf("%s: ", cwd);
	fgets(cmd, MAX_SIZE_CMD, stdin);

	if ((strlen(cmd) > 0) && (cmd[strlen (cmd) - 1] == '\n'))
        	cmd[strlen (cmd) - 1] = '\0';
}

void parseInput(){

	char *token;
	i = 0;
	token = strtok(cmd, " ");
	while(token != NULL){
		argv[i] = token;
		i++;
		token = strtok(NULL, " ");
	}
}

void executeCD(char *args[]){
    if (args[1] == NULL) {
		chdir(getenv("HOME")); 
    }
	else{ 
		if (chdir(args[1]) == -1) {
			printf(" %s: no such directory\n", args[1]);
		}
	}
}

void iocheck(){
	// i = 0;
	// while(argv[i]!=NULL){
	// 	//sjekker om input redir
	// 	if(strcmp(argv[i], ">")){
	// 		ioredir(argv, 0, argv[i+1], argv[i-1]);
	// 	}
	// 	else if(strcmp(argv[i], "<")){
	// 		ioredir(argv, 1, argv[i-1], argv[i+1]);

	// 	}
	// 	i++;
	// // }

	

}

void ioredir(char * argv[], int type ,  char* funksjon, char* outputfil) {
	int status;
	char path[PATH_MAX];

	fp = popen(funksjon, "r");
	if (fp == NULL)
		/* Handle error */;


	while (fgets(path, PATH_MAX, fp) != NULL){
	//	printf("%s", path);
	//	int num;
		FILE *fptr;

		// use appropriate location if you are using MacOS or Linux
		fptr = fopen(outputfil,"a");

		if(fptr == NULL)
		{
			printf("Error!");   
			exit(1);             
		}

		fprintf(fptr,"%s",path);
		fclose(fptr);
	}

	status = pclose(fp);
	if (status == -1) {
		/* Error reported by pclose() */
		//...
	} else {
		/* Use macros described under wait() to inspect `status' in order
		to determine success/failure of command executed by popen() */
		//...
	}
}
void ampersand(){
	
}

char * concatStr(char * argv[], int x){
	strcpy(retVal, argv[0]);
	for (i = 1; i < x; ++i)
  	{
    	strcat(retVal, argv[i]);
 	}
	return retVal;
}

void startShell(){
	while(!feof(stdin)){
		takeInput();
        parseInput();
    
        if(!strcmp("cd", cmd)) {
            executeCD(argv);
            continue;
        }
		int io = 0;
		i = 0;
		while(argv[i]!=NULL){
				if(strcmp(argv[i], "> ")){
				//	char retVal[21];
				//	strcpy(retVal, concatStr(argv, i));
					ioredir(argv, 0, /*retVal*/ argv[i-1], argv[i+1]);
				//	ioredir(argv, 1, "ls", "t.txt");
				io = 1;

				}
			i++;
		}

		if(io = 1){
		pid = fork();
		if(pid == 0){
            int x = execvp(argv[0], argv);
			printf("fungerer ikke");
            exit(0);
		}}
      
        int status;
     
            waitpid(pid, &status, 0);

            if ( WIFEXITED(status) ) {
                int exit_status = WEXITSTATUS(status);       
                printf("\nExit status [%s] = %d\n", cmd, exit_status);
            }
	}
}

int main(){
	
	startShell();
	// ioredir(argv, 1, "ls", "test.txt");
	return 0;
}