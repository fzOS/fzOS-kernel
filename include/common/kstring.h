#ifndef KSTRING
#define KSTRING
#include <types.h>
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
//一些与字符串操作有关的函数。
int strcopy(char* dest,char* src,int n);
int strcomp(const char* first,const char* second);
int sprintk(char* buf,const char* format,...);
int strmid(char* buffer,int buflen,char* last_position,char delim);
#endif
