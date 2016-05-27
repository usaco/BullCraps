#ifndef CLIENT_H
#define CLIENT_H

#define MAXPLAYERS 12
#define MAXDICE 100
#define MAXSIDES 1000

struct claim
{
	unsigned int owner;
	unsigned int val;
	unsigned int count;
};

// player information
struct player_data
{
	unsigned int id;

	// information about the player
	unsigned int dice[MAXDICE];
	struct claim last_claim;
	unsigned int last_accusation;
};

extern unsigned int NUMPLAYERS;
extern unsigned int NUMDICE;
extern unsigned int NUMSIDES;

// my bot's data
extern struct player_data *SELF;

// my bot's dice
extern unsigned int DICE[];

// file descriptors
extern int _fdout, _fdin;

#endif
