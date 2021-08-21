#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include "configuration.h"//import header files user defined
#include "feedback.h"
#include "passenger.h"
#include "route.h"

enum cmd //assign names to integral constants, the names make a program easy to read and maintain.
{
	HELP,
	LIST_ROUTES,
	ADD_ROUTE,
	REMOVE_ROUTE,
	ADD_PASSENGER,
	REMOVE_PASSENGER,
	MODIFY_PASSENGER,
	TAKE_ROUTE
};

struct task
{
	enum cmd cmd;
	int num_of_args;
	char ** args;
};

void init_state();
void save_db();
struct task * parse_args(int argc, char ** argv);
void do_task(struct task * task);
void task_help();
void task_list_routes();
void task_add_route(struct task * task);
void task_remove_route(struct task * task);
void task_add_passenger(struct task * task);
void task_remove_passenger(struct task * task);
void task_modify_passenger(struct task * task);
void task_take_route(struct task * task);
void manage_route(pid_t pid, int pipefd[2], struct route *route);
void guide_route(int pipefd[2], struct route *route);

struct route routes[MAX_NUMBER_OF_ROUTES];
int num_of_routes = 0;//holds num of routes
FILE * db;

int main(int argc, char ** argv) // first argument is the number of command line arguments and second is list of command-line arguments.
{
	struct task * task = parse_args(argc, argv); //line 103(create instance of struct)
	init_state();
	do_task(task);
	save_db();
	return 0;
}

void init_state()//initialise or create at first time
{
	if (access(DB_FILE_NAME, W_OK) == 0)//if file is accessible returns 0
	{
		db = fopen(DB_FILE_NAME, "rb"); //rb opens text in binary mode
		if (db == NULL)
		{
			printf("[ERROR] Unable to open the database.\n");
			exit(1);
		}

		num_of_routes = fread(routes, sizeof(struct route), MAX_NUMBER_OF_ROUTES, db);
		if (routes == NULL || num_of_routes == 0) {
			printf("[ERROR] Unable to read the database.\n");
			exit(1);
		}
	}
	else
	{
		printf("%s is not available, it will be created with the default values...\n", DB_FILE_NAME);
		db = fopen(DB_FILE_NAME, "wb+");//Truncate to zero length or create file for update.(wb+)
		if (db == NULL)
		{
			printf("[ERROR] Unable to create the database.\n");
			exit(1);
		}
		printf("%s sucessfully created.\n", DB_FILE_NAME);

		add_route("Mysore", &num_of_routes, routes);
		add_route("Dharwad", &num_of_routes, routes);
		add_route("Hubli", &num_of_routes, routes);
	}
}

void save_db()
{
	db = freopen(NULL, "wb", db);
	if (fwrite(routes, sizeof(struct route), num_of_routes, db) != num_of_routes
			|| fclose(db) != 0)
		printf("[ERROR] Failed to save the database.\n");
}

struct task * parse_args(int argc, char ** argv)
{
	struct task * task = malloc(sizeof(struct task *));

	if (argc < 2)
	{
		task->cmd = HELP;
		return task;//ends the prgrm
	}

	task->num_of_args = argc;
	if (task->num_of_args > 2)
	{
		task->args = argv;
	}

	char * raw_cmd = argv[1]; //redirect to that command

	if (strcmp(raw_cmd, "help") == 0)
	{
		task->cmd = HELP;
	}
	else if (strcmp(raw_cmd, "list-routes") == 0)
	{
		task->cmd = LIST_ROUTES;
	}
	else if (strcmp(raw_cmd, "add-route") == 0)
	{
		task->cmd = ADD_ROUTE;
	}
	else if (strcmp(raw_cmd, "remove-route") == 0)
	{
		task->cmd = REMOVE_ROUTE;
	}
	else if (strcmp(raw_cmd, "add-passenger") == 0)
	{
		task->cmd = ADD_PASSENGER;
	}
	else if (strcmp(raw_cmd, "remove-passenger") == 0)
	{
		task->cmd = REMOVE_PASSENGER;
	}
	else if (strcmp(raw_cmd, "modify-passenger") == 0)
	{
		task->cmd = MODIFY_PASSENGER;
	}
	else if (strcmp(raw_cmd, "take-route") == 0)
	{
		task->cmd = TAKE_ROUTE;
	}
	else
	{
		printf("Invalid command: %s\n", raw_cmd);//not there in list
		task->cmd = HELP;
	}

	return task;//exit
}

void do_task(struct task * task)
{
	switch(task->cmd)
	{
		case HELP:
			task_help();
			break;
		case LIST_ROUTES:
			task_list_routes();
			break;
		case ADD_ROUTE:
			task_add_route(task);
			break;
		case REMOVE_ROUTE:
			task_remove_route(task);
			break;
		case ADD_PASSENGER:
			task_add_passenger(task);
			break;
		case REMOVE_PASSENGER:
			task_remove_passenger(task);
			break;
		case MODIFY_PASSENGER:
			task_modify_passenger(task);
			break;
			break;
		case TAKE_ROUTE:
			task_take_route(task);
			break;
	}
}

void task_help()//prints all data to user
{
	puts("NAME\n\tTaxi Simulation\n\
SYNTAX\n\tuber <command> [<args>]\n\
COMMANDS\n\
\thelp - This help.\n\
\tlist-routes - List of the available routes.\n\
\tadd-route - Add a route. \n\t\tARGS:\n\
\t\t1. Destination of the new route.\n\
\tremove-route - Remove a route. \n\t\tARGS:\n\
\t\t1. Destination of the route to remove.\n\
\tadd-passenger - Add a passenger. \n\t\tARGS:\n\
\t\t1. Destination of the route.\n\
\t\t2. Name.\n\
\t\t2. Phone number.\n\
\tremove-passenger - Remove a passenger. \n\t\tARGS:\n\
\t\t1. Destination of the route.\n\
\t\t2. Name.\n\
\tmodify-passenger - Modify a passenger. \n\t\tARGS:\n\
\t\t1. Current destination of the route.\n\
\t\t2. Current name.\n\
\t\t3. New destination. In case of no change, type =\n\
\t\t4. New name. In case of no change, type =\n\
\t\t5. New phone. In case of no change, type =\n\
\ttake-route - Take a route. \n\t\tARGS:\n\
\t\t1. Destination of the route.\n");
}

