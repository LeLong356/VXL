#include "main.h"
#include "timer.h"
#include "display.h"
#include "traffic.h"

/* ---- pin map (from the schematic) ------------------------------------- */
// LEDs are active low: the anodes sit on +5V, the MCU pin is the cathode.
// (the 7-seg pins live in display.c)
#define LED_ON  GPIO_PIN_RESET
#define LED_OFF GPIO_PIN_SET

#define RED_N_PIN  GPIO_PIN_1
#define YEL_N_PIN  GPIO_PIN_2
#define GRN_N_PIN  GPIO_PIN_3
#define RED_D_PIN  GPIO_PIN_4
#define YEL_D_PIN  GPIO_PIN_5
#define GRN_D_PIN  GPIO_PIN_6

#define EN_ALL (GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10)

/* ---- timing (in ms, converted to ticks by the timer module) ------------ */
#define SECOND_MS 1000
#define SCAN_MS   100    // one digit per tick at 10 ms; with a slower TIM2 it becomes 1 tick

enum { COL_RED = 0, COL_AMBER = 1, COL_GREEN = 2, COL_OFF = 3 };

// default durations: red = green + amber  ->  10 = 7 + 3
static uint8_t durRed = 10, durAmber = 3, durGreen = 7;

static uint8_t curMode = MODE_NORMAL;
static uint8_t editVal = 0;

static uint8_t phase;        // 0: N green  1: N amber  2: D green  3: D amber
static uint8_t remaining;    // seconds left in the current phase
static uint8_t digitIdx;

static uint8_t valN, valD;   // the two numbers on the 7-seg displays

/* ---- LEDs ------------------------------------------------------------- */
static void set_road(uint16_t redPin, uint16_t yelPin, uint16_t grnPin, uint8_t col) {
    HAL_GPIO_WritePin(GPIOA, redPin, col == COL_RED   ? LED_ON : LED_OFF);
    HAL_GPIO_WritePin(GPIOA, yelPin, col == COL_AMBER ? LED_ON : LED_OFF);
    HAL_GPIO_WritePin(GPIOA, grnPin, col == COL_GREEN ? LED_ON : LED_OFF);
}

static void set_road_n(uint8_t col) { set_road(RED_N_PIN, YEL_N_PIN, GRN_N_PIN, col); }
static void set_road_d(uint8_t col) { set_road(RED_D_PIN, YEL_D_PIN, GRN_D_PIN, col); }

/* ---- display values --------------------------------------------------- */
static void refresh_values(void) {
    if (curMode == MODE_NORMAL) {
        switch (phase) {
            case 0:  valN = remaining;             valD = remaining + durAmber; break;
            case 1:  valN = remaining;             valD = remaining;            break;
            case 2:  valN = remaining + durAmber;  valD = remaining;            break;
            default: valN = remaining;             valD = remaining;            break;
        }
    } else {
        valN = curMode;     // mode number on the road-N pair
        valD = editVal;     // value being edited on the road-D pair
    }
    if (valN > 99) valN = 99;
    if (valD > 99) valD = 99;

    led_buffer[0] = valN / 10;
    led_buffer[1] = valN % 10;
    led_buffer[2] = valD / 10;
    led_buffer[3] = valD % 10;
}

/* ---- normal cycle ----------------------------------------------------- */
static void phase_start(uint8_t p) {
    phase = p;
    switch (p) {
        case 0: remaining = durGreen; set_road_n(COL_GREEN); set_road_d(COL_RED);   break;
        case 1: remaining = durAmber; set_road_n(COL_AMBER); set_road_d(COL_RED);   break;
        case 2: remaining = durGreen; set_road_n(COL_RED);   set_road_d(COL_GREEN); break;
        default:remaining = durAmber; set_road_n(COL_RED);   set_road_d(COL_AMBER); break;
    }
    refresh_values();
}

static void one_second_passed(void) {
    if (--remaining == 0) {
        phase_start((phase + 1) & 3);
    } else {
        refresh_values();
    }
}

/* ---- public API ------------------------------------------------------- */
void traffic_init(void) {
    HAL_GPIO_WritePin(GPIOA, EN_ALL, GPIO_PIN_SET);
    traffic_set_mode(MODE_NORMAL);
    setTimer(TIMER_SCAN, SCAN_MS);
}

void traffic_set_mode(uint8_t mode) {
    curMode = mode;
    if (mode == MODE_NORMAL) {
        setTimer(TIMER_SECOND, SECOND_MS);
        phase_start(0);                   // always restart the cycle
    } else {
        // while editing, both roads show the colour being edited
        uint8_t col = (mode == MODE_RED) ? COL_RED : (mode == MODE_AMBER) ? COL_AMBER : COL_GREEN;
        set_road_n(col);
        set_road_d(col);
        refresh_values();
    }
}

void traffic_set_edit_value(uint8_t value) {
    editVal = value;
    refresh_values();
}

void traffic_apply(uint8_t red, uint8_t amber, uint8_t green) {
    durRed = red;
    durAmber = amber;
    durGreen = green;
}

uint8_t traffic_get_red(void)   { return durRed; }
uint8_t traffic_get_amber(void) { return durAmber; }
uint8_t traffic_get_green(void) { return durGreen; }

void traffic_run(void) {
    if (curMode == MODE_NORMAL && isTimerExpired(TIMER_SECOND)) {
        setTimer(TIMER_SECOND, SECOND_MS);
        one_second_passed();
    }

    if (isTimerExpired(TIMER_SCAN)) {
        setTimer(TIMER_SCAN, SCAN_MS);
        update7SEG(digitIdx);
        digitIdx = (digitIdx + 1) & 3;
    }
}
