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
struct rapl_state_s rs;
char filetag[2048];
char sampleFile[2048];
int retVal = -1; 


/* Declare argc and argv as extern ?? */
//extern int argc;
//extern const char *argv;


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

	//Sample =1 
	if(sample == 1) {
		pid_t pid;
		pid = fork(); 
		int status;

		if(pid == -1){
			printf("\n Fork failed. Exiting.");
			exit(EXIT_FAILURE);
		}

		if(pid == 0){
			//Restart the current process with sample set to 0 -- this now becomes the child;
			     printf("\n Forked successfully. I am the child.");
                             printf("\narg 1 is %x", {{get_arg 1}});
                             printf("\ncontents of arg 1 is %x",* {{get_arg 1}});
                             printf("\ncontents contents of arg 1 is %s",** {{get_arg 1}});

                             printf("\n optind = %d", optind);

			     setenv("SAMPLE", "0", 1); 
   		   	     retVal = get_env_int("SAMPLE", &sample);
			     printf("\n SAMPLE is now %d", sample); 
                           
	   		     status = execvp(*({{get_arg 1}})[0], *({{get_arg 1}})+optind );

                             if(status){
                                perror("execvp failed");
                                return -1;
				}
		}
	
		else{
			printf("\n Here to sample...");
                        sprintf(sampleFile, "sample_%s_%d", hostname, rank);
			printf("\n Calling Async");
                        sampleAsync(&rs, sampleFile, 1000); //1000ms sampling rate


                          // wait for program to quit
                      do{
                        pid = wait(&status);
                      } while(pid == -1 && errno == EINTR);
		
			printf("\n We're done here...");
	
		
		}

	}

	else {
		//Sample is zero. Execute normally. 
	        if(rank%msr_rank_mod == 0){
                gethostname(hostname, 1024 );
                //TP
                sprintf(filetag, "%s_rapl_%d", hostname, rank); 
                //TP
                rapl_init(&rs, filetag);
		}
	}

{{endfn}}

{{fn foo MPI_Finalize}}
	double elapsed;
	if(rank%msr_rank_mod == 0){
		rapl_finalize(&rs, 1);
	}
	{{callfn}}
{{endfn}}
