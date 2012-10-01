
mpi-spin:
	mpicc  -fopenmp -o mpi-spin mpi-spin.c -Xlinker -rpath=$(HOME)/local/src/librapl_patki/libmsr/lib -L$(HOME)/local/src/librapl_patki/libmsr/lib -lrapl

clean:
	rm -f mpi-spin *.o

