#include "timer.h"

#define TIME_CYCLE 10        /* ms per tick of TIM2 */

/* FIX: the counters are uint16_t. The reference code used uint8_t, and 3000/10 = 300
 * does not fit in 8 bits (max 255), so the "3 s" timer was really 440 ms. */
static uint16_t counter_timer_0 = 0;
static uint8_t  flag_timer_0    = 0;
static uint16_t counter_timer_1 = 0;
static uint8_t  flag_timer_1    = 0;

void SetTimer_0(uint16_t duration) {
    counter_timer_0 = duration / TIME_CYCLE;
    flag_timer_0 = 0;
}

void SetTimer_1(uint16_t duration) {
    counter_timer_1 = duration / TIME_CYCLE;
    flag_timer_1 = 0;
}

void RunSoftwareTimer(void) {
    if (counter_timer_0 > 0) {
        counter_timer_0--;
        if (counter_timer_0 == 0) flag_timer_0 = 1;
    }
    if (counter_timer_1 > 0) {
        counter_timer_1--;
        if (counter_timer_1 == 0) flag_timer_1 = 1;
    }
}

uint8_t FlagTimer_0(void) { return flag_timer_0; }
uint8_t FlagTimer_1(void) { return flag_timer_1; }

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        RunSoftwareTimer();
    }
}
