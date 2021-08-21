#ifndef FEEDBACK_H_INCLUDED//if header file not there in other file itll be included(only check if all are there,doesnt check contents)
#define FEEDBACK_H_INCLUDED//contents of file

#include "configuration.h"
#include "passenger.h"

struct feedback
{
	struct passenger passengers[MAX_NUMBER_OF_PASSENGERS];
	int num_of_passengers;
	int values[MAX_NUMBER_OF_PASSENGERS];
};

#endif
