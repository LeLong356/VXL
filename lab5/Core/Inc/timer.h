#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "main.h"

/* Software timers. The hardware timer TIM2 must tick every 10 ms
 * (Prescaler 7999, Counter Period 9 with an 8 MHz clock).
 * duration is in ms. */
void SetTimer_0(uint16_t duration);
void SetTimer_1(uint16_t duration);

uint8_t FlagTimer_0(void);   /* 1 when the time is over */
uint8_t FlagTimer_1(void);

void RunSoftwareTimer(void); /* called every 10 ms by the TIM2 interrupt */

#endif /* INC_TIMER_H_ */
