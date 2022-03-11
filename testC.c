#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#define SIZE 1024
 
void read_file(int sockfd){
  int n;
  FILE *fp;
  char *filename = "test.txt";
  char buffer[SIZE];
 
  fp = fopen(filename, "r");
  while (1) {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0){
      break;
      return;
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);
  }
  return;
}
 
int main(){
  char *ip = "127.0.0.1";
  int port = 8080;
  int e;
 
  int sockfd;
  struct sockaddr_in server_addr;
  FILE *fp;
  char *filename = "recv.txt";
 
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");
 
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);
 
  e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e == -1) {
    perror("[-]Error in socket");
    exit(1);
  }
 printf("[+]Connected to Server.\n");
 
  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("[-]Error in reading file.");
    exit(1);
  }
 
  read_file(sockfd);
  printf("[+]File data sent successfully.\n");
 
  printf("[+]Closing the connection.\n");
  close(sockfd);
 
  return 0;
}