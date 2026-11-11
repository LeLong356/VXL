#include "Scheduler.h"

static uint32_t INTERRUPT_PERIOD = 0; // ms per timer interrupt
static volatile uint32_t sch_time_ms = 0;

void SCH_Init(TIM_HandleTypeDef *htim) {
	INTERRUPT_PERIOD = (htim->Instance->PSC+1)*(htim->Instance->ARR+1) / SCH_SYSCLK_FREQ;
	sch_time_ms = 0;
	TL_init();
}

uint32_t SCH_Add_Task(void(*pFunc)(), unsigned int DELAY, unsigned int PERIOD) {
	if(pFunc == NULL || TL_size() == SCH_MAX_TASKS) return 0;

	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	TL_insert(pFunc, DELAY, PERIOD);
	STask* first = TL_getFront();
	if(first && first->Delay == 0 && first->RunMe == 0) {
		first->RunMe = 1;
	}
	__set_PRIMASK(primask);
	return (uint32_t)pFunc;
}

uint8_t SCH_Delete_Task(uint32_t TaskID) {
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	uint8_t found = TL_removeID(TaskID);
	__set_PRIMASK(primask);

	return !found;
}

void SCH_Dispatch_Tasks(void) {
	STask* task =  TL_getFront();
	if(task && task->RunMe) {
		(*(task->pTask))();
		(task->RunMe)--;
		if(task->RunMe == 0) {
			uint32_t primask = __get_PRIMASK();
			__disable_irq();

			struct Node* delNode = TL_removeFront();
			if((delNode->data).Period) {
				(delNode->data).Delay = (delNode->data).Period;
				TL_insertNode(delNode);
			} else {
				TA_free(delNode); // one-shot: give the slot back to the pool
			}

			STask* next = TL_getFront();
			if(next && next->Delay == 0 && next->RunMe == 0) {
				next->RunMe = 1;
			}
			__set_PRIMASK(primask);
		}
	}
}

void SCH_Update(void) {
	sch_time_ms += INTERRUPT_PERIOD;

	TL_tick(INTERRUPT_PERIOD);

	STask* task = TL_getFront();
	if(task && task->Delay == 0 && task->RunMe == 0) {
		task->RunMe = 1;
	}
}

uint32_t SCH_Get_Time(void) {
	return sch_time_ms;
}
