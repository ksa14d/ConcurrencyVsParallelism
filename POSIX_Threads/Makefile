CC = gcc
CFLAGS= -O3 -Wall -ansi -pedantic 
LIBS = -lpthread\

all: gol 

clean:
	rm -f *.o
	rm -f gol
	rm -f *~
	rm -f core

gol.o: gol.c
	$(CC) $(CFLAGS) -g -c gol.c ${LIBS}

gol:  gol.o 
	$(CC) $(CFLAGS) -g  -o gol gol.o ${LIBS}
  
demo:	gol 
	./gol
	
