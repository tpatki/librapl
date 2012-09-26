#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include "msr_core.h"
#include "msr_rapl.h"
#include "msr_opt.h"
#include "blr_util.h"
#include "sample.h"
#include <errno.h>

static int rank;
static char hostname[1025];
extern int msr_debug;
static FILE* f;
struct rapl_state_s *rs;
struct rapl_state_s *rs2;
struct rapl_state_s *rs3;
char filetag[2048];
char sampleFile[2048];
char barrierFile[2048];
int retVal = -1; 

static struct rapl_state_s temp;

static int msr_rank_mod=-1;
static int sample = 0; 


{{fn foo MPI_Init}}
	{{callfn}}
	rank = -1;
	PMPI_Comm_rank( MPI_COMM_WORLD, &rank );
	retVal = get_env_int("MSR_RANK_MOD", &msr_rank_mod);


	if(retVal == -1){
		printf("Error: To run an MPI program, the MSR_RANK_MOD environment variable should be set.\n"); 
		exit(EXIT_FAILURE);
	}

	retVal = get_env_int("SAMPLE", &sample);

	if(retVal == -1){
		printf("Default: SAMPLE is set to zero. If you want to profile the program, set the SAMPLE environment variable to one.\n"); 
	}


	if(rank%msr_rank_mod == 0){
		gethostname(hostname, 1024 );
		//TP
		sprintf(filetag, "%s_rapl_%d", hostname, rank); 
		//TP
	//	register_sig();
		//TP
		rs2 = rapl_init(filetag); 
		//Initialization to record the first tick
		rs3 = &temp;
		sprintf(barrierFile, "barrier_%s_%d", hostname, rank);
		rapl_tick(rs3, barrierFile);	


		if(sample == 1) {
			printf("\nSAMPLE=1\n");
			sprintf(sampleFile, "sample_%s_%d", hostname, rank);
			sampleAsync(sampleFile, 1000); //1000ms sampling rate

		}
	}
{{endfn}}

{{fn foo MPI_Barrier}}
{{callfn}}
if(rank % msr_rank_mod ==0){
	rapl_tick(rs3, barrierFile);
}
{{endfn}}


{{fn foo MPI_Finalize}}
	double elapsed;
	if(rank%msr_rank_mod == 0){
		rapl_finalize(rs2,0);
	}
	{{callfn}}
{{endfn}}
