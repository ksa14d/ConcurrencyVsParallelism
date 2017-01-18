tar file contains 3 folders

a)OMP folder containing  Makefile + gol.c
b)Pthread folder containing Makefile + gol.c
c)MPI folder containing Makefile + gol.c

Execution Steps :
Step 1. Extract the tar file to a folder in system.
Step 2. Open a Terminal and change the directory path to the folder containing the source.
Step 3. use "make" command to build the code .
Step 4. run the code using 
 a) For Open MP : "./gol w_X w_Y" ex :-  "./gol 1000 1000" 
 b) For Pthreads  : "./gol w_X w_Y  n" ex :-  "./gol 1000 1000 8" where n represents the number of threads
     Note n <= w_X
 c) For MPI         : "./gol w_X w_Y  n" ex :-  "./gol 1000 1000  20"  where n represents the number of processes
most of array of size greater than 1000 achieve a speed up > 4.1 
  Note n <=w_X

*Features of the Open MP implementation of game of life :

1) #pragma omp parellel for is applied on the w_X itertation which parellelizes the columns of the matrix .
2) #pragma omp barrier demonstrates the use of barrier to synchronize .
3) use of reduction clause as a alternative to mutexes .

*Features of the Pthread implementation of game of life : 

1)Pthread provide more flexibility to the programmer by providing nested parallelism and fine grained control.
2)Program demonstrates thread creation , synchronization , barriers using POSIX pthread api. 
3) In the program you can see the i have parallelized all the loops except the file writing.4)the program also shows a simple solution to the problem where the number of rows is not divisible by the number of threads spawned

*Features of the Multi Process Implementation 

1) when compared to the threads which have to deal with the common memory processes have seperate memory provides a insight into pure distributed memory programming
2) if np is the number of processes and w_X is the number of columns then w_X/np = = n columns and two ghosts => w_Y * (n cols + 2)  starting always at 0th index in the w
  is initialized based on process id
3)initial count is exchanged among processes using bidirectional pipes respecting POSIX standards  .
4)ghost column updates takes place with all write followed by reads to take care of "no hangs"
5)each iteration global count is calculated by exchanging count in a circular fashion there by each process incrementing it with local count .
6)finally each process prints to the file by synchronization using pipe read and writes .