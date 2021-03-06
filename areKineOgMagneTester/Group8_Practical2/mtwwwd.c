#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "bbuffer.c"
#include <pthread.h>


#define PORT 8080
#define MAXREQ (4096*1024)
char buffer[MAXREQ], body[MAXREQ], msg[MAXREQ];
void error(const char *msg) { perror(msg); exit(1); }
BNDBUF bbuffer;



#define PATH_LENGTH 100


worker(void *arg){

    int fd;
     while(1){
        fd = bb_get(&bbuffer);
        char * htmlstring = 0;
        long length;
        FILE * fp;
       
    
        bzero(buffer,sizeof(buffer));
        int n = read (fd,buffer,sizeof(buffer)-1);

        if (n < 0) error("ERROR reading from socket");
        char delim[] = "\n";
        printf("%s\n", buffer);
        char *ptr = strtok(buffer, delim);
        printf("%s\n", ptr);


        char delim2[] = " ";
        char *ptr2 = strtok(ptr, delim2);
        printf("%s\n", ptr2);
        ptr2 =strtok(NULL, delim2);
        printf("%s\n", ptr2);
        printf("%s\n", arg);
        char str[10];
        strcpy(str, arg);


        strcat(str,ptr2);
        printf("%s", &str);
        fp = fopen(str, "r");

        if(fp) 
            if (fp) {
                fseek(fp,0, SEEK_END);
                length = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                htmlstring = malloc(length);

                if(htmlstring)

                {
                    fread(htmlstring, 1, length, fp);
                }

                fclose(fp);

            }



        



        snprintf (body, sizeof (body), htmlstring, buffer);
        snprintf (msg, sizeof (msg),
        "HTTP/1.0 200 OK\n"
        "Content-Type: text/html\n"
        "Content-Length: %lu\n\n%s", strlen(body), body);
        n = write (fd,msg,strlen(msg));
        if (n < 0) error("ERROR writing to socket");
    }
};


int main(int argc, char *argv[]) {


    printf("Starting program");
    if (argc < 3) {
        error("Error - command should be on the format: mtwwwd www-path port #threads #bufferslots");
    }

    char *www_path = argv[1];

    int port;
    if (sscanf(argv[2], "%i", &port) != 1) {
        error("\nError - port not an integer");
    }

    printf("\nport: %i", port);

    int n_threads = 1;
    if (argc > 3 && sscanf(argv[3], "%i", &n_threads) != 1) {
        error("\nError - number of threads is not an integer");
    }


    

    int n_bufferslots = 5;
    if (argc > 4 && sscanf(argv[4], "%i", &n_bufferslots) != 1) {
        error("\nError - number of bufferslots is not an integer");
    }

    bbuffer = *bb_init(n_bufferslots);


    for (size_t i = 0; i < n_threads; i++)
    {
        pthread_t threadId;
        int j = pthread_create(&threadId, NULL, &worker, (void*)www_path);
    }

 
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

   
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
    sizeof(serv_addr)) < 0) {
    error("ERROR on binding");
    }
    listen(sockfd,5);
    while (1) {
    clilen = sizeof(cli_addr);
    newsockfd = accept (sockfd, (struct sockaddr *) &cli_addr,
    &clilen);
    if (newsockfd < 0) error("ERROR on accept");
    bb_add(&bbuffer, newsockfd);



 }
}