#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>

#include "core.h"
#include "dice.h"
#include "debug.h"
#include "monsterfactory.h"
#include "objectfactory.h"
#include "parser.h"
#include "ui.h"

int Parser::parse(const char *path)
{
	ifstream ifs(path);

	if (!ifs) {
		cerr << "Failed to open " << path << endl;
		return -1;
	}
	
	if (ifs.eof())
		return -1;

	// meta
	string line;
	getline(ifs, line);
	if (line=="RLG327 MONSTER DESCRIPTION 1") {
		while (!ifs.eof()) {
			MonsterFactory *mf = MonsterFactory::next(ifs);

			if (mf) {
				cerr << *mf << endl;
				delete mf;
			}
		}
		ifs.close();
		return 0;
	}
	if (line=="RLG327 OBJECT DESCRIPTION 1") {
		while (!ifs.eof()) {
			ObjectFactory *of = ObjectFactory::next(ifs);

			if (of) {
				cerr << *of << endl;
				delete of;
			}
		}
		ifs.close();
		return 0;
	}

	ifs.close();
	return -1;
}

int Parser::parseAbil(string &abil)
{
	stringstream ss(abil);
	int iabil = 0;
	while (!ss.eof()) {
		string a;
		ss >> a;
		if (a.empty())
			return -1;
		else if (a=="SMART"  ) iabil |= (1<<0);
		else if (a=="TELE"   ) iabil |= (1<<1);
		else if (a=="TUNNEL" ) iabil |= (1<<2);
		else if (a=="ERRATIC") iabil |= (1<<3);
		else if (a=="PASS"   ) iabil |= (1<<4);
		else
			return -1;
	}
	return iabil;
}

int Parser::parseColor(string &s)
{
	if (s=="BLACK"  ) return COLOR_WHITE;
	if (s=="RED"    ) return COLOR_RED;
	if (s=="GREEN"  ) return COLOR_GREEN;
	if (s=="YELLOW" ) return COLOR_YELLOW;
	if (s=="BLUE"   ) return COLOR_BLUE;
	if (s=="MAGENTA") return COLOR_MAGENTA;
	if (s=="CYAN"   ) return COLOR_CYAN;
	if (s=="WHITE"  ) return COLOR_WHITE;
	return -1;
}

Dice *Parser::parseDice(string &s) {
	return Dice::parseDice(s);
}

int Parser::parseDiceField(stringstream &ss, string &field)
{
	if (!field.empty()) {
		Debug::log("duplicate field");
		return 1;
	}

	trim(ss);
	getline(ss, field);

	Dice *dice = parseDice(field);
	if (NULL==dice) {
		Debug::log("error parsing %s", field.c_str());
		return 1;
	} else {
		delete dice;
	}
	return 0;
}

int Parser::parseSymb(string &s)
{
	if (1==s.length()) return s.c_str()[0];
	return -1;
}

int Parser::parseType(string &s)
{
	for (int i = 0; i < 20; i++)
		if (s==ObjectFactory::TYPE[i])
			return i;
	return -1;
}

int Parser::trim(stringstream &ss)
{
	while (ss.peek()==' ')
		ss.get();
	
	return 0;
}

string &Parser::trim(string &s)
{
	stringstream ss(s);
	trim(ss);
	s = ss.str();
	return s;
}

