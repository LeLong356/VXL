#include "main.h"
#include "input_reading.h"
#include "input_processing.h"
#include "timer.h"
#include "traffic.h"

#define BTN_MODE 0   // sw0
#define BTN_INC  1   // sw1
#define BTN_SET  2   // sw2

#define REPEAT_MS 100   // auto-increase every 100 ms once held for 1 s

/* ---- per-button FSM: turns the debounced level into press events ------ */
enum ButtonState {
    BUTTON_RELEASED,
    BUTTON_PRESSED,
    BUTTON_PRESSED_MORE_THAN_1_SECOND
};

static enum ButtonState buttonState[N0_OF_BUTTONS];
static uint8_t pressEvent[N0_OF_BUTTONS];

static void button_fsm(uint8_t i) {
    switch (buttonState[i]) {
        case BUTTON_RELEASED:
            if (is_button_pressed(i)) {
                buttonState[i] = BUTTON_PRESSED;
                pressEvent[i] = 1;      // consumed by the mode FSM below
                // INCREASE VALUE OF PORT A BY ONE UNIT
                // (here: sw1's press event adds 1 to the value being edited)
            }
            break;

        case BUTTON_PRESSED:
            if (!is_button_pressed(i)) {
                buttonState[i] = BUTTON_RELEASED;
            } else {
                if (is_button_pressed_1s(i)) {
                    buttonState[i] = BUTTON_PRESSED_MORE_THAN_1_SECOND;
                    if (i == BTN_INC) setTimer(TIMER_REPEAT, REPEAT_MS);
                }
            }
            break;

        case BUTTON_PRESSED_MORE_THAN_1_SECOND:
            if (!is_button_pressed(i)) {
                buttonState[i] = BUTTON_RELEASED;
            }
            // todo
            // (done: sw1 repeats its press event every REPEAT_MS, the others do not)
            else if (i == BTN_INC && isTimerExpired(TIMER_REPEAT)) {
                setTimer(TIMER_REPEAT, REPEAT_MS);
                pressEvent[i] = 1;
            }
            break;
    }
}

static uint8_t take_event(uint8_t i) {
    uint8_t e = pressEvent[i];
    pressEvent[i] = 0;
    return e;
}

/* ---- mode FSM (the state diagram) ------------------------------------- */
static uint8_t mode = MODE_NORMAL;
static uint8_t tempVal;                              // value being edited
static uint8_t dirty;                                // sw1 used, sw2 not yet pressed
static uint8_t pendRed, pendAmber, pendGreen;        // locked-in, not yet applied

static uint8_t *pending_of(uint8_t m) {
    return (m == MODE_RED) ? &pendRed : (m == MODE_AMBER) ? &pendAmber : &pendGreen;
}

static void enter_edit_mode(uint8_t m) {
    mode = m;
    dirty = 0;
    tempVal = *pending_of(m);
    traffic_set_mode(m);
    traffic_set_edit_value(tempVal);
}

static void leave_edit_mode(void) {
    // red must equal green + amber, otherwise the whole set is discarded
    if (pendRed == (uint8_t)(pendGreen + pendAmber)) {
        traffic_apply(pendRed, pendAmber, pendGreen);
    }
    mode = MODE_NORMAL;
    traffic_set_mode(MODE_NORMAL);
}

void fsm_for_input_processing(void) {
    for (uint8_t i = 0; i < N0_OF_BUTTONS; i++) {
        button_fsm(i);
    }
    uint8_t b0 = take_event(BTN_MODE);
    uint8_t b1 = take_event(BTN_INC);
    uint8_t b2 = take_event(BTN_SET);

    switch (mode) {
        case MODE_NORMAL:
            if (b0) {
                pendRed = traffic_get_red();
                pendAmber = traffic_get_amber();
                pendGreen = traffic_get_green();
                enter_edit_mode(MODE_RED);
            }
            break;

        case MODE_RED:
        case MODE_AMBER:
        case MODE_GREEN:
            if (b1) {                                   // value++ in [1;99]
                tempVal = (tempVal >= 99) ? 1 : (uint8_t)(tempVal + 1);
                dirty = 1;
                traffic_set_edit_value(tempVal);
            }
            if (b2) {                                   // lock the value in
                *pending_of(mode) = tempVal;
                dirty = 0;
            }
            // b0 cycles only if sw1 was not used, or sw2 locked it in (b2 && b0)
            if (b0 && !dirty) {
                if (mode == MODE_GREEN) leave_edit_mode();
                else enter_edit_mode((uint8_t)(mode + 1));
            }
            break;
    }
}
