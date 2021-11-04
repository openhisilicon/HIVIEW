#ifndef __sstat_h__
#define __sstat_h__

// sstat_cpu_occupy 
// param: c[sleep c]
float sstat_cpu_occupy(int c);

// sstat_ddr_occupy 
// param: c[sleep c]
// depend: btools hiddrs
float sstat_ddr_occupy(int c);

// sstat_cpu_temp 
// param: c[sleep c]
// depend: btools tsensor
float sstat_cpu_temp(int c);

// sstat_mem_occupy
float sstat_mem_occupy();

// sstat_chipid
// depend: btools chipid
char* sstat_chipid();

#endif //__sstat_h__