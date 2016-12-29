#include <ncurses.h>
#include <stdlib.h>

#include "core.h"
#include "debug.h"
#include "dungeon.h"
#include "pc.h"

PC::PC() : Character()
{
	speed = 10;
	symb = '@';

	seenDungeon = (char *)malloc(DUNG_W * DUNG_H);

	clearSeenDungeon();

	color = COLOR_PC;

	hp = hpmaxNaked = 100;
	mp = mpmaxNaked = 100;

	dam = new Dice(5, 5, 5);

	attacking = NULL; 
}

PC::~PC()
{
	free(seenDungeon);

	delete dam;
}

void PC::clearSeenDungeon()
{
	for (int i = 0; i < DUNG_W * DUNG_H; i++)
		seenDungeon[i] = ' ';
}

void PC::setLocation(int x, int y)
{
	Character::setLocation(x, y);

	pcx = x;
	pcy = y;

	for (int r=0; r<DUNG_H; r++) {
		for (int c=0; c<DUNG_W; c++) {
			if (dungeon->isVisible(c, r))
				seenDungeon[r*DUNG_W+c]=dungeon->tmap[r][c];
		}
	}
}
/* pick up items */
int PC::pickup(int x, int y)
{
	Item *item = dungeon->imap[y][x];

	/* Check to see if inventory is full */ 
	if(invent->isFull()) { Debug::log("inv full"); return 1; }
	else
	{
		invent->addItem(item);
		//Debug::log("Erase %c", item->getSymb());
		dungeon->eraseItem(x,y);
		return 0;
	}
}

/*drop item*/
int PC::drop(int slot, int x, int y)
{
	/*can't drop at PC's feet because it will be picked up in the same turn*/
	if(dungeon->hmap[y][x+1]==0){ invent->removeItem(slot,x+1,y); }
	else if(dungeon->hmap[y+1][x+1]==0) { invent->removeItem(slot, x+1, y+1); }
	else if(dungeon->hmap[y+1][x]==0) { invent->removeItem(slot, x, y+1); }
	else if(dungeon->hmap[y+1][x-1]==0) { invent->removeItem(slot, x-1, y+1); }
	else if(dungeon->hmap[y][x-1]==0) { invent->removeItem(slot, x-1,y); }
	else if(dungeon->hmap[y-1][x-1]==0) { invent->removeItem(slot,x-1,y-1); }
	else if(dungeon->hmap[y-1][x]==0) { invent->removeItem(slot,x,y-1); }
	else if(dungeon->hmap[y-1][x+1]==0) { invent->removeItem(slot,x+1,y-1); }
	else { return 1; }
	return 0;
}

int PC::expunge(int slot)
{
	invent->banish(slot);
	return 0;
}

vector<Item *> PC::equipment;

int PC::hasItem(int itype)
{
	for(int i=0;i<equipment.size();i++)
	{
		if(equipment[i]->type == itype)
		{ 
			Debug::log("%c already is equipped", equipment[i]->getSymb());
			return 1; }
	}
	return 0;
}

int PC::equip(int slot)
{
	Debug::log("equipped %c",invent->inv[slot]->getSymb());
	if(hasItem(invent->inv[slot]->type)==0)
	{
		equipment.push_back(invent->inv[slot]);
		invent->banish(slot);//quick way to handle removing from inventory with no side effects fetches back from inventory
		return 0;
	}
	else{
		return 1;
	}
}

int PC::unequip(int slot)
{
	Debug::log("unequipped %c",equipment[slot]->getSymb());
	invent->inv.push_back(equipment[slot]);
	equipment.erase(equipment.begin()+slot);
	return 0;
}

int PC::getSpeed()
{
	int total = 0;
	total = Character::getSpeed();
	for(int i=0;i<equipment.size();i++)
	{
		total += equipment[i]->speed;
	}
	return total;
}

int PC::getTotalDam()
{
	int total = 0;
	total = dam->roll();
	for(int i=0;i<equipment.size();i++)
	{
		total += equipment[i]->dam->roll();
	}
	return total;
}

int PC::getTotalDef()
{
	int total = 0;
	for(int i=0;i<equipment.size();i++)
	{
		total += equipment[i]->def;
	}
	return total;
}

int PC::hpmax()
{
	return hpmaxNaked;
}

// singleton PC
PC *pc;
