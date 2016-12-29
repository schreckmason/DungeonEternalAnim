#include <stdlib.h>

#include <iostream>
#include <sstream>

#include "debug.h"
#include "objectfactory.h"
#include "parser.h"

using namespace std;

vector<ObjectFactory *> ObjectFactory::factories;

char ObjectFactory::SYMB[] =
{
	// 0 ~ 4
	'|' ,
	')' ,
	'}' ,
	'[' ,
	']' ,
	// 5 ~ 9
	'(' ,
	'{' ,
	'\\',
	'=' ,
	'"' ,
	// 10 ~ 15
	'_' ,
	'~' ,
	'?' ,
	'!' ,
	'$' ,
	// 16 ~ 20
	'/' ,
	',' ,
	'-' ,
	'%' ,
	'&'
};

string ObjectFactory::TYPE[] =
{
	// 0 ~ 4
	"WEAPON"    ,
	"OFFHAND"   ,
	"RANGED"    ,
	"ARMOR"     ,
	"HELMET"    ,
	// 5 ~ 9
	"CLOAK"     ,
	"GLOVES"    ,
	"BOOTS"     ,
	"RING"      ,
	"AMULET"    ,
	// 10 ~ 15
	"LIGHT"     ,
	"SCROLL"    ,
	"BOOK"      ,
	"FLASK"     ,
	"GOLD"      ,
	// 16 ~ 20
	"AMMUNITION",
	"FOOD"      ,
	"WAND"      ,
	"CONTAINER" ,
	"STACK"
};

ObjectFactory::ObjectFactory()
{
	dhit = ddam = ddodge = ddef = dweight = dspeed = dattr = dval = NULL;
}

ObjectFactory::~ObjectFactory()
{
	if (dhit   ) delete dhit   ;
	if (ddam   ) delete ddam   ;
	if (ddodge ) delete ddodge ;
	if (ddef   ) delete ddef   ;
	if (dweight) delete dweight;
	if (dspeed ) delete dspeed ;
	if (dattr  ) delete dattr  ;
	if (dval   ) delete dval   ;
}

int ObjectFactory::deleteFactories()
{
	for (int i = 0; i < (int)factories.size(); i++) {
		delete factories[i];
	}
	factories.clear();

	return 0;
}

Item *ObjectFactory::generateItem()
{
	Item *item = new Item(
	name           , 
	desc           ,
	itype          ,
	icolor         ,
	dhit   ->roll(),
	ddam           ,
	ddodge ->roll(),
	ddef   ->roll(), 
	dweight->roll(),
	dspeed ->roll(), 
	dattr  ->roll(), 
	dval   ->roll()
	);

	return item;
}

Item *ObjectFactory::generateRandItem()
{
	int index = rand() % (int)factories.size();

	return factories[index]->generateItem();
}

int ObjectFactory::initFields()
{
	dhit    = Dice::parseDice(hit   );
	ddam    = Dice::parseDice(dam   );
	ddodge  = Dice::parseDice(dodge );
	ddef    = Dice::parseDice(def   );
	dweight = Dice::parseDice(weight);
	dspeed  = Dice::parseDice(speed );
	dattr   = Dice::parseDice(attr  );
	dval    = Dice::parseDice(val   );

	itype  = Parser::parseType (type );
	icolor = Parser::parseColor(color);

	return 0;
}

int ObjectFactory::load(const char *path)
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
	if (line=="RLG327 OBJECT DESCRIPTION 1") {
		while (!ifs.eof()) {
			ObjectFactory *of = next(ifs);

			if (of && of->allFieldsFilled()) {
				factories.push_back(of);
			} else {
				delete of;
			}
		}
		ifs.close();
		return 0;
	}
	ifs.close();
	return -1;
}

ObjectFactory *ObjectFactory::next(istream &is)
{
	ObjectFactory *of = NULL;
	
	while (!is.eof()) {
		if (of) delete of;
		of = new ObjectFactory;

		bool hasParsingError = false;

		string line;
		while (!is.eof()) {
			getline(is, line);
			if (line=="BEGIN OBJECT")
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
				if (!of->name.empty()) {
					Debug::log("duplicate NAME");
					hasParsingError = true;
				}
				Parser::trim(ss);
				getline(ss, of->name);
			} else if (k=="DESC") {
				if (!of->desc.empty()) {
					Debug::log("duplicate DESC");
					hasParsingError = true;
				}
				while(!is.eof()) {
					getline(is, line);
					if (line.c_str()[0]=='.') break;
					of->desc += line;
					if (is.peek()!='.')
						of->desc +='\n';
				}
			} else if (k=="TYPE") {
				if (!of->type.empty()) {
					Debug::log("duplicate TYPE");
					hasParsingError = true;
				}
				Parser::trim(ss);
				getline(ss, of->type);

				if (0 > Parser::parseType(of->type)) {
					Debug::log("error parsing TYPE %s", of->type.c_str());
					hasParsingError = true;
				}
			} else if (k=="COLOR") {
				if (!of->color.empty()) {
					Debug::log("duplicate COLOR");
					hasParsingError = true;
				}
				Parser::trim(ss);
				getline(ss, of->color);

				if (0 > Parser::parseColor(of->color)) {
					Debug::log("error parsing COLOR %s", of->color.c_str());
					hasParsingError = true;
				}
			} else if (k=="HIT") {	
				hasParsingError = Parser::parseDiceField(ss, of->hit);
			} else if (k=="DAM") {	
				hasParsingError = Parser::parseDiceField(ss, of->dam);
			} else if (k=="DODGE") {	
				hasParsingError = Parser::parseDiceField(ss, of->dodge);
			} else if (k=="DEF") {	
				hasParsingError = Parser::parseDiceField(ss, of->def);
			} else if (k=="WEIGHT") {	
				hasParsingError = Parser::parseDiceField(ss, of->weight);
			} else if (k=="SPEED") {	
				hasParsingError = Parser::parseDiceField(ss, of->speed);
			} else if (k=="ATTR") {	
				hasParsingError = Parser::parseDiceField(ss, of->attr);
			} else if (k=="VAL") {	
				hasParsingError = Parser::parseDiceField(ss, of->val);
			}
		}
		if (hasParsingError)
			continue;
		if (of->allFieldsFilled()) {
			of->initFields();
			return of;
		}
	}
	if (of) delete of;	
	return NULL;
}

bool ObjectFactory::allFieldsFilled()
{
	return !(
	name  .empty() || 
	desc  .empty() || 
	type  .empty() ||
	color .empty() || 
	hit   .empty() || 
	dam   .empty() ||
	dodge .empty() || 
	def   .empty() || 
	weight.empty() ||
	speed .empty() || 
	attr  .empty() || 
	val   .empty()
	);
}

ostream& operator<<(ostream& os, ObjectFactory &of)
{
	os << of.name   << endl;
	os << of.desc   << endl;
	os << of.type   << endl;
	os << of.color  << endl;
	os << of.hit    << endl;
	os << of.dam    << endl;
	os << of.dodge  << endl;
	os << of.def    << endl;
	os << of.weight << endl;
	os << of.speed  << endl;
	os << of.attr   << endl;
	os << of.val    << endl;
	return os;
}

