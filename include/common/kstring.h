#ifndef KSTRING
#define KSTRING
#include <types.h>
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
//一些与字符串操作有关的函数。
int strcopy(char* dest,char* src,int n);
int strcomp(const char* first,const char* second);
int sprintk(char* buf,const char* format,...);
int strmid(char* buffer,int buflen,char* last_position,char delim);
U64 shrink_lstring_to_string(U16* lstring,U64 len,char* string);
U64 expand_string_to_lstring(char* string,U64 len,U16* lstring);
int strlen(const char* in);
#endif
