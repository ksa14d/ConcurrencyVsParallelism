/* 
 * Created 06/20/2015 by Xin Yuan for the COP5570 class
 */
#define _XOPEN_SOURCE 600 
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#define MAX_N 8191

#ifndef DEBUG_LEVEL 
#define DEBUG_LEVEL 0
#endif
int Nthreads;
char w[MAX_N][MAX_N];
char neww[MAX_N][MAX_N];
int threadCount ;
pthread_barrier_t barr;
pthread_mutex_t init_count_mutex;

int myid[MAX_N];
pthread_t tid[MAX_N];
int w_X, w_Y;
int init_count,count;

void *init1(void *arg)
{ 
  
  int i, j , myrank , rc;
  myrank = *(int *)arg;
  for (i=0; i<w_X;i++)
    for (j=myrank ; j<w_Y; j+=Nthreads ) {
      w[j][i] = 0;
 }
    rc = pthread_barrier_wait(&barr);
    if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
    {
        printf("Could not wait on barrier\n");
        exit(-1);
    }

  for (i=myrank; i<w_X; i+=Nthreads) w[0][i] = 1;
  for (i=myrank; i<w_Y; i+=Nthreads) w[i][0] = 1;
  
    
  pthread_exit(NULL);
}

void test_init2()
{ 
  int i, j;
  w_X = 4;
  w_Y = 6;

  for (i=0; i<w_X;i++)
    for (j=0; j<w_Y; j++) 
      w[j][i] = 0;
  w[0][3] = 1;
  w[1][3] = 1;
  w[2][1] = 1;
  w[3][0] = w[3][1] = w[3][2] = w[4][1] = w[5][1] = 1;
}

void print_world()
{
  int i, j;

  for (i=0; i<w_Y; i++) {
    for (j=0; j<w_X; j++) {
      printf("%d", (int)w[i][j]);
    }
    printf("\n");
  }
}

int neighborcount(int x, int y)
{
  int count = 0;

  if ((x<0) || (x >=w_X)) {
    printf("neighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x,y, 
	   w_X, w_Y);
    exit(0);
  }
  if ((y<0) || (y >=w_Y)) {
    printf("neighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x,y, 
	   w_X, w_Y);
    exit(0);
  }

  if (x==0) {
    if (y == 0) {
      count = w[y][x+1] + w[y+1][x] + w[y+1][x+1];
    } else if (y == w_Y-1) {
      count = w[y][x+1] + w[y-1][x] + w[y-1][x+1];
    } else {
      count = w[y-1][x] + w[y+1][x] + w[y-1][x+1] + w[y][x+1] + w[y+1][x+1];
    }
  } else if (x == w_X -1) {
    if (y == 0) {
      count = w[y][x-1] + w[y+1][x-1] + w[y+1][x];
    } else if (y == w_Y-1) {
      count = w[y][x-1] + w[y-1][x] + w[y-1][x-1];
    } else {
      count = w[y-1][x] + w[y+1][x] + w[y-1][x-1] + w[y][x-1] + w[y+1][x-1];
    }
  } else { /* x is in the middle */
    if (y == 0) {
      count = w[y][x-1] + w[y][x+1] + w[y+1][x-1] + w[y+1][x] + w[y+1][x+1];
    } else if (y == w_Y-1) {
      count = w[y][x-1] + w[y][x+1] + w[y-1][x-1] + w[y-1][x] + w[y-1][x+1];
    } else {
      count = w[y-1][x-1] + w[y][x-1] + w[y+1][x-1] + w[y-1][x] + w[y+1][x] 
              + w[y-1][x+1] + w[y][x+1] + w[y+1][x+1];
    }
  }

  return count;
}


void* GetInitCount(void *arg)
{
  int x, y ,myrank , c = 0;
  myrank = *(int *)arg;
  for (x=myrank; x<w_X; x+=Nthreads) {
    for (y=0; y<w_Y; y++) {
      if (w[y][x] == 1) c++;
    }
  }
  
   pthread_mutex_lock(&init_count_mutex);
   init_count+= c;     
   pthread_mutex_unlock(&init_count_mutex);
   pthread_exit(NULL);
}

