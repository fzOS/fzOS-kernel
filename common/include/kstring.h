#ifndef KSTRING
#define KSTRING
#include <types.h>
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
//一些与字符串操作有关的函数。
int strcopy(char* dest,char* src,int n);
int strcomp(char* first,char* second);
int memcopy(void* dest,void* src,int n);
int memcomp(void* first,void* second,int n);
int memmove(void* dest,void* src,int n);
U64 memset(void* pointer,U8 value,int n);
#endif
