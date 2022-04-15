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
int flag;											

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

void startShell(){
	while(!feof(stdin)){
		takeInput();
        parseInput();
    
        if(!strcmp("cd", cmd)) {
            executeCD(argv);
            continue;
        }

		pid = fork();
		if(pid == 0){
            execvp(argv[0], argv);
            exit(0);
		}
      
        int status;
     
            waitpid(pid, &status, 0);

            if ( WIFEXITED(status) ) {
                int exit_status = WEXITSTATUS(status);       
                printf("Exit status [%s] = %d\n", cmd, exit_status);
            }
            
        // flag = scanf("%[^\n]%*c", cmd);

	}
}

int main(){
	
	startShell();

	return 0;
}