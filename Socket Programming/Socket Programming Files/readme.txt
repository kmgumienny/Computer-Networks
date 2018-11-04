Kamil Gumienny
kmgumienny@wpi.edu
CS3516

The project file includes two code files, server.c client.c.

To compile the code, make sure the makefile is in the directory.
In command prompt, type "make" to create client.o, client, server.o. serverInfo

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