void task_list_routes()
{
	int i;
	for (i = 0; i < num_of_routes; ++i)
	{
		printf("%s\n", routes[i].destination);
		int j;
		for (j = 0; j < routes[i].num_of_passengers; ++j)
			printf("\tname: %s, phone: %s, last modification time: %s",
				routes[i].passengers[j].name,
				routes[i].passengers[j].phone,
				ctime(&(routes[i].passengers[j].mod_time)));//struct of struct(struct passenger)
	}
}

void task_add_route(struct task * task)//add a destinantion if required
{
	if (task->num_of_args != 3)
	{
		puts("[ERROR] Invalid number of args. It must be 1, which is the destination.");
		return;
	}

	add_route(task->args[2], &num_of_routes, routes);//route file
}

void task_remove_route(struct task * task)
{
	if (task->num_of_args != 3)
	{
		puts("[ERROR] Invalid number of args. It must be 1, which is the destination.");
		return;
	}

	remove_route(task->args[2], &num_of_routes, routes);//route file
}

void task_add_passenger(struct task * task)
{
	if (task->num_of_args != 5)
	{
		puts("[ERROR] Invalid number of args. It must be 3.");
		return;
	}

	int i;
	for (i = 0; i < num_of_routes; ++i)
		if (strcmp(routes[i].destination, task->args[2]) == 0)
		{
			add_passenger(task->args[3], task->args[4], &routes[i]);//route file
			return;
		}

	puts("[ERROR] No route found for the given destination.");
}

void task_remove_passenger(struct task * task)
{
	if (task->num_of_args != 4)
	{
		puts("[ERROR] Invalid number of args. It must be 2.");
		return;
	}

	remove_passenger(task->args[2], task->args[3], num_of_routes, routes);//route file
}

void task_modify_passenger(struct task * task)
{
	if (task->num_of_args != 7)
	{
		puts("[ERROR] Invalid number of args. It must be 5.");
		return;
	}

	modify_passenger(task->args[2], task->args[3], task->args[4], task->args[5], task->args[6], num_of_routes, routes);//route file
}

void task_take_route(struct task * task)
{
	if (task->num_of_args != 3)
	{
		puts("[ERROR] Invalid number of args. It must be 1.");
		return;
	}

	// Process the only argument.
	struct route *route = get_route(task->args[2], num_of_routes, routes);
	if (route != NULL)
	{
		printf("Take a route to %s...\n", route -> destination);
	}
	else
	{
		printf("[ERROR] No route to %s.\n", route -> destination);
		return;
	}

	// Create the pipe.(unidirection from left to right) 
	int pipefd[2];
	if (pipe(pipefd) == -1)//on success returns 0
	{
		printf("[ERROR] Cannot instantiate a pipe.\n");
		return;
	}

	// Create the child process, which will be the 'courier'.
	pid_t pid = fork();//to communicate btwn parent and child
	if (pid < 0)
	{
		printf("[ERROR] Cannot fork.\n");
		return;
	}
	else if (pid > 0)//parent (2nd)
	{
		// Parent process, which is the 'uber'.
		manage_route(pid, pipefd, route);
	}
	else//child as pid is 0(1st)
	{
		// Child process, which is the 'courier'.
		guide_route(pipefd, route);
	}
}

void manage_route(pid_t pid, int pipefd[2], struct route *route)//pipe used parent process
{
	// Close the writing end.
	close(pipefd[1]);//child to parent

	struct feedback fb;

	// Waiting for the results.
	puts("The Uber is waiting for the results...");
	read(pipefd[0], &fb, sizeof(fb));
	close(pipefd[0]);

	// Write out the results.
	int i;
	for (i = 0; i < fb.num_of_passengers; ++i)
		printf("%s answer: %d\n", fb.passengers[i].name, fb.values[i]);

	// Sends a signal to the courier that the result are arrived.
	puts("As the Uber, I am sending a signal to the courier that the result are arrived...");
	kill(pid, SIGTERM);//child killed,sigterm is prgrm termination

	// Wait for the courier and terminate.
	int status;
        waitpid(pid, &status, 0);// wait for process to change state//?????????
	puts("The courier terminated and the Uber is terminating now...");
}

void guide_route(int pipefd[2], struct route *route)
{
	// Close the reading end.
	close(pipefd[0]);

	// Guide the route.
	puts("The route is started...");
	int i;
	for (i = 0; i < ROUTE_LENGTH; ++i)
	{
		printf("Remaining time from the route: %d\n", ROUTE_LENGTH - i);
		sleep(1);
	}

	// Generate and write out the feedbacks.
	struct feedback fb;
  	srand(time(NULL));//The srand() function sets the starting point,after that random 
	fb.num_of_passengers = route -> num_of_passengers;
	for (i = 0; i < route -> num_of_passengers; ++i)
	{
		fb.passengers[i] = (route -> passengers)[i];
		fb.values[i] = 1 + rand() % 5;
	}
	write(pipefd[1], &fb, sizeof(fb));
	close(pipefd[1]);

	// Waiting for the response for the Uber, that the results arrived.
	puts("As a courier, I am waiting for the termination signal from the Uber...");
	pause();
}
