#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "core.h"
#include "point.h"

extern int distN[DUNG_H][DUNG_W]; // non-tunneling
extern int distT[DUNG_H][DUNG_W]; // tunneling

class Dijkstra
{
	private:
		static int visited[DUNG_H][DUNG_W];
	
	public:
		static Point next(int x, int y, int tunneling);
		
		static int print(int tunneling);

		// run Dijkstra to find distance to (x, y)
		static int run(int x, int y, int tunneling);
};

#endif

