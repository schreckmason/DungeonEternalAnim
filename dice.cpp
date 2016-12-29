#include <stdio.h>
#include <stdlib.h>

#include "dice.h"

ostream& operator<<(ostream& os, Dice &dice)
{
	os << dice.base << "+"
	   << dice.dice << "d"
	   << dice.side;
	return os;
}

Dice::~Dice()
{
}

Dice *Dice::parseDice(string &s)
{
	int base = -1, dice = -1, side = -1;
	if (sscanf(s.c_str(), "%d+%dd%d", &base, &dice, &side) < 3)
		return NULL;
	else
		return new Dice(base, dice, side);
}

int Dice::roll()
{
	int r = base;
	for (int i=0; i<dice; i++)
	{
		r += (1 + rand() % side);
	}
	return r;
}

