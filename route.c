#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "route.h"//struct has 3 types

int add_route(char destination[DESTINATION_LENGTH], int *num_of_routes, struct route routes[])
{
	if (*num_of_routes == MAX_NUMBER_OF_ROUTES)
	{
		printf("[ERROR] No more routes allowed.\n");
		return 1;
	}
	if (strlen(destination) == 0)
	{
		printf("[ERROR] Destination is empty.\n");
		return 1;
	}
	if (is_route_available(destination, *num_of_routes, routes))
	{
		printf("[ERROR] This destination is already available.\n");
		return 1;
	}

	strcpy(routes[*num_of_routes].destination, destination);
	routes[*num_of_routes].num_of_passengers = 0;
	++*num_of_routes;

	return 0;
}

int is_route_available(char destination[DESTINATION_LENGTH], int num_of_routes, struct route routes[])
{
	int i;
	for (i = 0; i < num_of_routes; i++)
		if (strcmp(routes[i].destination, destination) == 0)
			return 1;

	return 0;
}

int remove_route(char destination[DESTINATION_LENGTH], int *num_of_routes, struct route routes[])
{
	if (*num_of_routes == 1)
	{
		printf("[ERROR] You should have at least one route.\n");
		return 1;
	}

	if (is_route_available(destination, *num_of_routes, routes) == 0)
	{
		printf("[ERROR] No route available for this destination.\n");
		return 1;
	}

	int i;
	for (i = 0; i < *num_of_routes; ++i)
		if (strcmp(routes[i].destination, destination) == 0)
		{
			if(i == *num_of_routes - 1)
				break;

			int j;
			for (j = i; j < *num_of_routes - 1; ++j)
				routes[j] = routes[j + 1];
		}

	--*num_of_routes;

	return 0;
}

struct route* get_route(char destination[DESTINATION_LENGTH], int num_of_routes, struct route routes[])
{
	int i;
	for (i = 0; i < num_of_routes; i++)
		if (strcmp(routes[i].destination, destination) == 0)
			return &routes[i];

	return NULL;
}

int add_passenger(char name[PASSENGER_NAME_LENGTH], char phone[PASSENGER_PHONE_LENGTH], struct route * route)
{
	if (route->num_of_passengers == MAX_NUMBER_OF_PASSENGERS)
	{
		printf("[ERROR] No more passengers allowed.\n");
		return 1;
	}
	if (is_passenger_available(name, *route))
	{
		printf("[ERROR] This passenger is already exists.\n");
		return 1;
	}

	strcpy(route->passengers[route->num_of_passengers].name, name);
	strcpy(route->passengers[route->num_of_passengers].phone, phone);
	time(&(route->passengers[route->num_of_passengers].mod_time));
	++(route->num_of_passengers);

	return 0;
}

int is_passenger_available(char name[PASSENGER_NAME_LENGTH], struct route route)
{
	int i;
	for(i = 0; i < route.num_of_passengers; ++i)
		if (strcmp(route.passengers[i].name, name) == 0)
			return 1;

	return 0;
}

int remove_passenger(char destination[DESTINATION_LENGTH], char name[PASSENGER_NAME_LENGTH], int num_of_routes,
	struct route routes[])
{
	int i;
	for (i = 0; i < num_of_routes; ++i)
		if (strcmp(routes[i].destination, destination) == 0)
		{
			int j;
			for(j = 0; j < routes[i].num_of_passengers; ++j)
				if (strcmp(routes[i].passengers[j].name, name) == 0)
				{
					if (j == routes[i].num_of_passengers)
					{
						--routes[i].num_of_passengers;//deleted passenger
						return 0;
					}

					int k;
					for (k = j; k < routes[i].num_of_passengers - 1; ++k)
						routes[i].passengers[k] = routes[i].passengers[k + 1];

					--routes[i].num_of_passengers;

					return 0;
				}

			printf("[ERROR] No passenger available with this name for this destination.\n");
			return 1;
		}


	printf("[ERROR] No route available for this destination.\n");
	return 1;
}

int modify_passenger(char destination[DESTINATION_LENGTH], char name[PASSENGER_NAME_LENGTH],
	char newDestination[DESTINATION_LENGTH], char newName[PASSENGER_NAME_LENGTH],
	char newPhone[PASSENGER_PHONE_LENGTH], int num_of_routes, struct route routes[])
{
	if (strcmp(newDestination, "=") == 0 && strcmp(newName, "=") == 0 && strcmp(newPhone, "=") == 0)
	{
		printf("[ERROR] Nothing to modify.\n");
		return 1;
	}

	int i;
	for (i = 0; i < num_of_routes; ++i)
	{
		if (strcmp(routes[i].destination, destination) == 0)
		{
			int j;
			for(j = 0; j < routes[i].num_of_passengers; ++j)
				if (strcmp(routes[i].passengers[j].name, name) == 0)
				{
					if (strcmp(newDestination, "=") == 0)
					{
						newDestination = malloc(16);
						strcpy(newDestination, routes[i].destination);
					}

					if (strcmp(newName, "=") == 0)
					{
						newName = malloc(16);
						strcpy(newName, routes[i].passengers[j].name);
					}

					if (strcmp(newPhone, "=") == 0)
					{
						newPhone = malloc(16);
						strcpy(newPhone, routes[i].passengers[j].phone);
					}

					remove_passenger(destination, name, num_of_routes, routes);

					int k;
					for (k = 0; k < num_of_routes; ++k)
						if (strcmp(routes[k].destination, newDestination) == 0)
						{
							add_passenger(newName, newPhone, &(routes[k]));
							return 0;
						}

					printf("[ERROR] No route available for this destination.\n");
					return 1;
				}

			printf("[ERROR] No passenger available with this name for this destination.\n");
			return 1;
		}
	}

	printf("[ERROR] No route available for this destination.\n");
	return 1;
}
