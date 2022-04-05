#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PATH_MAX 1024
#define LSH_TOK_BUFSIZE 64


char* takeInput () {
    char *buffer;
    size_t size = 32;
    size_t characters;

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return "Error";
    } 

    buffer = (char *)malloc(size * sizeof(char));
    if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }

    printf("%s: ", cwd);
    characters = getline(&buffer,&size,stdin);
    printf("%s", buffer);
    return buffer;
}

char **parseInput(char *line) {

    int bufsize = LSH_TOK_BUFSIZE, position = 0;

    char **tokens = malloc(bufsize * sizeof(char*));

    char *token;
    token = strtok(line, " ");

    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " ");
    }    

    if (strcmp(tokens[0],"cd")){

        execv(tokens[0], tokens);
        
    } else{
        printf("Dette funket ikke \n");
    }
    return tokens;
}


int main () {
    do{
        char *input = takeInput();
        char** arguments = parseInput(input);
    }
    while (1); 
        printf("Exit status: %d", EXIT_SUCCESS);    
    
}