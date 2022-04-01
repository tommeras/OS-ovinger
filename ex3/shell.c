#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PATH_MAX 1024

char* takeInput () {
    char *buffer;
    size_t size = 32;
    size_t characters;

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
       // printf("Current working dir: %s\n", cwd);
    } else {
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

char* parseInput(char *input){
    char * token = strtok(input, " ");
    char * path;
    char * command;
    strcpy(token, path);

   // loop through the string to extract all other tokens
    while( token != NULL ) {
        printf( " %s\n", token ); //printing each token
        token = strtok(NULL, " ");
        strcpy(token, command);
    }
  //  printf("%s %s", path, command);

    return "Hei";
}

int main () {
    char *input = takeInput();
    parseInput(input);
    return 0;
}