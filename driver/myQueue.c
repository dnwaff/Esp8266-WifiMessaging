#include "myQueue.h"
#include "osapi.h"


queue* create_queue(int size){

	queue* q = (queue *)  os_zalloc(sizeof(queue));
	q->buffer = (char *) os_zalloc(size);
	q->size = size;
	q->depth = 0;

	int i ;
	for ( i = 0; i < size - 1; i++){
		q->buffer[i]= ' ';
	}

	return q;

};

void ICACHE_FLASH_ATTR
push(queue* queue, char value){

	os_memmove(queue->buffer, queue->buffer+1, Qsize(queue) - 1);

	queue->buffer[Qsize(queue) - 1] = value;

	if (queue->depth != queue->size) queue->depth--;

};

void ICACHE_FLASH_ATTR
destroy_queue(queue * queue){
	os_free(queue->buffer);
	os_free(queue);
}


