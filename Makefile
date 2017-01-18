CC = gcc
CFLAGS= -O3 -Wall -ansi  -pedantic -fopenmp

all: gol 
clean:
	rm -f *.o
	rm -f gol
	rm -f *~
	rm -f core

gol.o: gol.c
	$(CC) $(CFLAGS) -g -c gol.c

gol:  gol.o 
	$(CC) $(CFLAGS) -g  -o gol gol.o 
  
demo:	gol 
	./gol
	
