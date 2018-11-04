#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define RCVBUFSIZE 1028
#define QUEUE 10

int openConnection(char *port);
void DieWithErrors(char *errorMessage, int errnoNum) {
  printf(errorMessage, "/n");
  printf("Errno # %d : %s/n", errno, strerror(errno));
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
  int newSocket, lstnSocket;

  // Checks to see if user gave correct number of command line arguments
  if (argc != 2)
    // If the program does not have the correct number of arguments, it crashes.
    DieWithErrors("Usage: ./server <port>", 8);

  // Program gets a socket to listen on
  lstnSocket = openConnection(argv[1]);

  if (listen(lstnSocket, QUEUE) < 0) // If listening fails, the program crashes
    DieWithErrors("Error Listening.", errno);

  // Notifies user that the program is indeed running
  printf("Waiting for connection now.\n");

  // This part of the code listens for a connection and sends a file.
  while (1) {
    newSocket = accept(lstnSocket, NULL, NULL); // Accept incoming connection

    // Checks to see the connection was actually accepted
    if (newSocket < 0)
      DieWithErrors("Error accepting connection", errno);
    if (fork() == 0) {
      char rcvBuffer[RCVBUFSIZE]; // Buffer that accepts incoming message in
                                  // parts
      memset(rcvBuffer, 0, sizeof(rcvBuffer)); // clears memory for the buffer
      int readResult; // User to store the number of bytes read

      // Reads the header sent by client. Stops at the first new line.
      while ((readResult = read(newSocket, rcvBuffer, sizeof(rcvBuffer))) > 0) {
        rcvBuffer[readResult] = 0;
        printf("%s", rcvBuffer);
        if (strstr(rcvBuffer, "\r\n"))
          break;
      }

      if (readResult < 0)
        DieWithErrors("Error reading host message", errno);

      printf("%s\n", rcvBuffer);
      // We know the GET message goes GET (some path) HTTP/1.1
      char rqstMethod[6], rqstPath[100], rqstHttp[10], errorMessage[50];
      sscanf(rcvBuffer, "%s %s %s", rqstMethod, rqstPath, rqstHttp);
      // First Check to see if the client is sending a GET request
      if (strcmp(rqstMethod, "GET") != 0) {
        // If it is not a get request, error message is returned.
        strcpy(errorMessage, "Expected GET method");
        // Write error message to host unless error
        if (write(newSocket, errorMessage, strlen(errorMessage)) < 0) {
          close(newSocket);
          close(lstnSocket);
          DieWithErrors("Error sending error message to host", errno);
        }
        // Close the client's socket and continue listening.
        close(newSocket);
        continue;
        // If it is a get method continue
      } else {
        /*If the user is requesting "/", "/index.html" or "/TMDG.html" send
          TMDG.html*/
        if (strcmp(rqstPath, "/") == 0 ||
            strcmp(rqstPath, "/index.html") == 0 ||
            strcmp(rqstPath, "/TMDG.html") == 0) {
          FILE *file;
          long fileSize;
          // Opens file in the directory called TMDG.html
          file = fopen("TMDG.html", "r");
          // Checks to make sure the file exists, if not close program
          if (file == NULL) {
            close(newSocket);
            close(lstnSocket);
            DieWithErrors("Error opening file", 2);
          } else {
            // File is open and ready to transmit. Load file and send it.
            char okMessage[strlen("HTTP/1.1") + strlen("200 OK\n") + 1];
            sprintf(okMessage, "%s 200 OK\n\n", rqstHttp);
            if (write(newSocket, okMessage, strlen(okMessage)) < 0) {
              close(newSocket);
              close(lstnSocket);
              DieWithErrors("Error sending OK Message", errno);
            }

            // Get the size of the document to allocate space for it
            fseek(file, 0, SEEK_END);
            fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);
            // Allocate space to hold the files
            char *sendBuffer = malloc(fileSize * sizeof(char) + 1);
            memset(sendBuffer, 0, fileSize);

            fread(sendBuffer, fileSize, 1, file);
            fclose(file);
            sendBuffer[fileSize] = 0;
            // integers to keep tabs on how much data is being sent
            int bytes, bytesSent;
            // Send the file in chunks to the client until all of it is sent
            bytesSent = 0;
            while (fileSize > 0) {
              // If there is an error writing, close program.
              if ((bytes = write(newSocket, sendBuffer, fileSize - bytesSent)) <
                  0) {
                close(newSocket);
                close(lstnSocket);
                DieWithErrors("Error writing to socket", errno);
              }
              // If the file is completely sent end loop, else keep sending
              if (bytes == 0)
                break;
              else {
                bytesSent += bytes;
                continue;
              }
            }
            // Once we are done sending the file, we can free malloc'd space
            free(sendBuffer);
          }
        }
      }
      // Once file is sent, close the socket.
      close(newSocket);
    } else
      close(newSocket);
  }
  // We've done our job so server closes.
  close(lstnSocket);
}

int openConnection(char *port) {
  // This will be the addrinfo that stores what kind of socket we want
  struct addrinfo myInfo;
  /* This will be the structure that holds info on the addrstructure we
  specified */
  struct addrinfo *resolvedServer;
  // Integer to hold socket number
  int listenSocket;
  // variable needed when reusing a socket
  int yes = 1;
  /* we are programming in C and weird things tend to happen with
  regards to memory so we make sure we have enough space to
  allocatea packet with our information */
  memset(&myInfo, 0, sizeof(struct addrinfo));
  // we don't care whether we use IPV4 or 6
  myInfo.ai_family = AF_UNSPEC;
  // we're using TCP
  myInfo.ai_socktype = SOCK_STREAM;
  // automatically fills in our IP
  myInfo.ai_flags = AI_PASSIVE;

  // Creates a structure in resolvedServer for the socket we will create
  if (getaddrinfo(NULL, port, &myInfo, &resolvedServer) != 0)
    DieWithErrors("Error connecting with Server for 3-way handshake", errno);

  // If no results, the program shuts down in error.
  if (resolvedServer == NULL)
    DieWithErrors("Error binding.", errno);

  // We create a socket to listen for connections on
  listenSocket = socket(resolvedServer->ai_family, resolvedServer->ai_socktype,
                        resolvedServer->ai_protocol);
  // If the socket is not created, program ends
  if (listenSocket < 0)
    DieWithErrors("Error creating client socket. Errno #%d", errno);

  /* This takes a port and reuses it for this program in case that port
     is busy, useful when running this server multiple times */
  if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
      -1)
    DieWithErrors("Error reusing a socket.", errno);

  // Bind the socket so we can listen for incoming connections
  if (bind(listenSocket, resolvedServer->ai_addr, resolvedServer->ai_addrlen) <
      0) {
    close(listenSocket);
    DieWithErrors("Binding Error. Errno # %d\n", errno);
  }
  // Retuns the socket we are listening on
  return listenSocket;
}
