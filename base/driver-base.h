#pragma once

#define READ 0
#define WRITE 1

#define RUNNING 0
#define ERROR -1

#define MAXAGENTS 12
#define MAXDICE 100
#define MAXSIDES 1000

struct agent_t
{
	int id;
	char name[256];       // bot name

	unsigned int dice[MAXDICE];
	int truth;
	float score;

// META:
	int status;           // bot's current status
	int fds[2];           // file descriptors to codriverunicate
	int pid;              // process id of agent
	int timeout;          // amount of time given to respond

// VISUALIZATION:
	void* vis;            // data for visualisation
};

