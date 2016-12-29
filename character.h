#ifndef CHARACTER_H
#define CHARACTER_H

#include "core.h"
#include "dice.h"
#include "point.h"

class Character
{
	protected:
		int hpmaxNaked; // max hp without wearing items
		int mpmaxNaked; // max mp without wearing items
		
		// damage
		Dice *dam;
	
		int x, y;
		
		bool dead;
		
		char symb;
		
		int color;
		int speed;
		int turn;
		
	public:
		int hp;
		int mp;

		// turns left for the effects	
		int poison;
		int frozen;
	
	public:
		Character();
		virtual ~Character();
		
		// getters and setters for (x, y)
		Point getLocation();
		void getLocation(int *, int *);
		virtual void setLocation(int x, int y);
		inline int getX() { return x; }
		inline int getY() { return y; }
		inline void setX(int x) { setLocation(x, y); }
		inline void setY(int y) { setLocation(x, y); }
	
		int getColor();	
		
		virtual int hpmax() { return hpmaxNaked; } 
		virtual int mpmax() { return mpmaxNaked; }

		inline bool isDead() { return dead; }
		inline void setDead() { dead = true; hp = 0; }
		
		// true if this character is PC
		inline bool isPC() { return symb == '@'; }
	
		// speed
		virtual int getSpeed() { return speed; }
		
		// symbol
		inline char getSymb() { return symb; } 
	
		// turn	
		inline int  getTurn() { return turn; }
		inline void setTurn(int turn) { this->turn = turn; }
		
		// total damage and defense
		virtual int getTotalDam();
		virtual int getTotalDef();

};

#endif

