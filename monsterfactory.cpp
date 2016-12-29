#include <stdlib.h>

#include <iostream>
#include <sstream>

#include "debug.h"
#include "monsterfactory.h"
#include "parser.h"

using namespace std;

vector<MonsterFactory *> MonsterFactory::factories;

MonsterFactory::MonsterFactory()
{
	dspeed = dhp = ddam = NULL;
}

MonsterFactory::~MonsterFactory()
{
	if (dspeed) delete dspeed;
	if (dhp)    delete dhp;
	if (ddam)   delete ddam;
}

int MonsterFactory::deleteFactories()
{
	for (int i = 0; i < (int)factories.size(); i++) {
		delete factories[i];
	}
	factories.clear();

	return 0;
}

NPC *MonsterFactory::generateNPC()
{	
	NPC *npc = new NPC(
	name, 
	desc,
	csymb, 
	icolor, 
	dspeed->roll(), 
	iabil,
	dhp->roll(),
	ddam
	);
	return npc;
}

NPC *MonsterFactory::generateRandNPC()
{
	int index = rand() % factories.size();

	return factories[index]->generateNPC();
}

int MonsterFactory::initFields()
{	
	dspeed = Dice::parseDice(speed);
	dhp    = Dice::parseDice(hp);
	ddam   = Dice::parseDice(dam);

	csymb  = Parser::parseSymb (symb);
	icolor = Parser::parseColor(color);
	iabil  = Parser::parseAbil (abil);
	
	return 0;
}

int MonsterFactory::load(const char *path)
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
			MonsterFactory *mf = next(ifs);

			if (mf) {
				factories.push_back(mf);
			}
		}
		ifs.close();
		return 0;
	}

	ifs.close();
	return -1;
}

MonsterFactory *MonsterFactory::next(istream &is)
{
	MonsterFactory *mf = NULL;

	while (!is.eof()) {
		if (mf) delete mf;
		mf = new MonsterFactory;

		bool hasParsingError = false;

		string line;
		while (!is.eof()) {
			getline(is, line);
			if (line=="BEGIN MONSTER")
				break;
		}
		while (!is.eof() && !hasParsingError) {
			getline(is, line);
			string k;
			stringstream ss(line);
			ss >> k;
			if (k=="END") {
				break;
			} else if (k=="NAME") {
				if (!mf->name.empty()) {
					Debug::log("duplicate NAME");
					hasParsingError = true;
				}
				Parser::trim(ss);
				getline(ss, mf->name);
			} else if (k=="DESC") {
				if (!mf->desc.empty()) {
					Debug::log("duplicate DESC");
					hasParsingError = true;
				}
				while(!is.eof()) {
					getline(is, line);
					if (line.c_str()[0]=='.') break;
					mf->desc += line;
					if (is.peek()!='.')
						mf->desc +='\n';
				}
			} else if (k=="SYMB") {
				if (!mf->symb.empty()) {
					Debug::log("duplicate SYMB");
					hasParsingError = true;
				}
				Parser::trim(ss);
				getline(ss, mf->symb);

				if (-1==Parser::parseSymb(mf->symb)) {
					Debug::log("error parsing SYMB %s",
							mf->symb.c_str());
					hasParsingError = true;
				}
			} else if (k=="COLOR") {
				if (!mf->color.empty()) {
					Debug::log("duplicate COLOR");
					hasParsingError = true;
				}
				Parser::trim(ss);
				getline(ss, mf->color);

				if (-1==Parser::parseColor(mf->color)) {
					Debug::log("error parsing COLOR %s",
							mf->color.c_str());
					hasParsingError = true;
				}
			} else if (k=="SPEED") {
				hasParsingError = Parser::parseDiceField(ss, mf->speed);
			} else if (k=="ABIL") {
				if (!mf->abil.empty()) {
					Debug::log("duplicate ABIL");
					hasParsingError = true;
				}
				Parser::trim(ss);
				getline(ss, mf->abil);
				
				if (-1==Parser::parseAbil(mf->abil)) {
					Debug::log("error parsing ABIL %s",
							mf->abil.c_str());
					hasParsingError = true;
				}
			} else if (k=="HP") {
				hasParsingError = Parser::parseDiceField(ss, mf->hp);
			} else if (k=="DAM") {
				hasParsingError = Parser::parseDiceField(ss, mf->dam);
			}
		}
		if (hasParsingError)
			continue;
		if (mf->allFieldsFilled()) {
			mf->initFields();
			return mf;
		}
	}
	if (mf) delete mf;
	return NULL;
}

bool MonsterFactory::allFieldsFilled()
{
	return 
		!name .empty() && 
		!desc .empty() && 
		!symb .empty() &&
		!color.empty() &&
		!speed.empty() && 
		!abil .empty() && 
		!hp   .empty() && 
		!dam  .empty();
}

ostream& operator<<(ostream& os, MonsterFactory &mf)
{
	os << mf.name  << endl;
	os << mf.desc  << endl;
	os << mf.symb  << endl;
	os << mf.color << endl;
	os << mf.speed << endl;
	os << mf.abil  << endl;
	os << mf.hp    << endl;
	os << mf.dam   << endl;
	return os;
}

