#ifndef ITEM_H
#define ITEM_H

#include <iostream>

#include "dice.h"
#include "objectfactory.h"

using namespace std;

class Item
{
	public:
		Item(
				string &name,
				string &desc,
				int type, 
				int color, 
				int hit,
				Dice *dam,
				int dodge, 
				int def, 
				int weight, 
				int speed, 
				int attr, 
				int val
			);

		~Item();

		string name;
		string desc;
		int type;
		int color;
		int hit;
		Dice *dam;
		int dodge;
		int def;
		int weight;
		int speed;
		int attr;
		int val;

		// symbol determined from type
		char symb;

		Item *next; // for stacked item

		inline char getSymb() { return next==NULL ? symb:'&'; }
		inline bool isEquipment() { return type <= 10; }
};

#endif
