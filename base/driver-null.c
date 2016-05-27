#include <unistd.h>
#include <stdio.h>

#include "driver-base.h"
#include "driver-visual.h"

int setup_bcb_vis(int numagents, struct agent_t *agents, int *argc, char ***argv)
{
	return 1;
};

int update_bcb_vis(int numagents, struct agent_t *agents, const int turn)
{
	int i; struct agent_t *a = agents;
	fprintf(stderr, "Round #%d\n", turn);

	for (i = 0; i < numagents; ++i, ++a)
	{
		fprintf(stderr, "Player #%d (%s): Score = %f\n", i, a->name, a->score);
	}
	sleep(1);

	return 1;
};

void close_bcb_vis()
{

};

