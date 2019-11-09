/*
 * Queue.h
 *
 *  Created on: Jul 17, 2015
 *      Author: Darryl
 */

#ifndef _QUEUE_H
#define _QUEUE_H

#define Qsize(a) ( a->size-1 )


typedef struct _queue {
int size;
int depth;
char* buffer;
}queue;

queue* create_queue(int size);

void destroy_queue(queue* queue);

void push(queue* queue, char value);

char pull(queue* queue);

char peek(int pos);

#endif


