#ifndef MOVE_H
#define MOVE_H

#include "core.h"
#include "npc.h"

class Move
{
	public:
		/* move NPC based on its ABIL */
		static int npc(NPC *);
		/* move PC automatically */
		static int pcAI();
		/* move any character */
		static int move(Character *, int, int);

		static int dijkstra(Character *, int tunneling);
		static int random(Character *, int tunneling);
		static int toward(Character *, int, int, int);
};

#endif

