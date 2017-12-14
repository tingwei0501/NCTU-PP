/**********************************************************************
 * DESCRIPTION:
 *   Wave Equation - cu Version
 *   This program implements the concurrent wave equation
 *********************************************************************/
 #include <stdio.h>
 #include <stdlib.h>
 #include <math.h>
 #include <time.h>
 
 #define MAXPOINTS 1000000
 #define MAXSTEPS 1000000
 #define MINPOINTS 20
 #define PI 3.14159265
 
 void check_param(void);
 void init_line(void);
 void update (void);
 void printfinal (void);
 
 int nsteps,                 	/* number of time steps */
     tpoints, 	     		/* total points along string */
     rcode;                  	/* generic return code */
 float  values[MAXPOINTS+2], 	/* values at time t */
        oldval[MAXPOINTS+2], 	/* values at time (t-dt) */
        newval[MAXPOINTS+2]; 	/* values at time (t+dt) */
 
 
 /**********************************************************************
  *	Checks input values from parameters
  *********************************************************************/
 void check_param(void)
 {
    char tchar[20];
 
    /* check number of points, number of iterations */
    while ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS)) {
       printf("Enter number of points along vibrating string [%d-%d]: "
            ,MINPOINTS, MAXPOINTS);
       scanf("%s", tchar);
       tpoints = atoi(tchar);
       if ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS))
          printf("Invalid. Please enter value between %d and %d\n", 
                  MINPOINTS, MAXPOINTS);
    }
    while ((nsteps < 1) || (nsteps > MAXSTEPS)) {
       printf("Enter number of time steps [1-%d]: ", MAXSTEPS);
       scanf("%s", tchar);
       nsteps = atoi(tchar);
       if ((nsteps < 1) || (nsteps > MAXSTEPS))
          printf("Invalid. Please enter value between 1 and %d\n", MAXSTEPS);
    }
 
    printf("Using points = %d, steps = %d\n", tpoints, nsteps);
 
 }

 /**********************************************************************
  *     Initialize points on line
  *********************************************************************/
 __global__ void init_line(int tpoints, float *GPU_values, float *GPU_oldval)
 {
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    /* Calculate initial values based on sine curve */
     
    /* Initialize old values array */
    GPU_values[id] = sin((2.0 * PI)*((float)id/(float)(tpoints-1)));
    GPU_oldval[id] = sin((2.0 * PI)*((float)id/(float)(tpoints-1)));
 }
 
 /**********************************************************************
  *      Calculate new values using wave equation
  *********************************************************************/
 
 /**********************************************************************
  *     Update all values along line a specified number of times
  *********************************************************************/

 __global__ void update(int tpoints, int nsteps, float *GPU_values, float *GPU_oldval, float *GPU_newval)
 {
    int i;
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    /* Update values for each time step */
    for (i = 1; i<= nsteps; i++) {
       /* Update points along line for this time step */
       if ((id == 0) || (id  == tpoints-1))
            GPU_newval[id] = 0.0;
       else 
            GPU_newval[id] = 1.82 * GPU_values[id] - GPU_oldval[id];
 
       /* Update old values with new values */
       GPU_oldval[id] = GPU_values[id];
       GPU_values[id] = GPU_newval[id];
    }
 }
 
 /**********************************************************************
  *     Print final results
  *********************************************************************/
 void printfinal()
 {
    int i;
 
    for (i = 0; i < tpoints; i++) {
       printf("%6.4f ", values[i]);
       if (i%10 == 9)
          printf("\n");
    }
 }
 
 /**********************************************************************
  *	Main program
  *********************************************************************/
 int main(int argc, char *argv[])
 {
     sscanf(argv[1],"%d",&tpoints);
     sscanf(argv[2],"%d",&nsteps);
     check_param();

     float *GPU_values, *GPU_oldval, *GPU_newval;
     // GPU memory
     cudaMalloc(&GPU_values, sizeof(values));
     cudaMalloc(&GPU_oldval, sizeof(values));
     cudaMalloc(&GPU_newval, sizeof(values));

     printf("Initializing points on the line...\n");
     init_line<<<((tpoints + 1023) >> 10), 1024>>>(tpoints, GPU_values, GPU_oldval);

     printf("Updating all points for all time steps...\n");
     // <<<numBlocks, threadsPerBlock>>>
     update<<<((tpoints + 1023) >> 10), 1024>>>(tpoints, nsteps, GPU_values, GPU_oldval, GPU_newval);
     printf("Printing final results...\n");
     cudaMemcpy(values, GPU_values, sizeof(values), cudaMemcpyDeviceToHost);
     printfinal();
     printf("\nDone.\n\n");
     
     return 0;
 }
 