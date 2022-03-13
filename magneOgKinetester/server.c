#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
//#include "bbuffer.h"

/*
    1. Get your basic TCP sockets layer running (listen on port/ports, accept client connections and send/receive data).
    2. Implement a buffered reader so that you can read requests one line (delimited by CRLF) at a time.
    3. Read the very first line. Parse out the method, the request version and the path.
    4. Implement header parsing for the "Header: value" syntax. Don't forget unfolding folded headers.
    5. Check the request method, content type and content size to determine how/if the body will be read.
    6. Implement decoding of content based on content type.
    7. If you're going to support HTTP 1.1, implement things like "100 Continue", keep-alive, chunked transfer.
    8. Add robustness/security measures like detecting incomplete requests, limiting max number of clients etc.
*/

#define DEFAULT_BUFFER_SIZE 2048
#define PATH_LENGTH 100

/* Define response header. NOTE: uses HTTP v1.0 to simplify number of headers */
char HTTP_HEADERS_SUCCESS[] = "HTTP/1.1 200 OK\r\n"
                              "Content-type: text/html\r\n\r\n ";

char HTTP_HEADERS_NOT_FOUND[] = "HTTP/1.1 404 Not Found\r\n";

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int santitizePath(char path[], char *ptr)
{
    char *newPath = (char *) malloc(sizeof(char)*PATH_LENGTH);
    char previous_char = path[0];
    newPath[0] = path[0];
    for(int i = 1, counter = 1; i < strlen(path); i++)
    {
        if((previous_char == path[i]) && (previous_char == '.'))
        {
            continue;
        }
        previous_char = path[i];
        newPath[counter] = path[i];
        counter++;
    }
    printf("\nnewPath: %s", newPath);
    strcpy(ptr,newPath);
    free(newPath);
    return 1;
}

int getFilepathFromRequest(char request[], char filepath[PATH_LENGTH])
{
    /* First line should be like: "GET <filepath> HTTP/1.1" */
    char firstLine[100];
    sscanf(request, "%[^\r\n]", firstLine);

    char requestType[100];
    char protocol[100];

    int n = sscanf(firstLine, "%[a-zA-Z] %s %s", requestType, filepath, protocol);
    if (n != 3)
        error("Error - Could not parse request");
    printf("\n\nRequestType: %s\nFilepath: %s\nProtocol: %s\n", requestType, filepath, protocol);
    return 1;
}

// run command with: mtwwwd www-path port #threads #bufferslots
int main(int argc, char *argv[])
{
    // Parse command line input
    printf("Starting program");
    if (argc < 3)
        error("Error - command should be on the format: mtwwwd www-path port #threads #bufferslots");

    char *www_path = argv[1];

    printf("\nwww_path: %s", www_path);

    int port;
    if (sscanf(argv[2], "%i", &port) != 1)
        error("\nError - port not an integer");

    printf("\nport: %i", port);

    int n_threads = 1;
    if (argc > 3 && sscanf(argv[3], "%i", &n_threads) != 1)
        error("\nError - number of threads is not an integer");

    int n_bufferslots = 5;
    if (argc > 4 && sscanf(argv[4], "%i", &n_bufferslots) != 1)
        error("\nError - number of bufferslots is not an integer");

    char buffer[DEFAULT_BUFFER_SIZE];

    // set up socket
    struct sockaddr_in svr_addr, cli_addr;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        error("\nError - Could not create socket");

    // Change to getaddrinfo(2) ??
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0)
        error("Error - Could not bind socket");

    printf("\nListening...");
    if (listen(socket_fd, n_bufferslots) < 0)
        error("\nError - listening");

    while (1)
    {
        int addr_size = sizeof(struct sockaddr_in);
        int accepted_socket_fd = accept(socket_fd, (struct sockaddr *)&cli_addr, (socklen_t *)&addr_size);
        if (accepted_socket_fd < 0)
            error("Error - Could not accept");

        bzero(buffer, sizeof(buffer));

        /* Read HTTP request */
        int n = read(accepted_socket_fd, buffer, sizeof(buffer) - 1);

        char request_path[PATH_LENGTH];
        getFilepathFromRequest(buffer, request_path);
        char *sanitized_path = (char *) malloc(sizeof(char)*PATH_LENGTH);
        santitizePath(request_path, sanitized_path);
        printf("\nSanitized_path: %s", sanitized_path);
        char *fullpath = malloc(strlen(www_path) + strlen(sanitized_path));
        sprintf(fullpath, "%s%s", www_path, sanitized_path);
        if (fullpath == NULL)
            error("Error - fullpath");
        printf("\nfullpath: %s", fullpath);
        char file_content[DEFAULT_BUFFER_SIZE / 2]; /* TODO fix flush buffer and data between requests */
        char data[DEFAULT_BUFFER_SIZE];
        char headers[sizeof HTTP_HEADERS_SUCCESS];
        FILE *file_ptr;
        printf("\nBefore fopen");
        file_ptr = fopen(fullpath, "r");
        printf("\nAfter fopen");
        free(sanitized_path);

        if (file_ptr == NULL)
        {
            printf("\nFile not found");
            snprintf(headers, sizeof(headers),
                     "%s",
                     HTTP_HEADERS_NOT_FOUND);
        }
        else
        {
            snprintf(headers, sizeof(headers),
                     "%s",
                     HTTP_HEADERS_SUCCESS);

            // Read from file
            fseek(file_ptr, 0, SEEK_END);
            long fsize = ftell(file_ptr);
            fseek(file_ptr, 0, SEEK_SET);  /* same as rewind(file_ptr); */
            
            char *string = malloc(fsize + 1);
            fread(string, fsize, 1, file_ptr);
            strcpy(file_content, string);

            free(string);
            fclose(file_ptr);
        }

        snprintf(data, sizeof(data),
                 "%s\n%s",
                 headers, file_content);

        n = write(accepted_socket_fd, data, strlen(data));
        if (n < 0)
            error("Error - writing to socket");

        free(fullpath);
        close(accepted_socket_fd);
    }
    close(socket_fd);
    return 0;
}