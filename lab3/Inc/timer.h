#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include <stdint.h>

// TIM2 interrupt period in ms. It MUST match the CubeMX setup:
// 8 MHz / (Prescaler 7999 + 1) / (Period 9 + 1) = 100 Hz = 10 ms.
// This is the only place to edit if the TIM2 period is ever changed
// (every duration below is given in ms and converted to ticks here).
#define TIMER_CYCLE_MS 10

// software timers, all driven by the TIM2 interrupt (nothing blocks)
#define TIMER_SECOND 0   // 1 s step of the traffic light countdown
#define TIMER_SCAN   1   // time between two 7-seg digits
#define TIMER_REPEAT 2   // auto-increase while sw1 is held
#define MAX_TIMERS   3

void setTimer(uint8_t index, uint32_t duration_ms);
uint8_t isTimerExpired(uint8_t index);   // 1 once after expiry, then clears
void timerRun(void);                     // called from the TIM2 interrupt

#endif /* INC_TIMER_H_ */
