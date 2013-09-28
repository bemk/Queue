#ifndef __QUEUE_H
#define __QUEUE_H

#include "lock.h"

#ifdef cplusplus
extern "C" {
#endif

/* Buffer size has to be a multiple of 2, as we're going to do a bitmask on it
 * rather than a modulo to save on cpu cycles 
 * 
 * Also it shouldn't be too big, as it's just a temporary holding spot, to keep
 * give the data a place, before it can safely be flushed to the queue.
 */
#define BUFFER_SIZE (1<<4)

struct node {
	spinlock lock;
	void* data;
	struct node* next;
};

struct node_holder {
	spinlock lock; // Locked when non-empty, but work still is in progress
	spinlock complete; // Locked when non-empty, but completed
	struct node* node;
};

struct queue {
	struct node_holder buffer[BUFFER_SIZE];
	struct node* in;
	struct node* out;

	int idx;
	spinlock lock;
};

#ifdef cplusplus
}
#endif

#endif
