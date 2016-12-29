#include <stdio.h>
#include <stdlib.h>

#include "character.h"
#include "debug.h"
#include "npc.h"
#include "pc.h"

Character::Character()
{
	dead = false;
	turn = 0;

	poison = 0;
	frozen = 0;
}

Character::~Character()
{
}

Point Character::getLocation()
{
	return Point::getPoint(x, y);
}

void Character::getLocation(int *x, int *y)
{
	*x = this->x;
	*y = this->y;
}

void Character::setLocation(int x, int y)
{
	this->x = x;
	this->y = y;
}

int Character::getTotalDam()
{
	return dam->roll();
}

int Character::getTotalDef()
{
	return 0;
}

int Character::getColor()
{
	if (frozen)
		return COLOR_FROZEN;
	if (poison)
		return COLOR_POISON;
	return color;
}

