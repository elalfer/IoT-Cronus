#ifndef CAL_H
#define CAL_H


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <libical/ical.h>

#include "libical/icalproperty_cxx.h"
#include "libical/vcomponent_cxx.h"
#include "libical/icalrecur.h"

using namespace std;
using namespace LibICal;

#define MAX_DAYS_IN_FUTURE 14

// Store events in weeks - reuse the data if needed and system is offline

enum grass_type_t {
	GT_NONE,
	GT_DEFUALT,
	GT_GRASS,
	GT_BUSHES,
	GT_FLOWERS,
	GT_TREES
};

class IValveControl
{
public:
	virtual bool IsActive() = 0;
};

class iCalValveControl: IValveControl
{
public:
	iCalValveControl(grass_type_t grass_type): gt(grass_type), last_updated(0) {};

	// Parse calendar information from the string
	int ParseICALFromString(const string &ical);
	// Load and parse calendar information from file
	int ParseICALFromFile(string file_name);

	// Returns true if the vavle should be open right now
	bool IsActive();

private:
	void add_to_list(icaltimetype start, icaltimetype end);
	void sort_events();

	time_t	last_updated;

	grass_type_t gt;
	vector<icaltime_span> events;
};

#endif
