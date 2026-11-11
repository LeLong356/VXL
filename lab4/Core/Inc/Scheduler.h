#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "main.h"
#include <stdlib.h>
#include "TaskList.h"
#include "SCH_Config.h"

void SCH_Init(TIM_HandleTypeDef *htim);
uint32_t SCH_Add_Task(void(*pFunc)(), unsigned int DELAY, unsigned int PERIOD); // returns 0 on failure
uint8_t SCH_Delete_Task(uint32_t TaskID); // 0 = OK, 1 = not found
void SCH_Dispatch_Tasks(void);
void SCH_Update(void);
uint32_t SCH_Get_Time(void); // ms since SCH_Init

#endif /* INC_SCHEDULER_H_ */
