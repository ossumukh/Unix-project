#ifndef PASSENGER_H_INCLUDED
#define PASSENGER_H_INCLUDED

#include <time.h>

#include "configuration.h"

struct passenger
{
	char name[PASSENGER_NAME_LENGTH];
	char phone[PASSENGER_PHONE_LENGTH];
	time_t mod_time;
};

#endif
