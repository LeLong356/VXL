#include "main.h"
#include "display.h"

uint8_t led_buffer[4] = {0, 0, 0, 0};

// segments a..g. Road N uses PB0..PB6, road D uses PB7..PB13
// (the two digits of one road share the same 7 segment pins)
static GPIO_TypeDef *segPortsN[7] = {GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB};
static uint16_t segPinsN[7] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
                               GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6};
static GPIO_TypeDef *segPortsD[7] = {GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB};
static uint16_t segPinsD[7] = {GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10,
                               GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13};

void display7SEG(GPIO_TypeDef *ports[7], uint16_t pins[7], uint8_t num)
{
    uint8_t seg[10][7] =
    {
        {0,0,0,0,0,0,1}, // 0
        {1,0,0,1,1,1,1}, // 1
        {0,0,1,0,0,1,0}, // 2
        {0,0,0,0,1,1,0}, // 3
        {1,0,0,1,1,0,0}, // 4
        {0,1,0,0,1,0,0}, // 5
        {0,1,0,0,0,0,0}, // 6
        {0,0,0,1,1,1,1}, // 7
        {0,0,0,0,0,0,0}, // 8
        {0,0,0,0,1,0,0}  // 9
    };

    if (num > 9)
        return;

    for (int i = 0; i < 7; i++)
    {
        HAL_GPIO_WritePin(ports[i], pins[i], seg[num][i]);
    }
}

// the enables en0..en3 are PA7..PA10 on this schematic
void update7SEG(int index)
{
    /* Turn all displays off */
    HAL_GPIO_WritePin(GPIOA,
                      GPIO_PIN_7 | GPIO_PIN_8 |
                      GPIO_PIN_9 | GPIO_PIN_10,
                      GPIO_PIN_SET);

    switch (index)
    {
        case 0:
            display7SEG(segPortsN, segPinsN, led_buffer[0]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
            break;

        case 1:
            display7SEG(segPortsN, segPinsN, led_buffer[1]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
            break;

        case 2:
            display7SEG(segPortsD, segPinsD, led_buffer[2]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
            break;

        case 3:
            display7SEG(segPortsD, segPinsD, led_buffer[3]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
            break;

        default:
            break;
    }
}
