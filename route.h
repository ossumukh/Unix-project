#ifndef ROUTE_H_INCLUDED
#define ROUTE_H_INCLUDED

#include "configuration.h"
#include "passenger.h"

struct route {
	char destination[DESTINATION_LENGTH];
	int num_of_passengers;
	struct passenger passengers[MAX_NUMBER_OF_PASSENGERS];
};

int add_route(char destination[DESTINATION_LENGTH], int *num_of_routes, struct route routes[]);
int is_route_available(char destination[DESTINATION_LENGTH], int num_of_routes, struct route routes[]);
int remove_route(char destination[DESTINATION_LENGTH], int *num_of_routes, struct route routes[]);
struct route* get_route(char destination[DESTINATION_LENGTH], int num_of_routes, struct route routes[]);//array

int add_passenger(char name[PASSENGER_NAME_LENGTH], char phone[PASSENGER_PHONE_LENGTH], struct route * route);
int is_passenger_available(char name[PASSENGER_NAME_LENGTH], struct route route);
int remove_passenger(char destination[DESTINATION_LENGTH], char name[PASSENGER_NAME_LENGTH], int num_of_routes,
	struct route routes[]);
int modify_passenger(char destination[DESTINATION_LENGTH], char name[PASSENGER_NAME_LENGTH],
	char newDestination[DESTINATION_LENGTH], char newName[PASSENGER_NAME_LENGTH],
	char newPhone[PASSENGER_PHONE_LENGTH], int num_of_routes, struct route routes[]);

#endif
