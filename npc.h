#ifndef NPC_H
#define NPC_H

#include "character.h"

class NPC : public Character
{
	private:
		int mempcx;
		int mempcy;

	public:
		string name;
		string desc;
		
		int abil;

		NPC();
		NPC(string &name, string &desc, 
		char symb,
		int color, int speed, int abil, int hp, 
		Dice *dam); 

		~NPC();

		inline int getAbil() {return abil;}

		void getMemPCLocation(int *, int *);
	
		inline void setAbil(int abil) {this->abil = abil;}
		
		void setMemPCLocation(int x, int y);
};

#endif

