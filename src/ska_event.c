//
// sk_app - Event queue implementation

#include "ska_internal.h"

void ska_event_queue_init(ska_event_queue_t* queue) {
	memset(queue, 0, sizeof(*queue));
}

bool ska_event_queue_push(ska_event_queue_t* queue, const ska_event_t* event) {
	if (queue->count >= SKA_EVENT_QUEUE_SIZE) {
		return false;
	}

	queue->events[queue->write_pos] = *event;
	queue->write_pos = (queue->write_pos + 1) % SKA_EVENT_QUEUE_SIZE;
	queue->count++;
	return true;
}

bool ska_event_queue_pop(ska_event_queue_t* queue, ska_event_t* event) {
	if (queue->count == 0) {
		return false;
	}

	*event = queue->events[queue->read_pos];
	queue->read_pos = (queue->read_pos + 1) % SKA_EVENT_QUEUE_SIZE;
	queue->count--;
	return true;
}

bool ska_event_queue_is_empty(const ska_event_queue_t* queue) {
	return queue->count == 0;
}

void ska_event_queue_clear(ska_event_queue_t* queue) {
	queue->read_pos = 0;
	queue->write_pos = 0;
	queue->count = 0;
}
