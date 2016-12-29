#include <stdlib.h>

#include "point.h"

Point::Point(int x, int y)
{
	this->x = x;
	this->y = y;
}

Point::~Point()
{
}

Point Point::getPoint(int x, int y)
{
	Point p;
	p.x = x;
	p.y = y;
	return p;
}

void Point::neighbors(Point ns[8], int x, int y)
{	
	ns[0] = getPoint(x, y - 1); // N
	ns[1] = getPoint(x, y + 1); // S
	ns[2] = getPoint(x - 1, y); // W
	ns[3] = getPoint(x + 1, y); // E
	ns[4] = getPoint(x - 1, y - 1); // NW
	ns[5] = getPoint(x + 1, y - 1); // NE
	ns[6] = getPoint(x - 1, y + 1); // SW
	ns[7] = getPoint(x + 1, y + 1); // SE
}

