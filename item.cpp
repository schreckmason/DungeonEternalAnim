#include "item.h"
#include "objectfactory.h"

Item::Item(
		string &name  , 
		string &desc  ,
		int     type  , 
		int     color , 
		int     hit   ,
		Dice   *dam   ,
		int     dodge ,
		int     def   , 
		int     weight,
		int     speed , 
		int     attr  ,
		int     val)
{
	this->name   = name;
	this->desc   = desc;
	this->type   = type;
	this->color  = color;
	this->hit    = hit;
	this->dam    = dam;
	this->dodge  = dodge;
	this->def    = def;
	this->weight = weight;
	this->speed  = speed;
	this->attr   = attr;
	this->val    = val;

	// symbol
	this->symb   = ObjectFactory::SYMB[type];

	// stacked next
	this->next   = NULL;
}

Item::~Item()
{
}

