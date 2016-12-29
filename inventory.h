#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.h"

using namespace std;

class Inventory
{

public:
Inventory();
~Inventory();

static vector<Item *> inv;

inline Item *get(int index) { return inv.at(index); }
inline int getSize() { return inv.size(); } 

static void addItem(Item *item);
static void removeItem(int slot, int x, int y);
static bool isFull();

static void banish(int slot);
};






#endif
