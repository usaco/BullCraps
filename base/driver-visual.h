#pragma once
#include "driver-base.h"

extern int NUMDICE;
extern int NUMSIDES;
extern int NUMAGENTS;

int setup_bcb_vis(int numagents, struct agent_t *agents, int *argc, char ***argv);

int update_bcb_vis(int numagents, struct agent_t *agents, const int turn);

void close_bcb_vis();

