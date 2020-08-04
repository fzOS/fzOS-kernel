#ifndef VERSION_H
#define VERSION_H
#include <types.h>
#define GIT_VERSION "git-d4301ff"
#define NUMBER_VERSION "0.1.3"

#ifdef FZOS_DEBUG_SWITCH
#define VERSION GIT_VERSION
#else
#define VERSION NUMBER_VERSION
#endif
//在取版本号时输出信息。
#ifdef GET_VERSION_DIRECTLY
#pragma message "version: "VERSION" "
#endif
#endif