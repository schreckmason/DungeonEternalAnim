#ifndef DEBUG_H
#define DEBUG_H

extern int debug;

class Debug
{
	public:
		static int log(const char *, ...);
};

#endif
