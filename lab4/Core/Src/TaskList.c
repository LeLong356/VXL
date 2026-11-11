#include "TaskList.h"

static uint32_t taskCount = 0;

static struct Node* head = NULL;

void TL_init(void) {
	TA_init();
	head = TA_malloc(); // create a virtual node

	taskCount = 0;
}

void TL_insert(void(*pFunc)(), unsigned int DELAY, unsigned int PERIOD) {
	struct Node* preTemp = head;
	struct Node* temp = head->next;

	uint32_t accumDelay = 0;
	while(temp && (accumDelay + (temp->data).Delay) <= DELAY) {
		accumDelay += (temp->data).Delay;

		preTemp = temp;
		temp = temp->next;
	}

	struct Node* newNode = TA_malloc();
	preTemp->next = newNode;
	newNode->next = temp;


	(newNode->data).Period = PERIOD;
	(newNode->data).RunMe = 0;
	(newNode->data).TaskID = (uint32_t)pFunc;
	(newNode->data).pTask = pFunc;
	(newNode->data).Delay = DELAY - accumDelay;

	if(temp) {
		(temp->data).Delay -= (newNode->data).Delay;
	}

	taskCount++;
}

void TL_insertNode(struct Node* node) {
	if(node == NULL) return;

	struct Node* preTemp = head;
	struct Node* temp = head->next;

	uint32_t accumDelay = 0;
	while(temp && (accumDelay + (temp->data).Delay) <= (node->data).Delay) {
		accumDelay += (temp->data).Delay;

		preTemp = temp;
		temp = temp->next;
	}

	preTemp->next = node;
	node->next = temp;

	(node->data).Delay -= accumDelay;
	if(temp) {
		(temp->data).Delay -= (node->data).Delay;
	}

	taskCount++;
}

uint8_t TL_removeID(uint32_t TaskID) {
	struct Node* preTemp = head;
	struct Node* temp = head->next;

	while(temp && (temp->data).TaskID != TaskID) {
		preTemp = temp;
		temp = temp->next;
	}

	if(temp == NULL) return 0;

	if(temp->next) {
		((temp->next)->data).Delay += (temp->data).Delay;
	}

	preTemp->next = temp->next;
	taskCount--;

	TA_free(temp);
	return 1;
}

struct Node* TL_removeFront(void) { // just remove node from current list in order to add it in again -> no need to free()
	if(taskCount == 0) return NULL;

	struct Node* delNode = head->next;
	head->next = delNode->next;

	taskCount--;
	return delNode;
}

STask* TL_getFront() {
	if(head && head->next)
		return &(head->next->data);
	return NULL;
}

void TL_tick(uint32_t ms) {
	struct Node* n = (head) ? head->next : NULL;
	while(n && (n->data).Delay == 0) {
		n = n->next;
	}
	if(n) {
		if((n->data).Delay > ms) (n->data).Delay -= ms;
		else (n->data).Delay = 0;
	}
}

uint32_t TL_size() {
	return taskCount;
}
