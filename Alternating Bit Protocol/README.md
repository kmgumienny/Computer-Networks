# CS3516_Project2
kmgumienny

In this project I recreated the Alternating-Bit-Protocol, a proposed solution to the reliable transfer problem in networking. Most of the code was provided for this project by the instructor. My work can be found in the student2.c file.

###Compiling the code###
There is a makefile included in the project files.
1) Navigate to the directory with the code in terminal
2) Once in the project directory, type "make" or "make all" in terminal to compile the code
3) The object files are compiled into "gbn." To run the code type ./gbn to run the code

###Code Parameters for ABP Simulation###
1) Enter the number of messages to simulate: [enter number > 0]
2) Packet loss probability : [enter number between 0.0 and 1.0]
3) Packet corruption probability: [enter number between 0 up to and including 1 or 0.0 for no corruption]
4) Packet out-of-order probability: [enter number between 0 up to and including 1 or 0.0 for no out-of-order]
5) Average time between messages from sender's layer5: [ > 0.0]
6) Enter Level of tracing desired: [Enter number 0-5]
7) Do you want actions randomized: [1 = yes, 0 = no]
8) Do you want Bidirectional: [1 = yes, 0 = no]

###Cleaning Object Files###
1) In the project directory, type "make clean"
