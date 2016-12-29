#ifndef DICE_H
#define DICE_H

#include <iostream>

using namespace std;

class Dice
{
	public:
		int base, dice, side;

		Dice(int base, int dice, int side) : 
		base(base),
		dice(dice),
		side(side) {}
		~Dice();

		static Dice *parseDice(string &s);

		int roll();

		inline int min() { return base + dice; }
		inline int max() { return base + dice * side; }
};

ostream& operator<<(ostream& os, Dice &dice);

#endif

