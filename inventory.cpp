#include "inventory.h"
#include "item.h"
#include "debug.h"
#include "dungeon.h"

Inventory::Inventory()
{

}

Inventory::~Inventory()
{

}

vector<Item *> Inventory::inv;

/* Should be called every time PC's location and imap at the same location is not null */
void Inventory::addItem(Item *item)
{
	inv.push_back(item);
	Debug::log("Added %c to inventory", item->getSymb());

}

/* Should be called once you select the index of inventory, need to get the item at then spot then call this method */
void Inventory::removeItem(int slot, int x, int y)
{
	for(int i=0;i<inv.size();i++)
	{
		if(inv.at(slot) == NULL)
		{
			Debug::log("Nothing in that inventory slot");
			break;
		}
		else
		{
			dungeon->placeItem(inv.at(slot),x, y);
			Debug::log("Dropped %c", inv.at(slot)->getSymb());
			inv.erase(inv.begin()+slot);
			break;
		}
	}
}

/* Should be called every time a PC's location is the same as a location for imap, first check */
bool Inventory::isFull()
{
	return inv.size() == 10;
}

void Inventory::banish(int slot)
{
	for(int i=0;i<inv.size();i++)
	{
		if(inv.at(slot)==NULL)
		{
			Debug::log("Can't expunge NULL, come on now");
			break;
		}
		else
		{
			inv.erase(inv.begin()+slot);
			break;
		}
	}
}




