#include <mpi.h>
#include <unistd.h>
#include <stdio.h>

int
main(int argc, char *argv[]){
 MPI_Init(&argc, &argv);
 printf("\n Going to sleep now...");
 sleep(10);
 printf("\n Out of sleep...");
 MPI_Finalize();
 return 0;
}
