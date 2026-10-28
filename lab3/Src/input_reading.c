#include "main.h"
#include "input_reading.h"
#include "timer.h"

// timer interrupt duration is 10ms, so to pass 1 second,
// we need to jump to the interrupt service routine 100 times
// (written as 1000 ms / TIMER_CYCLE_MS so it follows the TIM2 period)
#define DURATION_FOR_AUTO_INCREASING (1000 / TIMER_CYCLE_MS)

#define BUTTON_IS_PRESSED  GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

// the three buttons from the schematic (pull-up inputs, pressed = low)
static GPIO_TypeDef *const buttonPort[N0_OF_BUTTONS] = {GPIOA, GPIOA, GPIOA};
static const uint16_t buttonPin[N0_OF_BUTTONS] = {GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13};

// the buffer that the final result is stored after debouncing
static GPIO_PinState buttonBuffer[N0_OF_BUTTONS];

// we define two buffers for debouncing
static GPIO_PinState debounceButtonBuffer1[N0_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer2[N0_OF_BUTTONS];

// we define a flag for a button pressed more than 1 second
static uint8_t flagForButtonPress1s[N0_OF_BUTTONS];

// we define a counter for automatically increasing the value
// after the button is pressed more than 1 second
static uint16_t counterForButtonPress1s[N0_OF_BUTTONS];

// zero-initialised buffers would read as "pressed", so start released
void button_init(void) {
    for (uint8_t i = 0; i < N0_OF_BUTTONS; i++) {
        buttonBuffer[i] = BUTTON_IS_RELEASED;
        debounceButtonBuffer1[i] = BUTTON_IS_RELEASED;
        debounceButtonBuffer2[i] = BUTTON_IS_RELEASED;
        flagForButtonPress1s[i] = 0;
        counterForButtonPress1s[i] = 0;
    }
}

unsigned char is_button_pressed(uint8_t index) {
    if (index >= N0_OF_BUTTONS) return 0;
    return (buttonBuffer[index] == BUTTON_IS_PRESSED);
}

unsigned char is_button_pressed_1s(unsigned char index) {
    if (index >= N0_OF_BUTTONS) return 0;
    return (flagForButtonPress1s[index] == 1);
}

void button_reading(void) {
    for (uint8_t i = 0; i < N0_OF_BUTTONS; i++) {
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(buttonPort[i], buttonPin[i]);

        if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i]) {
            buttonBuffer[i] = debounceButtonBuffer1[i];
        }

        if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
            // if a button is pressed, we start counting
            if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                counterForButtonPress1s[i]++;
            } else {
                // the flag is turned on when 1 second has passed
                // since the button is pressed
                flagForButtonPress1s[i] = 1;
                // to do
            }
        } else {
            counterForButtonPress1s[i] = 0;
            flagForButtonPress1s[i] = 0;
        }
    }
}
