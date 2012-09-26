#ifndef MSR_CLOCKS_H
#define MSR_CLOCKS_H

#include "msr_rapl.h"

double measure_tsc();

/*
void
get_effective_frequency(struct rapl_state_s *s, int core, double *ratio, double *c0);
*/

void
get_effective_frequencies(int package, struct rapl_state_s *s);

void dump_clocks();

#endif //MSR_CLOCKS_H
