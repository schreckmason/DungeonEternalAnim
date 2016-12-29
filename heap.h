#ifndef HEAP_H
#define HEAP_H

#define HEAP_BUFFER_SIZE (1<<10)

class Heap
{
	private:
		void **heap;
		void swap(int i, int j);
		void percolateUp(int index);
		void percolateDown(int index);

		int (*comp)(const void *, const void *);

	public:
		int size;

		Heap(int (*compare)(const void*, const void*));
		~Heap();

		bool insert(void *);
		void *extract();
};

#endif

