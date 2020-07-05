#ifndef KSTRING
#define KSTRING
#include <types.h>
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
//一些与字符串操作有关的函数。
int strcopy(char* dest,char* src,int n);
int strcomp(char* first,char* second);
int memcopy(U8* desc,U8* src,int n);
int memcomp(U8* first,U8* second,int n);
int memmove(U8* dest,U8* src,int n);
#endif