void* GetNewWorld(void* arg)
{


  int iter, x, y ,myrank , rc , c = 0, c2 =0;

  myrank = *(int *)arg;
  for (iter = 0; (iter < 200) && (count <50*init_count) && 
	 (count > init_count / 50); iter ++) 
  {
    rc = pthread_barrier_wait(&barr);
    if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
    {
        printf("Could not wait on barrier\n");
        exit(-1);
    }
    count = 0; 
  
  for (x=myrank; x<w_X; x+=Nthreads) {
      for (y=0; y<w_Y; y++) {
        c = neighborcount(x, y);  /* count neighbors */
        if (c <= 1) neww[y][x] = 0;      /* die of loneliness */
        else if (c >=4) neww[y][x] = 0;  /* die of overpopulation */
        else if (c == 3)  neww[y][x] = 1;             /* becomes alive */ 
	else neww[y][x] = w[y][x];   /* c == 2, no change */
      }
    }
   
    rc = pthread_barrier_wait(&barr);
    if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
    {
        printf("Could not wait on barrier\n");
        exit(-1);
    }  
   
  /* copy the world, and count the current lives */
    c2 = 0;

    for (x=myrank; x<w_X; x+=Nthreads) {
      for (y=0; y<w_Y; y++) {
	w[y][x] = neww[y][x];
	if (w[y][x] == 1) c2++;
      }
    }

   pthread_mutex_lock(&init_count_mutex);
   count+= c2;    
   threadCount++;
   if(threadCount==Nthreads)
   {
    printf("iter = %d, population count = %d\n", iter, count); 
    threadCount = 0;
   } 
   pthread_mutex_unlock(&init_count_mutex);
   
    rc = pthread_barrier_wait(&barr);
    if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
    {
        printf("Could not wait on barrier\n");
        exit(-1);
    } 

  } 
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int x, y;
  int i,rc;
 
  struct timeval start, end; /* for timing */

   if(pthread_mutex_init(&init_count_mutex, NULL))
    {
        printf("Unable to initialize a mutex\n");
        return -1;
    }
  
   /* Now begins the real work which we want to parallelize. */
  /* Mark the starting time. */
  if (gettimeofday(&start, NULL) < 0) {
    perror(": gettimeofday");
    exit(0);
  }

  if (argc == 1 ||argc == 2) {
    printf("Usage: ./a.out w_X w_Y\n");
    exit(0);
  } else if (argc == 3){ 
    test_init2();Nthreads = atoi(argv[2]);
  }
  else{ /* more than three parameters */
    
    w_X = atoi(argv[1]);  
    w_Y = atoi(argv[2]);
    Nthreads = atoi(argv[3]);
    if(pthread_barrier_init(&barr, NULL, Nthreads))
    {
        printf("Could not create a barrier\n");
        return -1;
    }
   /*-------------------------init2----------*/
    for (i=0; i<Nthreads; i++) {
    myid[i] = i;
    rc = pthread_create(&tid[i], NULL, &init1, &myid[i]);
    if(rc)
    {
       printf("Error creating thread\n");
       exit(-1);
    }
  } 
   for (i=0; i<Nthreads; i++) {
    pthread_join(tid[i], NULL);
  } 
     /*-------------------------init2----------*/

  }
  

     /*-------------------------initcount----------*/
  for (i=0; i<Nthreads; i++) {
    myid[i] = i;
    rc = pthread_create(&tid[i], NULL, &GetInitCount, &myid[i]);
    if(rc)
    {
       printf("Error creating thread\n");
       exit(-1);
    }
  } 
   for (i=0; i<Nthreads; i++) {
    pthread_join(tid[i], NULL);
  } 
  /*-------------------------initcount----------*/
  count = init_count;
  

  printf("initial world, population count: %d\n", init_count);
  /* if (DEBUG_LEVEL > 10) print_world(); */

 /*------------------------GetNewWorld----------*/
    for (i=0; i<Nthreads; i++) {
    myid[i] = i;
    rc = pthread_create(&tid[i], NULL, &GetNewWorld, &myid[i]);
    if(rc)
    {
       printf("Error creating thread\n");
       exit(-1);
    }
  } 
   for (i=0; i<Nthreads; i++) {
    pthread_join(tid[i], NULL);
  } 
 /*-------------------------GetNewWorld----------*/

   
 
   /* if (DEBUG_LEVEL > 10) print_world(); */
 

  {
    FILE *fd;
    if ((fd = fopen("final_world000.txt", "w")) != NULL) {
      for (x=0; x<w_X; x++) {
	for (y=0; y<w_Y; y++) {
          fprintf(fd, "%d", (int)w[y][x]);
	}
	fprintf(fd, "\n");
      }
    } else {
      printf("Can't open file final_world000.txt\n");
      exit(1);
    }
  }
/* Work's done. Get the elapsed wall time. */
  if (gettimeofday(&end, NULL) < 0) {
    perror(": gettimeofday");
    exit(0);
  }

   fprintf( stdout, "\nwall time = %.2f ms \n",
	   (end.tv_sec-start.tv_sec)*1000.0
         + (end.tv_usec - start.tv_usec) / 1000.0 );

  return 0;
}
