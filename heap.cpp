#include <stdio.h>
#include <stdlib.h>

#include "heap.h"

void Heap::swap(int i, int j)
{
	void *tmp = heap[i];
	heap[i] = heap[j];
	heap[j] = tmp;
}

void Heap::percolateUp(int index)
{
	while (index>0) {
		int p = (index-1)/2;
		if (comp(heap[p], heap[index]) <= 0)
			break;
		else {
			swap(p, index);
			index = p;
		}
	}
}

void Heap::percolateDown(int index)
{
	while (index<size/2) {
		int l = index*2+1;
		int r = index*2+2;
		int imin = index;
		if (l < size && comp(heap[l], heap[imin]) < 0)
			imin = l;
		if (r < size && comp(heap[r], heap[imin]) < 0)
			imin = r;
		if (comp(heap[index], heap[imin]) == 0)
			break;
		else {
			swap(index, imin);
			index = imin;
		}
	}
}

Heap::Heap(int (*compare)(const void*, const void*))
{
	comp = compare;
	heap = (void **)malloc(sizeof(void*) * HEAP_BUFFER_SIZE);
	size = 0;
}

Heap::~Heap()
{
	free(heap);
}

bool Heap::insert(void *e)
{
	if (size >= HEAP_BUFFER_SIZE) {
		fprintf(stderr, "Error: heap_insert: buffer overflow\n");
		return 1;
	}
	heap[size] = e;
	percolateUp(size);
	size++;
	return 0;
}

void* Heap::extract()
{
	if (size <= 0) {
		return NULL;
	}
	void *tmp = heap[0];
	swap(--size, 0);
	percolateDown(0);
	return tmp;
}

