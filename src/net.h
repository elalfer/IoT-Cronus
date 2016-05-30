#ifndef NET_H
#define NET_H

#include <string>
#include <time.h>

// Network error codes
#define NET_SUCCESS 		0
#define NET_NOT_CHANGED 	1
#define NET_ERROR 			-1

// Load ical from URL
int load_ical_from_url(std::string &ical, const std::string &host, const std::string &URL, time_t mod_since);

#endif