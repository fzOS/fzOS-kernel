#ifndef KSTRING
#define KSTRING
#include <types.h>
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
//一些与字符串操作有关的函数。
int strcopy(char* dest,char* src,int n);
int strcomp(char* first,char* second);
int sprintk(char* buf,const char* format,...);
#endif
