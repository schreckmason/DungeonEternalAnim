#include <stdio.h>
#include <stdlib.h>

#include "point.h"
#include "dijkstra.h"
#include "dungeon.h"
#include "heap.h"

Point *arr[DUNG_H * DUNG_W];

int Dijkstra::visited[DUNG_H][DUNG_W];

int distN[DUNG_H][DUNG_W]; // non-tunneling
int distT[DUNG_H][DUNG_W]; // tunneling

int arrsize = 0;

// compare two points in non-tunneling distance view
static int comparePointN(const void *pv, const void *qv)
{
	Point *p = (Point *) pv;
	Point *q = (Point *) qv;
	return distN[p->y][p->x] - distN[q->y][q->x];
}

// compare two points in tunneling distance view
static int comparePointT(const void *pv, const void *qv)
{
	Point *p = (Point *) pv;
	Point *q = (Point *) qv;
	return distT[p->y][p->x] - distT[q->y][q->x];
}

// hardness to weight
static int htow(int hardness)
{
	if (0 <= hardness && hardness <= 84)
		return 1;
	else if ( 85 <= hardness && hardness <= 170)
		return 2;
	else if (171 <= hardness && hardness <= 254)
		return 3;
	else
		return -1;
}

Point Dijkstra::next(int x, int y, int t)
{
	int i, closest = 1<<10;

	Point ns[8];
	Point::neighbors(ns, x, y);

	for (i=0; i<8; i++) {
		Point n = ns[i];

		int cost = t ?
			distT[n.y][n.x] + htow(dungeon->hmap[n.y][n.x]):
			distN[n.y][n.x] + htow(dungeon->hmap[n.y][n.x]);

		if (cost < closest) {
			x = n.x;
			y = n.y;
			closest = cost;
		}
	}
	return Point::getPoint(x, y);
}

int Dijkstra::print(int t)
{
	int r, c;

	for (r=0; r<DUNG_H; r++) {
		for (c=0; c<DUNG_W; c++) {
			int dist = (t ? distT[r][c] : distN[r][c]);

			// get char representation of distance
			char ch = -1;
			if (dist<10)
				ch = '0'+dist;
			else if (dist>=10 && dist<36)
				ch = 'a'+(dist-10);
			else if (dist>=36 && dist<62)
				ch = 'A'+(dist-36);

			if (ch<0)
				printf("%c", dungeon->tmap[r][c]);
			else
				printf("%c", ch);
		}
		printf("\n");
	}
	return 0;
}

int Dijkstra::run(int x, int y, int t)
{
	Point p[DUNG_H][DUNG_W];
	int alt, i, r, c, huge = 1<<10;

	for (r=0; r<DUNG_H; r++) {
		for (c=0; c<DUNG_W; c++) {
			// init distance to some huge number
			if (t)
				distT[r][c] = huge;
			else
				distN[r][c] = huge;

			p[r][c] = Point::getPoint(c, r);
		}
	}
	// destination should have distance 0
	distT[y][x] = 0;
	distN[y][x] = 0;

	Heap *h = new Heap(t ? comparePointT : comparePointN);
	h->insert(&p[y][x]);

	Point *min;

	while ((min = (Point *)h->extract())) {
		x = min->x;
		y = min->y;

		Point ns[8]; // neighbors		
		Point::neighbors(ns, x, y);

		// distance if path to destination begins with (x, y)			
		alt = htow(dungeon->hmap[y][x]) + (t ? distT[y][x] : distN[y][x]);

		for (i=0; i<8; i++) {
			Point n = ns[i]; // neighbor cell

			if (IMMUTABLE==dungeon->hmap[n.y][n.x] ||
					(!t && dungeon->hmap[n.y][n.x]>0)) continue;

			if (alt < (t ? distT[n.y][n.x] : distN[n.y][n.x])) {
				if (t)
					distT[n.y][n.x] = alt;
				else
					distN[n.y][n.x] = alt;

				h->insert(&p[n.y][n.x]);
			}
		}
	}

	delete h;

	return 0;
}
