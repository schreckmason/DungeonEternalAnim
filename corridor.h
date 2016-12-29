#ifndef CORRIDOR_H
#define CORRIDOR_H

#include "room.h"

class Corridor
{
	public:
		// pave a path from an unconnected room to a connected room
		static void connect(Dungeon *, Room*, Room*);
};

#endif

