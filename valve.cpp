#include "valve.h"

#include <stdio.h>

int set_valve_status(unsigned int id, bool active)
{
	printf("INFO: Set valve #%d status to %d\n", id, active);
	return 0;
}
