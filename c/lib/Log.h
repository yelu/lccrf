#pragma once

#include <cstdlib>
#include <cstdio>
#include <stdarg.h>
#include <ctime>

#define LOG(_fmt_, ...) \
    do\
    {\
		time_t rawtime; \
		struct tm * timeinfo; \
		char buffer [80]; \
		time (&rawtime); \
		timeinfo = localtime (&rawtime); \
		strftime (buffer, 80, "%y/%m/%d %H:%M:%S", timeinfo); \
		fprintf(stderr, "[%s][%s][%d][%s]"#_fmt_"\n", buffer, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
    }while(false)

#define LOG_DEBUG(_fmt_, ...) \
    do\
    {\
		time_t rawtime; \
		struct tm * timeinfo; \
		char buffer [80]; \
		time (&rawtime); \
		timeinfo = localtime (&rawtime); \
		strftime (buffer, 80, "%y/%m/%d %H:%M:%S", timeinfo); \
		fprintf(stderr, "[%s][%s][%d][%s]"##_fmt_"\n", buffer, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);\
    }while(false)

#ifdef NDEBUG
#undef LOG_DEBUG
#define LOG_DEBUG(_fmt_, ...) do{}while(false)
#endif
