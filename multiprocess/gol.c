/* 
 * Created 06/20/2015 by Xin Yuan for the COP5570 class
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#define MAX_N 8191

#ifndef DEBUG_LEVEL 
#define DEBUG_LEVEL 0
#endif

//char w[MAX_N][MAX_N];
//char neww[MAX_N][MAX_N]; 
char **w;
char **neww;

/*MPI related */
struct Process
{
  int p_id;
  int myrank;
  int start;
  int end;
  int range;
} Processes[50];

struct Data
{
  int myrank;
  int type; //custom protocol 1 -> row | 2 -> local_count | 3 -> global_count
  char col[MAX_N];
  int local_count;
  int global_count;
};
int n_procs;
/*MPI related */


int w_X, w_Y;


void init1(struct Process p)
{ 
   int i,j = 0;
   int l_X = p.range+ 2;
   w = new char*[w_Y +1];
   neww = new char*[w_Y +1];
   for(int i = 0 ;i < w_Y +1 ; i++)
   {
     w[i] = new char[l_X];
     neww[i] = new char[l_X];
   }
   for(j = 0; j< w_Y ; j++)
     for(i = 0; i< l_X; i++)
        w[j][i] = 0;
   for(j = 0 ; j < l_X ; j++)
        w[0][j]  = 1 ;
   if(p.start == 0)
   {
     w[0][0] = 0 ;
     for(i = 0; i < w_Y; i++)
          w[i][1] = 1;
   }
   if(p.myrank == 1 && w_X == n_procs)
   {
     for(i = 0; i < w_Y; i++)
          w[i][0] = 1;
   }
   if(p.end == w_X)
   {
       w[0][l_X -1] = 0; 
   }
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

void print_world(struct Process p)
{
  int i, j;
  int l_X =  p.range + 2;
  printf("------------------------\n\n");
  for (i=0; i<w_Y; i++) {
    for (j=0; j<l_X; j++) {
      printf("%d", (int)w[i][j]);
    }
    printf("\n");
  }
  printf("-------------------------\n\n");
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

void mpi_create_proc(int cols, int procs)
{
  int i, x = cols % procs , n_cols = cols / procs;
  for(i = 0 ; i < procs ; i++)
  {
    Processes[i].myrank = i;
    if(i == 0)Processes[i].start = 0;
    else Processes[i].start = Processes[i-1].end;
    if(i < x) Processes[i].end = Processes[i].start + n_cols + 1;
    else Processes[i].end = Processes[i].start + n_cols;
    Processes[i].range = Processes[i].end - Processes[i].start;
  //  printf("rank start end range \n");
 // printf("%d      %d       %d       %d  \n",Processes[i].myrank , Processes[i].start , Processes[i].end , Processes[i].range);

  }
}

    FILE *fd;
void PrintFinalWorld(struct Process p)
  {
    int x,y;
     for (x= 1; x<= p.range ; x++) {
        for (y = 0; y< w_Y; y++) {
             fprintf(fd, "%d", (int)w[y][x]);
//             printf("%d",(int)w[y][x]);
	}
	fprintf(fd, "\n");
//	printf("\n");
      }
      fflush(fd);
  }

void CopyColumn(struct Data *d,int col,bool from )
{
   if(from)
   {  
      for(int i = 0 ; i < w_Y ; i++)
      {
        d->col[i] = neww[i][col];	
      }
   }
   else
   {
      for(int i = 0 ; i < w_Y ; i++)
      {
        w[i][col]  = d->col[i]; 
      }
   }
}

int main(int argc, char *argv[])
{
  int x, y, i;
  int iter = 0;
  int c;
  int init_count;
  int count;
  
    if ((fd = fopen("final_world000.txt", "w")) == NULL) {
         perror("open file failed");
    }

  if (argc == 1 || argc == 2) {
    printf("Usage: ./a.out w_X w_Y P\n");
    exit(0);
  } else if (argc == 3) {
    test_init2();n_procs = atoi(argv[2]);
  }
  else{ /* more than three parameters */
    w_X = atoi(argv[1]);
    w_Y = atoi(argv[2]);
    n_procs = atoi(argv[3]);
    mpi_create_proc(w_X,n_procs);
//   for(int i=0 ; i< n_procs ; i++)
  //  { 
  //  init1(Processes[i]);
   //   print_world(Processes[i]);
   // }
   
  }
  ssize_t sz;
  int* childs =  new int[n_procs];
  int** lst_pipes[2];
  for(int i =0 ; i < 2; i++)
  {
    lst_pipes[i] = new int*[n_procs - 1];
    for(int j = 0 ; j < n_procs -1 ; j++)
    {
      lst_pipes[i][j] = new int[2];
      int *fds = lst_pipes[i][j];
      int ret = pipe(fds);
      if(ret == -1)perror("Error piping");
    }
  }
  int **ipipes = lst_pipes[0];
  int **opipes = lst_pipes[1];
  int *ofd = NULL , *ifd = NULL , *opfd = NULL, *ipfd = NULL;
  for(i = 0 ; i< n_procs ; i++)
  {
    if(i < n_procs - 1) 
    {
       ofd = opipes[i]; 
       ifd = ipipes[i];
    }
    if(i > 0)
    {
       opfd = opipes[i-1];
       ipfd = ipipes[i-1];
    }
    if((childs[i] = fork()) == 0)
    { 
     int owrite = -1 , oread = -1 , iwrite = -1 , iread = -1 ;
     if(i > 0)
     {
        oread = dup(opfd[0]);   
        iwrite = dup(ipfd[1]);
     }
     if(i < n_procs - 1)
     {
        owrite = dup(ofd[1]);
        iread = dup(ifd[0]);
     }
     init1(Processes[i]);
      w_X =  Processes[i].range + 2;
   //  print_world(Processes[i]);

  printf("closing pipes in child\n");
     for(int j = 0 ;j <n_procs -1 ; j++)
        for(int k = 0 ; k < 2; k++)
        {
           close(opipes[j][k]);close(ipipes[j][k]);
        }
     c = 0;
     for (x= 1; x<= Processes[i].range ; x++) {
        for (y = 0; y< w_Y; y++) {
            if (w[y][x] == 1) c++;
        }
      } 
      
      // printf("initial world, population local count: %d\n", c);
      /* Logic to exchange local and global count */
      struct Data lc;
      if(n_procs > 1){
      if(i == 0)
      {
         // first  owrite(local) -> iread(global) - > signal all
         lc.local_count = c;
         sz = write(owrite,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         sz = read(iread,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
        // printf("initial world, population count: %d\n",lc.local_count);
      }
      else if(i == n_procs -1)
      {
           // last  oread(local) -> iwrite
         sz = read(oread,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         lc.local_count += c;
         sz = write(iwrite,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
      }
      else
      {
             // intermediate oread(local) -> owrite(+mylocal) -> ireada
         sz = read(oread,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         lc.local_count += c;
         sz = write(owrite,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         sz = read(iread,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         sz = write(iwrite,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
      }
      c = lc.local_count;
      }
      /* Logic to exchange local and global count */

    if(i == 0)
      printf("initial world, population count: %d\n",c);
      init_count = c;
      count = init_count;
       //size_t byt_y = w_Y * sizeof(int);
       int i_last = Processes[i].range + 1;

  for (iter = 0; (iter < 200) && (count <50*init_count) && 
	 (count > init_count / 50); iter ++) {
     for (x= 1; x<= Processes[i].range ; x++) {
        for (y = 0; y< w_Y; y++) {
        c = neighborcount(x, y);  /* count neighbors */
        if (c <= 1) neww[y][x] = 0;      /* die of loneliness */
        else if (c >=4) neww[y][x] = 0;  /* die of overpopulation */
        else if (c == 3)  neww[y][x] = 1;             /* becomes alive */ 
	else neww[y][x] = w[y][x];   /* c == 2, no change */
      }
    }
      /* Logic to exchange border rows */

      struct Data dta;
      if(n_procs >1) {
      if(i == 0)
      {
         //manage bottom row
         CopyColumn(&dta,i_last - 1,true);
         sz = write(owrite,&dta,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         sz = read(iread,&dta,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         CopyColumn(&dta,i_last,false);
      }
      else if(i == n_procs - 1)
      {
         //manage top row
         CopyColumn(&dta,1,true);
         sz = write(iwrite,&dta,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         sz =read(oread,&dta,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         CopyColumn(&dta,0,false);
      }
      else
      {
         //manage both bottom and top
         CopyColumn(&dta,1,true);
         sz = write(iwrite,&dta,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         CopyColumn(&dta,i_last - 1 ,true);
         sz = write(owrite,&dta,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         sz = read(iread,&dta,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         CopyColumn(&dta,i_last,false);
         sz = read(oread,&dta,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         CopyColumn(&dta,0,false);
      }
      }
      /* Logic to exchange border rows */

    /* copy the world, and count the current lives */
    count = 0;

     for (x= 1; x<= Processes[i].range ; x++) {
        for (y = 0; y< w_Y; y++) {
	w[y][x] = neww[y][x];
	if (w[y][x] == 1) count++;
      }
    }
   /* if (DEBUG_LEVEL > 10) print_world(); */
  
    
      /* Logic to exchange local count and global count*/
      if(n_procs > 1){
      if(i == 0)
      {
         // first  owrite(local) -> iread(global) - > signal all
         lc.myrank = 0;
         lc.type = 2;
         lc.local_count = count;
         sz = write(owrite,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         sz = read(iread,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
      }
      else if(i == n_procs -1)
      {
           // last  oread(local) -> iwrite
         sz = read(oread,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         lc.local_count += count;
         lc.global_count = lc.local_count;
         lc.type = 3;
         sz = write(iwrite,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
      }
      else
      {
             // intermediate oread(local) -> owrite(+mylocal) -> ireada
         sz = read(oread,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         lc.local_count += count;
         sz = write(owrite,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         sz = read(iread,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
         sz = write(iwrite,&lc,sizeof(struct Data));
	 if(sz == -1)perror("read write error");
      }
      count = lc.local_count;
      }
      /* Logic to exchange local count and global count */
    if(i == 0)
    printf("iter = %d, population count = %d\n", iter, count);   
   /* if (DEBUG_LEVEL > 10) print_world(); */
  }
      if(n_procs > 1){
      int moveon = 1;
      if(i == 0 )
      {
         PrintFinalWorld(Processes[i]);
         sz = write(owrite,&moveon,sizeof(int));
	 if(sz == -1)perror("read write error");
      }
      else if(i == n_procs - 1)
      {
         sz = read(oread,&moveon,sizeof(int));
	 if(sz == -1)perror("read write error");
         PrintFinalWorld(Processes[i]);
      }
      else
      {
         sz = read(oread,&moveon,sizeof(int));
	 if(sz == -1)perror("read write error");
         PrintFinalWorld(Processes[i]);
         sz = write(owrite,&moveon,sizeof(int));
	 if(sz == -1)perror("read write error");
      }
    }
    else  PrintFinalWorld(Processes[i]);
    exit(0);
    }
    else if(childs[i] > 0)
    {

    }
    else if(childs[i] == -1)
    {
      
    }
  }
  printf("closing pipes in parent\n");
  for(int j = 0 ;j <n_procs -1 ; j++)
    for(int k = 0 ; k < 2; k++)
    {
      close(opipes[j][k]);close(ipipes[j][k]);
    }
  for(int i = 0 ; i < n_procs ; i++)
  {
    int stat ;
    int pid =  wait(&stat);
    printf("child %d returns status %d\n\n",pid,stat);
  }


  return 0;
}
