CC = gcc
CFLAGS = -g -Wall

all: p3

p3: project3.o node0.o node1.o node2.o node3.o
	$(CC) $(CFLAGS) project3.c node0.c node1.c node2.c node3.c -o project3


clean:
	rm -rf project3 project3.o node0.o node1.o node2.o node3.o
