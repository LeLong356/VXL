#include "TaskAllocate.h"

static struct Node slot[MAX_NODE];

static uint32_t remain = MAX_NODE;

static struct Node* head = NULL;

void TA_init(void) {
	for(int i = 0; i < MAX_NODE; i++) {
		slot[i].data.Delay = 0;
		slot[i].data.Period = 0;
		slot[i].data.RunMe = 0;
		slot[i].data.TaskID = 0;
		slot[i].data.pTask = NULL;
		slot[i].next = NULL;

		if(i == 0) {
			head = &(slot[0]);
		}
		else {
			slot[i-1].next = &(slot[i]);
		}
	}
}

struct Node* TA_malloc(void) {
	if(remain == 0) return NULL;

	struct Node* temp = head;
	head = head->next;

	remain--;
	return temp;
}

void TA_free(struct Node* node) {
	if(node == NULL) return;

	(node->data).Delay = 0;
	(node->data).Period = 0;
	(node->data).RunMe = 0;
	(node->data).TaskID = 0;
	(node->data).pTask = NULL;
	node->next = NULL;

	node->next = head;
	head = node;

	remain++;
}

uint32_t TA_remaining(void) {
	return remain;
}
