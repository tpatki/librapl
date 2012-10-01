#include <stdio.h>
#include <mpi.h>

#define LONG_RANGE 3600000000
//#define LONG_RANGE 50000

main(int argc, char **argv)
{
  int rank;
  unsigned long i=0;

   
   MPI_Init(&argc,&argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
     
   printf("Hello from Node %d\n",rank);

  #pragma omp parallel for 
   for(i=0;i<LONG_RANGE; i++); //do nothing but spin

   printf("Goodbye from Node %d\n",rank);
            
   MPI_Finalize();
}

