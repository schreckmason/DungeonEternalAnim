#ifndef TURN_H
#define TURN_H

#include "character.h"
#include "heap.h"

class Turn
{
	private:
		Heap *h;
	public:
		Turn();
		~Turn();
		int enqueue(Character *);
		Character *dequeue();
};

#endif

