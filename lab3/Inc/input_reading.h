#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

#include "main.h"

#define N0_OF_BUTTONS 3

void button_init(void);
void button_reading(void);                       // call every TIMER_CYCLE_MS from TIM2
unsigned char is_button_pressed(uint8_t index);
unsigned char is_button_pressed_1s(unsigned char index);

#endif /* INC_INPUT_READING_H_ */
