#ifndef INC_TASKALLOCATE_H_
#define INC_TASKALLOCATE_H_

#include "main.h"
#include "SCH_Config.h"

#define MAX_NODE (SCH_MAX_TASKS+1) // extra slot for virtual node

typedef struct {
	void(*pTask)(void);
	uint32_t Delay;
	uint32_t Period;
	uint8_t RunMe;
	uint32_t TaskID;
} STask;

struct Node {
	STask data;
	struct Node* next;
};

void TA_init(void);
struct Node* TA_malloc(void);
void TA_free(struct Node* node);
uint32_t TA_remaining(void);

#endif /* INC_TASKALLOCATE_H_ */
