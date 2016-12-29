#include <stdio.h>

#include "character.h"
#include "core.h"
#include "debug.h"
#include "heap.h"
#include "turn.h"

static int compare(const void *cv, const void *dv)
{
	Character *c = (Character *) cv;
	Character *d = (Character *) dv;
	
	int cturn = c->getTurn();
	int dturn = d->getTurn();
	
	if (cturn == dturn)
		return (!c->isPC()) - 
		       (!d->isPC()); // PC has higher priority
	else
		return cturn - dturn;
}

Turn::Turn()
{
	h = new Heap(compare);
}

Turn::~Turn()
{
	delete h;
}

int Turn::enqueue(Character *c)
{
	int turn = c->getTurn();
	int speed = c->getSpeed();
	 	 
	turn += (100/speed);
	c->setTurn(turn);
	 	 
	h->insert(c);
	return 0;
}

Character *Turn::dequeue()
{
	return (Character *)h->extract();
}

