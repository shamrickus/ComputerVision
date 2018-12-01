#ifndef _LOGGER_H_
#define _LOGGER_H_

#define _CRT_SECURE_NO_WARNINGS 1

#include "GLFW/glfw3.h"
#include <cstdio>
#include "time.h"
#include <cstdarg>

#define LOG_FILE "final.log"

class CVLogger 
{
public:
	static bool Start() { return Restart(); }
	static bool Restart()
	{
		FILE* file = fopen(LOG_FILE, "w");
		if(!file)
		{
			fprintf(stderr, "ERROR: could not open log file %s\n", LOG_FILE);
			return false;
		}
		time_t now = time(NULL);
		char* date = ctime(&now);
		fprintf(file, "%s. local time %s\n", LOG_FILE, date);
		fclose(file);
		return true;
	}

	static bool Log(const char* message, ...)
	{
		va_list argptr;
		FILE* file = fopen(LOG_FILE, "a");
		if(!file)
		{
			fprintf(stderr, "ERROR: could not open %s\n", LOG_FILE);
			return false;
		}
		va_start(argptr, message);
		vfprintf(file, message, argptr);
		vfprintf(stdout, message, argptr);
		va_end(argptr);
		fclose(file);
		return true;
	}
};

#endif