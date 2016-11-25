// Copyright 2016 Ilya Albrekht
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <syslog.h>
#include <iostream>

#include <time.h>
#include <string.h>

extern int g_DebugLevel;
extern bool g_isDaemon;

#if DEBUG
	#define PRINT_FUNC " {" << __PRETTY_FUNCTION__ << "}"
#else
	#define PRINT_FUNC ""
#endif

#define LOG_PRINT(log, msg) DEBUG_PRINT(log, msg)

#define DEBUG_PRINT(log,txt) if(log <= g_DebugLevel) std::cout << format_time_t(time(0)) << " " << log << ": " << txt << PRINT_FUNC <<  std::endl;

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
