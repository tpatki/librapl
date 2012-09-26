/* msr_clocks.c
 *
 * MPERF, APERF and friends.
 */

#include <stdio.h>
#include <unistd.h>
#include "msr_core.h"
#include "msr_clocks.h"
#include "msr_common.h"
#include "blr_util.h"

#define MSR_IA32_MPERF 		0x000000e7
#define MSR_IA32_APERF 		0x000000e8
#define IA32_TIME_STAMP_COUNTER 0x00000010

double measure_tsc(){
  struct timeval t1, t2;
  gettimeofday(&t1, 0);
  uint64_t tsc1 = rdtsc();
  sleep(1);
  gettimeofday(&t2, 0);
  uint64_t tsc2 = rdtsc();
  return (tsc1 < tsc2 ? (tsc2 - tsc1) : (tsc1 - tsc2)) / ts_delta(&t1, &t2);
}

void 
read_aperf_mperf(int package, int core, uint64_t *aperf, uint64_t *mperf, uint64_t *tsc){
  read_msr_single_core(package, core, MSR_IA32_APERF, aperf );
  read_msr_single_core(package, core, MSR_IA32_MPERF, mperf );
  read_msr_single_core(package, core, IA32_TIME_STAMP_COUNTER, tsc);
}

void 
dump_clocks(){
  int package, core;
  uint64_t aperf[NUM_PACKAGES][NUM_CORES_PER_PACKAGE], 
    mperf[NUM_PACKAGES][NUM_CORES_PER_PACKAGE], 
    tsc[NUM_PACKAGES][NUM_CORES_PER_PACKAGE];


  //! @todo use cpuid.c to number cores; numbering may not be consecutive
  for( package=0; package<NUM_PACKAGES; package++)
    for(core = 0; core < NUM_CORES_PER_PACKAGE; core++)
      read_aperf_mperf(package, core, 
		       &aperf[package][core], 
		       &mperf[package][core], 
		       &tsc[package][core]);

  fprintf(stdout, "MSR_IA32_APERF\tMSR_IA32_MPERF\tTSC\n");
  for( package=0; package<NUM_PACKAGES; package++)
    for(core = 0; core < NUM_CORES_PER_PACKAGE; core++)
      fprintf(stdout, "%20lu\t%20lu\t%20lu\n", 
	      aperf[package][core],
	      mperf[package][core],
	      tsc[package][core]);

}

//! @todo this will not work; aperf/mperf are per-core.
/*
void
get_effective_frequency(struct rapl_state_s *s, int core, double *ratio, double *c0){
	static int init=0;
	uint64_t mperf, aperf, tsc;
	read_aperf_mperf(core, &aperf, &mperf, &tsc);
	if(init && (mperf - s->previous_mperf[core])){
	  double delta_mperf;

	  if(ratio){
	    double delta_aperf;
	    // aperf and mperf are actual 64-bit counters, but will not overflow in my lifetime...
	    delta_aperf = aperf - s->previous_aperf[core];
	    delta_mperf = mperf - s->previous_mperf[core];
	    
	    *ratio = delta_aperf / delta_mperf;
	  }

	  if(c0){
	    double delta_tsc;
	    delta_tsc = tsc - s->previous_tsc[core];
	    *c0 = delta_mperf / delta_tsc;
	  }
	}
	s->previous_mperf[core] = mperf;
	s->previous_aperf[core] = aperf;
	s->previous_tsc[core] = tsc;
	init=1;
}
*/

void
get_effective_frequencies(int package, struct rapl_state_s *s){
  static int init=0;
  int core;
  uint64_t mperf[NUM_CORES_PER_PACKAGE], 
    aperf[NUM_CORES_PER_PACKAGE], 
    tsc[NUM_CORES_PER_PACKAGE];
  double delta_mperf;

  for(core = 0; core < NUM_CORES_PER_PACKAGE; core++)
    //! @todo fix core indexing
    read_aperf_mperf(package, core, aperf+core, mperf+core, tsc+core);

  if(init){
    double delta_aperf;
    double delta_tsc;

    for(core = 0; core < NUM_CORES_PER_PACKAGE; core++){
      // aperf and mperf are actual 64-bit counters, but will not overflow in my lifetime...
      delta_aperf = 
	aperf[core] - s->previous_aperf[NUM_CORES_PER_PACKAGE * package + core];
      delta_mperf = 
	mperf[core] - s->previous_mperf[NUM_CORES_PER_PACKAGE * package + core];
      s->effective_freq_ratio[NUM_CORES_PER_PACKAGE * package + core] = 
	delta_aperf / delta_mperf;
	    
      delta_tsc = 
	tsc[core] - s->previous_tsc[NUM_CORES_PER_PACKAGE * package + core];
      s->c0_ratio[NUM_CORES_PER_PACKAGE * package + core] = 
	delta_mperf / delta_tsc;
      s->previous_mperf[NUM_CORES_PER_PACKAGE * package + core] = mperf[core];
      s->previous_aperf[NUM_CORES_PER_PACKAGE * package + core] = aperf[core];
      s->previous_tsc[NUM_CORES_PER_PACKAGE * package + core] = tsc[core];
    }
  }
  init=1;
}
