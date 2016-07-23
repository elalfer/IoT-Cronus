#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <syslog.h>
#include <iostream>

#include <time.h>
#include <string.h>

extern int g_DebugLevel;

#define DEBUG_PRINT(log,txt) if(log <= g_DebugLevel) std::cout << log << ": " << txt << " (" << __PRETTY_FUNCTION__ << ")" <<  std::endl;

static char *format_time_t(time_t t)
{
	char *b;
	b = ctime(&t);
	b[strlen(b)-1]='\0';

	return b;
}

static char *format_timespan_t(time_t t1, time_t t2)
{
	static char ts[256];
	sprintf(ts, "%s", format_time_t(t1));
	sprintf(ts, "%s - %s", ts, format_time_t(t2));
	return ts;
}

#endif