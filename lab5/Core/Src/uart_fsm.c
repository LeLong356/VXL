#include "uart_fsm.h"
#include "timer.h"
#include <stdio.h>

extern ADC_HandleTypeDef  hadc1;   /* defined in main.c by CubeMX */
extern UART_HandleTypeDef huart2;

/* ---------- receive by interrupt: character -> buffer ---------- */
static uint8_t temp = 0;                    /* the byte that the UART just received */
static uint8_t buffer[MAX_BUFFER_SIZE];
static volatile uint8_t index_buffer = 0;   /* write position (interrupt) */
static uint8_t parse_index = 0;             /* read position (command_parser_fsm) */
uint8_t buffer_flag = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        buffer[index_buffer++] = temp;
        if (index_buffer >= MAX_BUFFER_SIZE) index_buffer = 0;
        buffer_flag = 1;
        HAL_UART_Receive_IT(&huart2, &temp, 1);   /* ready for the next character */
    }
}

/* ---------- FSM 1: command parser ---------- */
enum PARSER_STATE { P_INIT, P_EXCL, P_R, P_S, P_T, P_O, P_K };
static enum PARSER_STATE parser_state = P_INIT;

static uint8_t command_RST = 0;   /* a full !RST# was received */
static uint8_t command_OK  = 0;   /* a full !OK#  was received */

static void parser_step(uint8_t c) {
    /* a '!' always starts a new command, even in the middle of a broken one */
    if (c == '!') { parser_state = P_EXCL; return; }

    switch (parser_state) {
    case P_INIT:
        break;
    case P_EXCL:
        if      (c == 'R') parser_state = P_R;
        else if (c == 'O') parser_state = P_O;
        else               parser_state = P_INIT;
        break;
    case P_R:
        parser_state = (c == 'S') ? P_S : P_INIT;
        break;
    case P_S:
        parser_state = (c == 'T') ? P_T : P_INIT;
        break;
    case P_T:
        if (c == '#') command_RST = 1;
        parser_state = P_INIT;
        break;
    case P_O:
        parser_state = (c == 'K') ? P_K : P_INIT;
        break;
    case P_K:
        if (c == '#') command_OK = 1;
        parser_state = P_INIT;
        break;
    default:
        parser_state = P_INIT;
        break;
    }
}

/* takes every new character of the buffer, so no character is lost
 * even if several arrive before the main loop comes back */
void command_parser_fsm(void) {
    uint8_t end = index_buffer;
    while (parse_index != end) {
        parser_step(buffer[parse_index]);
        parse_index++;
        if (parse_index >= MAX_BUFFER_SIZE) parse_index = 0;
    }
}

/* ---------- FSM 2: communication ---------- */
enum COMM_STATE { C_IDLE, C_WAIT_OK };
static enum COMM_STATE comm_state = C_IDLE;

static uint32_t ADC_VALUE = 0;   /* value of the last packet (kept when we resend) */

static void send_packet(void) {
    char str[32];
    int len = sprintf(str, "!ADC=%lu#\r\n", (unsigned long)ADC_VALUE);
    HAL_UART_Transmit(&huart2, (uint8_t *)str, (uint16_t)len, 1000);
}

void uart_communication_fsm(void) {
    switch (comm_state) {
    case C_IDLE:
        if (command_RST) {                     /* !RST# : read the sensor and answer at once */
            command_RST = 0;
            ADC_VALUE = HAL_ADC_GetValue(&hadc1);
            send_packet();
            SetTimer_0(3000);                  /* wait 3 s for !OK# */
            comm_state = C_WAIT_OK;
        }
        command_OK = 0;                        /* an !OK# with no request is ignored */
        break;

    case C_WAIT_OK:
        if (command_OK) {                      /* the user confirmed: done */
            command_OK = 0;
            comm_state = C_IDLE;
        } else if (FlagTimer_0()) {            /* 3 s without !OK# : send the same packet again */
            send_packet();
            SetTimer_0(3000);
        }
        command_RST = 0;                       /* a new !RST# while waiting is ignored */
        break;

    default:
        comm_state = C_IDLE;
        break;
    }
}

void uart_fsm_init(void) {
    index_buffer = parse_index = 0;
    buffer_flag = 0;
    command_RST = command_OK = 0;
    parser_state = P_INIT;
    comm_state = C_IDLE;
    HAL_UART_Receive_IT(&huart2, &temp, 1);    /* start receiving the first character */
}
