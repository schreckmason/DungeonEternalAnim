#ifndef PC_H
#define PC_H

#include "character.h"
#include "item.h"
#include "inventory.h"

class PC : public Character
{
	public:
		PC();
		~PC();

		static vector<Item *> equipment;

		static Inventory *invent;

		NPC *attacking; // currently attacking

		char *seenDungeon;
			
		void clearSeenDungeon();
		
		inline char *getSeenDungeon() { return seenDungeon; }
	
		void setLocation(int x, int y);
	
		int getSpeed();

		int getTotalDam();

		int getTotalDamRanged();

		int getTotalDef();

		int hpmax(); // hpmax plus item hit bonus

		/* Inventory Updates */
		static int pickup(int x, int y);
		static int drop(int slot, int x, int y);
		static int expunge(int slot);

		/* Equipment Updates */
		static int equip(int slot);
		static int unequip(int index);
		static int hasItem(int type);

};

extern PC *pc;

#endif

