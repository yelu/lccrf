#pragma once

#include <cstdlib>
#include <cstdio>
#include <stdarg.h>

#define LOG(_fmt_, ...) \
    do\
    {\
	fprintf(stderr, "[%s][%d][%s]"##_fmt_"\n",  __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);\
    }while(false)

#define LOG_DEBUG(_fmt_, ...) \
    do\
    {\
	fprintf(stderr, "[%s][%d][%s]"##_fmt_"\n",  __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);\
    }while(false)

#ifdef NOLOG
#undef LOG
#undef LOG_DEBUG
#define LOG(_fmt_, args...) do{}while(false)
#define LOG_DEBUG(_fmt_, args...) do{}while(false)
#endif