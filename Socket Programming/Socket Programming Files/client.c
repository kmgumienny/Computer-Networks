#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>

#define RCVBUFSIZE 1028 /* Size of receive buffer */

//Function that gets called when something goes wrong
void DieWithError(char *errorMessage, int errnoNum){
  printf(errorMessage, "/n");
  printf("Errno # %d : %s/n", errno, strerror(errno));
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
  struct timeval t1, t2;

  int socketDescriptor, returnValue; //Used later with socket()
  int pTagSet = 0; //Sets the P tag to print RSS to no by default

  char *servURL, *servPort; // Server URL and port
  char *pntrToPath, *pntrToHost;
  int bytesRcvd, totalBytesRcvd = 0; /* Bytes read in single recv() and total bytes read */
  int bytes, bytesSent, totalBytesSent;



  /*Checks if correct number of arguments*/
  if((argc < 3) || (argc > 4))
    DieWithError("Usage: .client.o <optional RTT print tag (-p)> <Server URL> <Server Port>", 8);

  /*Assigns variables based on whether or not RTT will print*/
  int i = argc - 3;  //determines if there are 3 or 4 arguments
  servURL = argv[1+i];
  servPort = argv[2+i];
  if(i)
    pTagSet = 1;  //if i is equal to 1, there are 4 arguments so p must be set

  // myInfo - this will be the addrinfo that stores what kind of socket we want
  // serverInfo - this will be the addrinfo that iterates through a loop
  // resolvedServer - holds the address info we specified
  struct addrinfo myInfo, *serverInfo, *resolvedServer;
  /*we now set up the packet we will use to do the handshake with the server
    we are programming in C and weird things tend to happen with regards
    to memory so we make sure we have enough space to allocate a
    packet with our information*/
  memset(&myInfo, 0, sizeof(struct addrinfo));
  //we don't care whether we use IPV4 or 6
  myInfo.ai_family = AF_UNSPEC;
  //we're using TCP
  myInfo.ai_socktype = SOCK_STREAM;
  //automatically fills in our IP
  myInfo.ai_flags = AI_PASSIVE;


  /* if a user did not specify the file they want, a "/" is placed to indicate
     to the server that it wants the default file, usually index.html */
  char path[1024];
  pntrToPath = strstr(servURL, "/");
  if(!pntrToPath){
    strcpy(path, "/");
    pntrToHost = servURL;
  }else{
    strcpy(path, pntrToPath);
    pntrToHost = strtok(servURL, "/");
  }
  printf("%s", pntrToHost);

/*Initiate three-way handshake with the server and store the response in
pointer to pointer of addrinfo which is a linked list we will traverse
getaddrinfo(server's url or ip, port we want to connect to, information
about client, linked list (pointer to pointer) of addrinfo)*/
  if((returnValue = getaddrinfo(pntrToHost, servPort, &myInfo, &resolvedServer)) != 0){
    printf("\n\n Return value is %d and %s\n", returnValue, gai_strerror(returnValue));
    DieWithError("Error connecting with Server for 3-way handshake", errno);
  }

    /* Go through all the results getaddrinfo returned until we find
       something valid we can connect to */
  for(serverInfo = resolvedServer; serverInfo != NULL; serverInfo = serverInfo->ai_next){
    /*First try to create a socket with the information
    socket(ipv4 or 6, stream, and protocol)*/
    if((socketDescriptor = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol)) == -1){
      printf("Error creating client socket. Errno #%d", errno);
      continue;
    }
    //Don't have to bind so we can skip right ahead to connecting

    /*Now we will try to connect to the server. Address and length depend on IPv4 or 6
    connect(socket descriptor, server address, server length)*/
    if(connect(socketDescriptor, serverInfo->ai_addr, serverInfo->ai_addrlen) == -1){
      printf("Error connecting with server. Errno #%d", errno);
      continue;
    }
    break;
  }
  //free the memory used by the linked list of addrinfo structure
  freeaddrinfo(resolvedServer);

  /*If we went through the entire linked list of server responses and
    were not able to connect with the server, we shut the program down.*/
  if(serverInfo == NULL)
    DieWithError("Not able to connect with Server after resolving it", 11);


  //Create GET Message
  char getMessage[1024];
  sprintf(getMessage, "GET %s HTTP/1.1\r\nHOST: %s\r\nConnection: close\r\n\r\n", path, pntrToHost);
  printf("%s", getMessage);
  totalBytesSent = strlen(getMessage);
  bytesSent = 0;

  //Here we start calculating how long the RTT is
  gettimeofday(&t1, NULL);

  //We send the GET message to the socket we connected with.
  while(totalBytesSent>0){
    if((bytes = write(socketDescriptor, getMessage, totalBytesSent-bytesSent)) < 0){
      close(socketDescriptor);
      DieWithError("Error writing to socket", errno);
    }if(bytes == 0){
      break;
    }else{
      bytesSent+=bytes;
      continue;
    }
  }

  //This is where we receive a response
  char buffer[RCVBUFSIZE];
  memset(buffer, 0, sizeof(buffer));

  //Reads the response from the server until all of its response comes through
  while((bytesRcvd = read(socketDescriptor, buffer, RCVBUFSIZE - 1)) > 0){
    buffer[bytesRcvd] = 0;
    totalBytesRcvd += bytesRcvd;
    printf("%s", buffer); //Prints the response from the server
  }
  //We stop recording the time
  gettimeofday(&t2, NULL);

  //If we did not recieve anything from the server, we reoprt the error.
  if(totalBytesRcvd == 0)
    DieWithError("Error reading server response", errno);

  //We calculate the RTT for the sending process and print it if user specified

  if(pTagSet){
    long RTT = ((t2.tv_sec - t1.tv_sec) * 1000000LL) + (t2.tv_usec - t1.tv_usec);
  	printf("\nThe RTT is: %ld milliseconds \n", RTT);
  }

  //Lastly we close the socket after we are done recieving a message.
  close(socketDescriptor);
  return(0);
}
