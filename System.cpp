#include <windows.h>
#include <iostream>

int sLogLevel;
WCHAR LogBuff[1024];

void Log(WCHAR* string, int LogLevel)
{
	wprintf(L"%s \n",string);
}
