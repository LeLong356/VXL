#ifndef INC_TRAFFIC_H_
#define INC_TRAFFIC_H_

#include <stdint.h>

#define MODE_NORMAL 1
#define MODE_RED    2
#define MODE_AMBER  3
#define MODE_GREEN  4

void traffic_init(void);
void traffic_run(void);                       // call from the main loop
void traffic_set_mode(uint8_t mode);          // 1 = normal, 2..4 = editing a colour
void traffic_set_edit_value(uint8_t value);   // value shown while editing
void traffic_apply(uint8_t red, uint8_t amber, uint8_t green);

uint8_t traffic_get_red(void);
uint8_t traffic_get_amber(void);
uint8_t traffic_get_green(void);

#endif /* INC_TRAFFIC_H_ */
