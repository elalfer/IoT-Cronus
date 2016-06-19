#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <syslog.h>
#include <iostream>

extern int g_DebugLevel;

#define DEBUG_PRINT(log,txt) if(log <= g_DebugLevel) std::cout << log << ": " << txt << " (" << __PRETTY_FUNCTION__ << ")" <<  std::endl;

#endif