#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define PI 3.1415926535

int main(int argc, char **argv) 
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    long long i, num_intervals, times;
    double rect_width, area, sum, x_middle, total_sum; 

    if (rank==0) {
        sscanf(argv[1],"%llu",&num_intervals);
        times = num_intervals/size;
    }
    MPI_Bcast(&times, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);    
    MPI_Bcast(&num_intervals, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    rect_width = PI / num_intervals;

    long long int start = times * rank;
	long long int end = start + times;

    sum = 0;
    if (rank==0) start++;
    if (rank==size-1) end = num_intervals + 1;
    for(; start<end; start++) {
        /* find the middle of the interval on the X-axis. */ 
        x_middle = (start - 0.5) * rect_width;
        area = sin(x_middle) * rect_width; 
        sum += area;
    } 

    MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank==0) printf("The total area is: %f\n", (float)total_sum);

    MPI_Finalize();
    return 0;
}   