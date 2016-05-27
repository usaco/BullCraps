#include <stdio.h>

#include "client.h"

// This is the name that the driver will refer to your bot as.
const char* BOT_NAME = "BOTNAME";

// Return whether setup was successful, bot dies if 0.
int client_setup(int *argc, char ***argv)
{
	return 1;
}

// This function is called when the game begins.
void game_setup(const struct player_data* players)
{

}

// This function is called when your turn comes up. Make changes to output to make a claim.
void make_claim(unsigned int roundnum, const struct player_data* players, struct claim* output)
{

}

// This function is called when someone else has made a claim. Return 1 to accuse "truth" and 0 to accuse "lie".
int make_accusation(struct claim* claim)
{

}

// This function is called when all accusations have been recorded (a good place to learn from your opponents)
void end_turn(unsigned int roundnum, const struct player_data* players, struct claim* last_claim)
{

}

// This function is called at the end of the game, as a courtesy.
void game_end()
{

}

