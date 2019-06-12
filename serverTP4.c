#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>
#include "hangman.h"
#include "serverTP4.h"

#define MAX_PENDING 5
#define BUFF_SIZE 200

void end(char *msg) { perror(msg); exit(1); }

void handleClient(int sock) {
  startHangman(sock);
  close(sock);
  return;
}

int main(int argc, char const *argv[])
{
  int serverSock, clientSock;
  struct sockaddr_in echoServer, echoClient;

  if (argc != 2) {
    fprintf(stderr, "USAGE: ./serverTP4 <port>\n");
    exit(1);
  }
  /* Create the TCP socket */
  if ((serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    end("Failed to create socket");
  }
  /* Constryct the server sockaddr_in structure */
  memset(&echoServer, 0, sizeof(echoServer));   /* Clear struct */
  echoServer.sin_family = AF_INET;    /* Internet/IP */
  echoServer.sin_addr.s_addr = htonl(INADDR_ANY);  /* Incoming addr */
  echoServer.sin_port = htons(atoi(argv[1]));   /* Server port */
  /* Bind the server socket */
  if (bind(serverSock, (struct sockaddr *) &echoServer,
              sizeof(echoServer)) < 0) {
    end("Failed to bind server to socket");
  }
  /* Listen on the server socket */
  if (listen(serverSock, MAX_PENDING) < 0) {
    end("Failed to listen on server socket");
  }
  /* Run until cancelled */
  while(1) {
    unsigned int clientLen = sizeof(echoClient);
    /* Wait for client connection */
    if ((clientSock = accept(serverSock, (struct sockaddr*) &echoClient,
                                &clientLen)) < 0) {
      end("Failed to accept client connection");
    }
    if (fork() == 0) {
      handleClient(clientSock);
    }
  }
  return 0;
}

char readChar(int sock) {
  char buffer[BUFF_SIZE];
  if (recv(sock, buffer, BUFF_SIZE, 0) < 1) {
    end("Failed to read character");
  }
  return buffer[0];
}

int readInt(int sock) {
  char buffer[BUFF_SIZE];
  if (recv(sock, buffer, BUFF_SIZE, 0) < 1) {
    end("Failed to read character");
  }
  int i = 0;
  while(buffer[i] != '\n') {
    if (buffer[i] > 57 || buffer[i] < 48) {
      return -1;
    }
    i++;
  }
  buffer[i] = 0;
  return atoi(buffer);
}