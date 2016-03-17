#ifndef NET_H
#define NET_H

#include <string>

using namespace std;

// Network error codes
#define NET_SUCCESS 		0
#define NET_NOT_CHANGED 	1
#define NET_ERROR 			-1

// Load ical from URL
int load_ical_from_url(string &ical, const string &host, const string &URL);

#endif