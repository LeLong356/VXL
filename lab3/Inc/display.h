#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "main.h"

// led_buffer[0..1] = road N (tens, units), led_buffer[2..3] = road D (tens, units)
extern uint8_t led_buffer[4];

void display7SEG(GPIO_TypeDef *ports[7], uint16_t pins[7], uint8_t num);
void update7SEG(int index);

#endif /* INC_DISPLAY_H_ */
