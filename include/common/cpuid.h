//对CPUID的简单包装。
#ifndef CPUID
#define CPUID
int get_processor_vendor(char* buff);
int get_processor_name(char* buff);
int get_processor_rdseed_support(void);
#endif
