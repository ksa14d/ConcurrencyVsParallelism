/* 
 * Created 06/20/2015 by Xin Yuan for the COP5570 class
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define MAX_N 8192

#ifndef DEBUG_LEVEL 
#define DEBUG_LEVEL 0
#endif

char w[MAX_N][MAX_N];
char neww[MAX_N][MAX_N];

int w_X, w_Y;

void init1(int X, int Y)
{ 
  int i, j;
  w_X = X,  w_Y = Y;
  for (i=0; i<w_X;i++)
    for (j=0; j<w_Y; j++) 
      w[j][i] = 0;
 
  for (i=0; i<w_X; i++) w[0][i] = 1;
  for (i=0; i<w_Y; i++) w[i][0] = 1;
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

int main(int argc, char *argv[])
{
  int x, y;
  int iter = 0;
  int c;
  int init_count;
  int count;
 
  struct timeval start, end; /* for timing */
  
  
   /* Now begins the real work which we want to parallelize. */
  /* Mark the starting time. */
  if (gettimeofday(&start, NULL) < 0) {
    perror(": gettimeofday");
    exit(0);
  }

  if (argc == 1) {
    printf("Usage: ./a.out w_X w_Y\n");
    exit(0);
  } else if (argc == 2) 
    test_init2();
  else /* more than three parameters */
    init1(atoi(argv[1]), atoi(argv[2]));
     print_world();
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
