#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "alloc.h"

#define TRY_COUNT_COOKIE 4

/*
 * Because queue_buffer_next_idx is only a heuristic value, atomic constraints
 * don't have to be too strict. Because of this, this function might return,
 * from time to time, a value you didn't necessarily expect.
 *
 * The values will remain within the constraints of the array though because of
 * calculations made after the modification of the value.
 */
int queue_buffer_next_idx(struct queue* q)
{
	int n = q->idx++;

	n &= (BUFFER_SIZE-1); // Is what keeps the return value within bounds.

	return n;
}

int queue_flush_buffer(struct queue* q)
{
	/*
 	 * 1. Try to lock queue, if not successful, return success
	 * 2. find all completed nodes
	 * 3. Write nodes to main queue, freeing node locks one by one
	 * 4. Release main queue
	 * 5. Return success
	 */
	return 0;
}

int queue_add_node(struct queue* q, struct node* n)
{
	int idx = 0;
	int buffer_idx = 0;
	for (; idx < TRY_COUNT_COOKIE; idx++)
	{
		int i = 0;
		for (; i < TRY_COUNT_COOKIE; i++)
		{
			buffer_idx = queue_buffer_next_idx(q);
			int ret = spinlock_try(&(q->buffer[buffer_idx].lock));
			if (ret == MUTEX_ACQUIRED)
				goto success;
		}
		queue_flush_buffer(q);
	}
	return -1;
success:
	q->buffer[buffer_idx].node = n;
	spinlock_lock(&(q->buffer[buffer_idx].complete));
	queue_flush_buffer(q);
	return 0;
}

struct node* queue_get_node(struct queue* q)
{
	if (q->out == NULL)
		queue_flush_buffer(q);

	int lock = spinlock_try(&(q->lock));
	if (lock != MUTEX_ACQUIRED)
		return NULL;

	struct node* n = q->out;
	if (q->in == q->out)
	{
		q->in = NULL;
		q->out = NULL;
	}
	q->out = q->out->next;

	spinlock_free(&(q->lock));

	return NULL;
}

struct queue* queue_new()
{
	struct queue* q = b_alloc(sizeof(*q));
	if (q == NULL)
		return q;

	memset(q, 0, sizeof(*q));
	return q;
}

int queue_add(struct queue* q, void* data)
{
	if (q == NULL || data == NULL)
		return -1;

	struct node* n = b_alloc(sizeof(*n));
	if (n == NULL)
		return -1;

	memset(n, 0, sizeof(*n));
	n->data = data;

	return queue_add_node(q, n);
}

void* queue_get(struct queue* q)
{
	if (q == NULL)
		return NULL;
	struct node* n = queue_get_node(q);
	if (n == NULL)
		return n;
	return n->data;
}

