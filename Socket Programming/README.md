# CS3516_Project1
kmgumienny

This project is an introduction to how programs work over a network. You are able to connect to the server via a TCP socket and:
o Submit a valid HTTP/1.1 GET request for the supplied URL.
o Read the server's response and display it on the terminal
o Print the RTT for accessing the URL on the terminal showing the server's response time and display it on the terminal


The project file includes two code files, server.c client.c.

To compile the code, make sure the makefile is in the directory.
In command prompt, type "make" to create client.o, client, server.o. serverInfo

###Compiling the code###
To run the Client, in command prompt type:
  ./client <-p tag> <URL> <Port #>
For example the command below will connect to www.google.com on port 80:
  ./client -p www.google.com 80
The -p tag prints the a single RTT between client and server at the end of the data stream

To run the server, in command prompt type:
  ./server <port>
For example the command below will open the server on port 6859:
  ./server 6859
When choosing a port, pick one greater than 1023 and lower than 65535

###Cleaning Object Files###
1) In the project directory, type "make clean"
