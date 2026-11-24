#ifndef INC_UART_FSM_H_
#define INC_UART_FSM_H_

#include "main.h"

#define MAX_BUFFER_SIZE 30

/* the LED of the lab: PA5 (change here if your pin has another name) */
#define LED_PORT GPIOA
#define LED_PIN  GPIO_PIN_5

void uart_fsm_init(void);          /* reset both FSMs and start receiving by interrupt */
void command_parser_fsm(void);     /* FSM 1: find !RST# and !OK# in the buffer */
void uart_communication_fsm(void); /* FSM 2: the protocol (send !ADC=..#, wait !OK#, 3 s resend) */

extern uint8_t buffer_flag;        /* 1 = a new character is in the buffer */

#endif /* INC_UART_FSM_H_ */
