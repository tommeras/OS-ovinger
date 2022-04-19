#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
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
int background;

/*
	For 3.1
*/
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
		token = strtok(NULL, " \n");
	}
}

/*
	For 3.2
*/
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

/* 

	For 3.3

*/

void ioredir(char * argv[], int type ,  char* funksjon, char* outputfil) {
	int status;
	char path[PATH_MAX];

	fp = popen(funksjon, "r");
	if (fp == NULL)
		/* Handle error */;


	while (fgets(path, PATH_MAX, fp) != NULL){
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

}

/*
	For 3.5
*/
typedef struct job {
	int pid;
	struct job* next;
} job;

void printJobs(struct job *job) {
	while (job != NULL) {
		printf(" %d ", job->pid);
     	job = job->next;
	}
}

void appendJob(struct job** head_ref, int new_data)
{
    struct job* new_node = (struct job*) malloc(sizeof(struct job));
 
    struct job *last = *head_ref; 

    new_node->pid  = new_data;
 
    new_node->next = NULL;
 
    if (*head_ref == NULL)
    {
       *head_ref = new_node;
       return;
    }
 
    while (last->next != NULL)
        last = last->next;
 
    last->next = new_node;
    return;
}

void signalHandlerChild() {
	while (waitpid(-1, NULL, WNOHANG) > 0) {
	}
}

void signalHandlerInt() {
	if (kill(pid,SIGTERM) == 0){
		printf("\nProcess %d received a SIGINT signal\n",pid);
	}
}

void executeCommand(char * args[], int background) {
	pid = fork();
	if(pid == 0){
        execvp(args[0], args);
        exit(0);
	}

	//Parent process will wait for child to finish.
	if (background == 0){
		int status;
		waitpid(pid,&status,0);

		 if ( WIFEXITED(status) ) {
                int exit_status = WEXITSTATUS(status);       
                printf("\nExit status [%s] = %d\n", cmd, exit_status);
        }
	}
	if (background == 1) {
		job* new_job = malloc(sizeof(job));
		if (new_job == NULL) {
			exit(2);
		}
		appendJob(&new_job, pid);
		printf("\nProcess created with PID: %d\n",pid);
	}	
}

void startShell(){
	while(!feof(stdin)){
		background = 0;

		takeInput();
        parseInput();
    
        if(!strcmp("cd", cmd)) {
            executeCD(argv);
            continue;
        }

		//Print background jobs. 
		struct job* head = NULL;
		if(strcmp("jobs", cmd) == 0) {
			printJobs(head);
		}

		int i = 0;
		while(argv[i]!=NULL){
				if(strcmp(argv[i], "&") == 0) {
					background = 1;
				}
				else {
					background = 0;
				}
				if(strcmp(argv[i], ">") == 0){
					ioredir(argv, 0, /*retVal*/ argv[i-1], argv[i+1]);
				}
				if (strcmp(argv[i], "<") == 0) {
					ioredir(argv, 0, /*retVal*/ argv[i-1], argv[i-1]);
				} 
			i++;
		}

		executeCommand(argv, background);

	}
}

int main(){

	signal(SIGCHLD, signalHandlerChild);
	signal(SIGINT, signalHandlerInt);
	startShell();
	return 0;
}