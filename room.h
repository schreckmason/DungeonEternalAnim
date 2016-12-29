#ifndef ROOM_H
#define ROOM_H

class Dungeon;

class Room
{
	public:
		unsigned char x, y, w, h;

		Room();
		~Room();

		// true if (x, y) is in room
		bool contains(int x, int y);

		// get the centroid of the room
		int getCentroid(int *, int *);

		// determine if two rooms are adjacent or overlap
		static bool collide(Room *, Room *);

		// true if two points are in the same room
		static bool isInSameRoom(Dungeon *, int x1, int y1, int x2, int y2);

		// paint the room on the dungeon
		int paintOn(Dungeon *);

		// get a random location inside this room
		int getRandLocation(int *x, int *y);
};

#endif

