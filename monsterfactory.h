#ifndef MONSTERFACTORY_H
#define MONSTERFACTORY_H

#include <fstream>
#include <iostream>
#include <vector>

#include "dice.h"
#include "npc.h"

using namespace std;

class MonsterFactory
{
	private:
		Dice *dspeed; // dice speed
		Dice *dhp;    // dice hp
		Dice *ddam;   // dice dam

		char csymb; // char symb
		int icolor; // int  color
		int iabil;  // int  abil
			
	public:
		MonsterFactory();
		~MonsterFactory();
		
		string name;
		string desc;
		string symb;
		string color;
		string speed;
		string abil;
		string hp;
		string dam;

		bool allFieldsFilled();

		static int deleteFactories();

		NPC *generateNPC();

		// generate a random monster from factories
		static NPC *generateRandNPC();

		// convert all string fields to monster fields
		int initFields();

		static MonsterFactory *next(istream &is);
				
		static int load(const char *path);

		static vector<MonsterFactory *> factories;
};

ostream &operator<<(ostream &os, MonsterFactory &);

#endif
