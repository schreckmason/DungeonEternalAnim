#ifndef POINT_H
#define POINT_H

class Point
{
	public:
		int x, y;

		Point() : x(0), y(0) {};
		Point(int x, int y);
		~Point();

		// get a Point without using new
		static Point getPoint(int x, int y);

		static void neighbors(Point neighbors[8], int x, int y);
};

#endif

