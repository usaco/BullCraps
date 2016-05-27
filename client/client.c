#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "client.h"

#define MSG_BFR_SZ 128

// helper macro, sorry for the mess...
#define EXPECTED(m, s) { fprintf(stderr, "[%u] Expected command %s," \
	" received %s.\n", SELF->id, s, m); return EXIT_FAILURE; }
/* these functions should be defined by the bot author */

extern const char* BOT_NAME;

extern int client_setup(int* /*argc*/, char*** /*argv*/);

extern void game_setup(const struct player_data* /*players*/);

extern void make_claim(unsigned int /*roundnum*/, const struct player_data* /*players*/, struct claim* /*output*/);

extern int make_accusation(struct claim* /*input*/);

extern void end_turn(unsigned int /*roundnum*/, const struct player_data* /*players*/);

extern void game_end();

// ########################################################

unsigned int NUMPLAYERS = -1;
unsigned int NUMDICE = -1;
unsigned int NUMSIDES = -1;
struct player_data players[MAXPLAYERS];

struct player_data* SELF;

int _fdout = STDOUT_FILENO, _fdin = STDIN_FILENO;

int recv(char* msg)
{
	bzero(msg, MSG_BFR_SZ);

	// read message from file descriptor for a bot
	int bl, br; char* m = msg;
	for (bl = MSG_BFR_SZ; bl > 0; m += br)
		bl -= br = read(_fdin, m, bl);

	return br;
}

int send(char* msg)
{
	// write message to file descriptor for a bot
	int bl, br; char* m = msg;
	for (bl = MSG_BFR_SZ; bl > 0; m += br)
		bl -= br = write(_fdout, m, bl);

	return br;
}

int main(int argc, char **argv)
{
	int i, myid, cc;
	char msg[MSG_BFR_SZ];
	char tag[MSG_BFR_SZ];

	struct player_data *p = NULL;

	--argc; ++argv;
	setbuf(stdout, NULL);
	setbuf(stdin , NULL);
	
	struct timeval tv;
	gettimeofday(&tv, NULL);
	if (!client_setup(&argc, &argv))
		return EXIT_FAILURE;

	recv(msg); sscanf(msg, "%*s %d", &myid);
	sprintf(msg, "NAME %s", BOT_NAME); send(msg);

	SELF = &players[myid];
	srand(tv.tv_usec+myid);
	while ((cc = recv(msg)))
	{
		sscanf(msg, "%s", tag);
		
		if (!strcmp(tag, "READY")) break;
		else if (!strcmp(tag, "PLAYERS"))
		{
			sscanf(msg, "%*s %u", &NUMPLAYERS);
			for (i = 0, p = players; i < NUMPLAYERS; ++i, ++p)
			{
				p->id = i;
				p->last_claim.owner = i;
			}
		}
		else if (!strcmp(tag, "DICE"))
			sscanf(msg, "%*s %u", &NUMDICE);
		else if (!strcmp(tag, "SIDES"))
			sscanf(msg, "%*s %u", &NUMSIDES);
	}

	game_setup(players);

	unsigned int rnum = 0, oid;
	while ((cc = recv(msg)))
	{
		sscanf(msg, "%s", tag);
		
		if (!strcmp(tag, "ENDGAME")) break;
		else if (!strcmp(tag, "GO"))
		{
			make_claim(++rnum, players, &SELF->last_claim);
			sprintf(msg, "CLAIM %u %u",
				SELF->last_claim.val, SELF->last_claim.count);
			send(msg);
		}
		else if (!strcmp(tag, "CLAIMED"))
		{
			sscanf(msg, "%*s %u", &oid);
			sscanf(msg, "%*s %*u %u %u",
				&players[oid].last_claim.val,
				&players[oid].last_claim.count);

			sprintf(msg, "RESPOND %u",
				make_accusation(&players[oid].last_claim));
			send(msg);
		}
		else if (!strcmp(tag, "ACCUSE"))
		{
			sscanf(msg, "%*s %u", &oid);
			sscanf(msg, "%*s %*u %u",
				&players[oid].last_accusation);
		}
		else if (!strcmp(tag, "ENDTURN"))
		{
			end_turn(rnum, players);
		}
		
		// got an unexpected message...
		else EXPECTED(tag, "anything familiar");
	}

	quit: game_end();
	return EXIT_SUCCESS;
}
