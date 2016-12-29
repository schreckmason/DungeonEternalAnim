#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include <fstream>
#include <iostream>
#include <vector>

#include "dice.h"
#include "item.h"

using namespace std;

class Item;

class ObjectFactory
{
	private:
		Dice *dhit;
		Dice *ddam;
		Dice *ddodge;
		Dice *ddef;
		Dice *dweight;
		Dice *dspeed;
		Dice *dattr;
		Dice *dval;

		int icolor;
		int itype; // type index

	public:
		string name;
		string desc;
		string type;
		string color;
		string hit;
		string dam;
		string dodge;
		string def;
		string weight;
		string speed;
		string attr;
		string val;

		ObjectFactory();
		~ObjectFactory();

		static char   SYMB[];
		static string TYPE[];

		static vector<ObjectFactory *> factories;

		bool allFieldsFilled();

		static int deleteFactories();

		Item *generateItem();

		static Item *generateRandItem();
		
		// convert all string fields to object fields
		int initFields();

		static int load(const char *path);

		static ObjectFactory *next(istream &is);
};

ostream &operator<<(ostream &os, ObjectFactory &);

#endif
