#ifndef SAMPLE_H
#define SAMPLE_H

#include <stdint.h>

double tsc_rate;
uint64_t lastNonzeroTick;

void sampleAsync(const char * const filename, unsigned int msPeriod);
void msSample(const char * const filename, int log);

#endif
