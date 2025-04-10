#ifndef __SYSTEM__
#define __SYSTEM__

#include <windows.h>

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_SYSTEM 2

int sLogLevel;
WCHAR LogBuff[1024];

#define _LOG(LogLevel,fmt,...) \
do\
{\
if(sLogLevel<=LogLevel)\
{\
wsprintf(LogBuff, fmt, ##__VA_ARGS__);\
Log(LogBuff, LogLevel);\
}\
}while(0)\


void Log(WCHAR* string, int LogLevel);


#endif
