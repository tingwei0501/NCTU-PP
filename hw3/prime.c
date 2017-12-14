#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int isprime(long long int n) {
  long long int i,squareroot;
  if (n>10) {
    squareroot = (long long int) sqrt(n);
    for (i=3; i<=squareroot; i=i+2)
      if ((n%i)==0)
        return 0;
    return 1;
  }
  else
    return 0;
}

int main(int argc, char *argv[])
{
  long long int pc,       /* prime counter */
      foundone, /* most recent prime found */
      pc_sum, /* prime sum counted in host */
      prime_max;
  long long int n, limit, times;
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  pc = 0;
  foundone = 0;

  if (rank==0) {
    sscanf(argv[1],"%llu",&limit);
    printf("Starting. Numbers to be scanned= %lld\n",limit);
    pc = 4;
    times = limit/size;
  }
  MPI_Bcast(&times, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&limit, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

  long long int start = times * rank;
	long long int end = start + times;
  /*
  17/4 = 4   13/4 = 3  14/4 = 3
  0 1 4      0 3       0 3
  4 5 8      3 6       3 6
  8 9 12     6 9
  12 13 16   9 12
  */
  if (start % 2 == 0) start++;
  if (rank == size-1) end = limit + 1;
  for (;start<end;start+=2) {
    if (isprime(start)) {
      pc++;
      foundone = start;
    }
  }

  MPI_Reduce(&pc, &pc_sum, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Reduce(&foundone, &prime_max, 1, MPI_LONG_LONG_INT, MPI_MAX, 0, MPI_COMM_WORLD);
  if(rank == 0)
		printf("Done. Largest prime is %lld Total primes %lld\n", prime_max, pc_sum);
  
  MPI_Finalize();
  
  return 0;
} 
