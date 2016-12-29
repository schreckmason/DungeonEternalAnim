#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "npc.h"

NPC::NPC() : Character()
{
	abil = rand() % 16;
	speed = 5 + rand() % 16;

	// get symbol
	char symb[2];
	sprintf(symb, "%x", abil);
	this->symb = symb[0];

	mempcx = 0;
	mempcy = 0;

	// get color
	if (speed <= 8)
		color = COLOR_WHITE;
	else if (speed <= 12)
		color = COLOR_GREEN;
	else if (speed <= 16)
		color = COLOR_CYAN;
	else
		color = COLOR_MAGENTA;
}

NPC::NPC(
string &name , 
string &desc , 
char    symb ,
int     color, 
int     speed, 
int     abil , 
int     hp   , 
Dice *dam)
{
	this->name  = name;
	this->desc  = desc;
	this->symb  = symb;
	this->color = color;
	this->speed = speed;
	this->abil  = abil;
	this->hp    = this->hpmaxNaked = hp;
	this->dam   = dam;

	mempcx = 0;
	mempcy = 0;
}

NPC::~NPC()
{
}

void NPC::getMemPCLocation(int *x, int *y)
{
	*x = mempcx;
	*y = mempcy;
}

void NPC::setMemPCLocation(int x, int y)
{
	mempcx = x;
	mempcy = y;
}

