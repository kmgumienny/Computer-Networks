kmgumienny

In this project, I implemented the distance vector routing algorithm between 4 nodes. Nodes share their distance with their neighbors and their neighbors try and update their routing table, in which case they would again broadcast their updated table to their neighbors. This continues until every node knows the shortest path.


###Compiling the code###
There is a makefile included in the project files.

Navigate to the directory with the code in terminal
Once in the project directory, type "make" or "make all" in terminal to compile the code
The object files are compiled into "project3." To run the code type "./project3" in cmd/terminal

###Code Parameters for Project3###

Enter Trace Level: [enter integer >= 0]

###Cleaning Object Files###

In the project directory, type "make clean" to remove all object files
