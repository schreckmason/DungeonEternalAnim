#ifndef PARSER_H
#define PARSER_H

#include <iostream>

using namespace std;

class Parser
{
	public:
		// parse a given file specified by the path
		static int parse(const char *);
		
		// get int for given abilities
		static int parseAbil(string &);
	
		// get int associated with the given color string
		static int parseColor(string &);

		// see also Dice::parseDice
		static Dice *parseDice(string &);

		// return 1 if there are parsing errors
		static int parseDiceField(stringstream &ss, string &field);

		// get first char from string, return -1 if s.length > 1
		static int parseSymb(string &s);

		// get object type index
		static int parseType(string &s);

		// trim leading spaces of stringstream
		static int trim(stringstream &);
		
		// trim leading spaces of string
		static string &trim(string &);
};

#endif
