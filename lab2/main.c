/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Lab 2 - Timer Interrupt and LED Scanning
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* Moi bai tap gom 3 "hook". Hook nao khong dung thi de ham rong. */
typedef struct
{
  void (*setup)(void);   /* chay 1 lan, TRUOC khi bat timer               */
  void (*isr)(void);     /* chay moi 10ms trong ngat TIM2                 */
  void (*loop)(void);    /* chay lap lai trong while(1) cua main          */
} Exercise_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* ============================================================
 *  CHON BAI MUON CHAY O DAY (1 -> 10), roi Build (Ctrl+B) lai
 * ============================================================ */
#define ACTIVE_EXERCISE   9

#if (ACTIVE_EXERCISE < 1) || (ACTIVE_EXERCISE > 10)
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* Ham dung chung */
void display7SEG(int num);
static void enableOnly(int idx);
void update7SEG(int index);
void updateClockBuffer(void);
void setTimer0(int duration);
void setTimer1(int duration);
void setTimer2(int duration);
void timer_run(void);

/* Bai 9, 10: LED matrix va ham phu cho vong lap main */
void updateLEDMatrix(int index);
static void enableMatrixColumn(int idx);
static void displayMatrixColumn(int col);
static int  matrixScanStep(void);
static void clockTick(void);
static void scan7SEGStep(void);

/* Hook cua tung bai */
void ex1_setup(void);  void ex1_isr(void);  void ex1_loop(void);
void ex2_setup(void);  void ex2_isr(void);  void ex2_loop(void);
void ex3_setup(void);  void ex3_isr(void);  void ex3_loop(void);
void ex4_setup(void);  void ex4_isr(void);  void ex4_loop(void);
void ex5_setup(void);  void ex5_isr(void);  void ex5_loop(void);
void ex6_setup(void);  void ex6_isr(void);  void ex6_loop(void);
void ex7_setup(void);  void ex7_isr(void);  void ex7_loop(void);
void ex8_setup(void);  void ex8_isr(void);  void ex8_loop(void);
void ex9_setup(void);  void ex9_isr(void);  void ex9_loop(void);
void ex10_setup(void); void ex10_isr(void); void ex10_loop(void);
void ex11_setup(void); void ex11_isr(void); void ex11_loop(void);   /* kiem tra LED matrix */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* ---------------- Chan dieu khien (doc theo hinh trong de) ----------------
 *  SEG0..SEG6 (a..g) : PB0..PB6   (7SEG-COM-ANODE: 0 = sang, 1 = tat)
 *  EN0..EN3          : PA6..PA9   (qua PNP: 0 = bat LED 7 doan, 1 = tat)
 *  DOT (2 LED giua)  : PA4
 *  LED do D1         : PA5
 * ------------------------------------------------------------------------ */
static const uint16_t segPins[7] = {
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6
};
static const uint16_t enPins[4] = {
    GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9
};
#define DOT_PIN   GPIO_PIN_4
#define LED_PIN   GPIO_PIN_5

static const uint8_t segTable[10][7] = {
    /*        a  b  c  d  e  f  g */
    /* 0 */ { 1, 1, 1, 1, 1, 1, 0 },
    /* 1 */ { 0, 1, 1, 0, 0, 0, 0 },
    /* 2 */ { 1, 1, 0, 1, 1, 0, 1 },
    /* 3 */ { 1, 1, 1, 1, 0, 0, 1 },
    /* 4 */ { 0, 1, 1, 0, 0, 1, 1 },
    /* 5 */ { 1, 0, 1, 1, 0, 1, 1 },
    /* 6 */ { 1, 0, 1, 1, 1, 1, 1 },
    /* 7 */ { 1, 1, 1, 0, 0, 0, 0 },
    /* 8 */ { 1, 1, 1, 1, 1, 1, 1 },
    /* 9 */ { 1, 1, 1, 1, 0, 1, 1 },
};

/* ---------------- Bo dem cho Bai 3 -> 8 (khung cua de) ------------------ */
const int MAX_LED = 4;
int index_led = 0;
int led_buffer[4] = {1, 2, 3, 4};

/* Gio hien tai cho Bai 5 -> 8 */
int hour = 15, minute = 8, second = 50;

/* ---------------- LED matrix (Bai 9, 10) ---------------------------------
 *  ENM0..ENM7 : PA2, PA3, PA10..PA15  (qua ULN2803, noi dat 1 cot)
 *  ROW0..ROW7 : PB8..PB15
 *  Moi phan tu cua matrix_buffer la du lieu cua 1 COT: bit r = 1 => LED o
 *  hang r cua cot do sang.
 *  Neu ma tran khong sang hoac sang nguoc -> doi muc 4 macro duoi day.
 * ------------------------------------------------------------------------ */
/* Theo so do: moi cot duoc keo len +3.3V qua dien tro 100 ohm (R5..R12), con
   ULN2803 keo cot do xuong dat de TAT cot  =>  ENM = 0 la BAT cot (cot co dien).
   Hang la phia cathode: hang xuong 0V thi LED o giao diem do sang.            */
#define MATRIX_ENM_ON    GPIO_PIN_RESET
#define MATRIX_ENM_OFF   GPIO_PIN_SET
#define MATRIX_ROW_ON    GPIO_PIN_RESET
#define MATRIX_ROW_OFF   GPIO_PIN_SET

static const uint16_t enmPins[8] = {
    GPIO_PIN_2,  GPIO_PIN_3,  GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15
};
static const uint16_t rowPins[8] = {
    GPIO_PIN_8,  GPIO_PIN_9,  GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15
};

const int MAX_LED_MATRIX = 8;
int index_led_matrix = 0;
uint8_t matrix_buffer[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

/* ---------------- Software timer (Bai 6 tro di) ------------------------- */
/* volatile vi bien duoc sua trong ngat va doc trong main */
volatile int timer0_counter = 0;
volatile int timer0_flag = 0;
int TIMER_CYCLE = 10;          /* chu ky ngat TIM2 = 10ms */

void setTimer0(int duration)
{
  timer0_counter = duration / TIMER_CYCLE;
  timer0_flag = 0;
}

/* Timer thu hai - dung o Bai 8 de quet LED 7 doan trong main */
volatile int timer1_counter = 0;
volatile int timer1_flag = 0;

void setTimer1(int duration)
{
  timer1_counter = duration / TIMER_CYCLE;
  timer1_flag = 0;
}

/* Timer thu ba - dung o Bai 9, 10 de quet LED matrix */
volatile int timer2_counter = 0;
volatile int timer2_flag = 0;

void setTimer2(int duration)
{
  timer2_counter = duration / TIMER_CYCLE;
  timer2_flag = 0;
}

void timer_run(void)
{
  if (timer0_counter > 0)
  {
    timer0_counter--;
    if (timer0_counter == 0) timer0_flag = 1;
  }
  if (timer1_counter > 0)
  {
    timer1_counter--;
    if (timer1_counter == 0) timer1_flag = 1;
  }
  if (timer2_counter > 0)
  {
    timer2_counter--;
    if (timer2_counter == 0) timer2_flag = 1;
  }
}

/* ---------------- Bang chon bai ----------------------------------------- */
static const Exercise_t exercises[12] = {
  { 0,          0,         0          },   /* 0: khong dung */
  { ex1_setup,  ex1_isr,   ex1_loop   },
  { ex2_setup,  ex2_isr,   ex2_loop   },
  { ex3_setup,  ex3_isr,   ex3_loop   },
  { ex4_setup,  ex4_isr,   ex4_loop   },
  { ex5_setup,  ex5_isr,   ex5_loop   },
  { ex6_setup,  ex6_isr,   ex6_loop   },
  { ex7_setup,  ex7_isr,   ex7_loop   },
  { ex8_setup,  ex8_isr,   ex8_loop   },
  { ex9_setup,  ex9_isr,   ex9_loop   },
  { ex10_setup, ex10_isr,  ex10_loop  },
  { ex11_setup, ex11_isr,  ex11_loop  },   /* 11: kiem tra LED matrix */
};

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  /* Khoi tao bai dang chon, roi moi bat timer de ngat khong chay som */
  if (exercises[ACTIVE_EXERCISE].setup) exercises[ACTIVE_EXERCISE].setup();
  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (exercises[ACTIVE_EXERCISE].loop) exercises[ACTIVE_EXERCISE].loop();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  *        8MHz / (7999+1) = 1kHz, dem 0..9 => ngat 100Hz = 10ms
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  *        PA4..PA9 : DOT, LED do, EN0..EN3
  *        PB0..PB6 : SEG0..SEG6
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_SET);

  /*Configure GPIO pins : PA4 PA5 PA6 PA7 PA8 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3 PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* LED matrix (bai 9, 10): ENM0..7 = PA2 PA3 PA10..PA15, ROW0..7 = PB8..PB15 */
  /* Muc ban dau = TAT ma tran (ENM = 1: cot bi keo xuong dat, ROW = 1: khong dan) */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,
                    GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,
                    GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* ========================================================================
 *  HAM DUNG CHUNG
 * ===================================================================== */

/* Hien so 0..9 len cac chan SEG (PB0..PB6) - COM-ANODE nen 0 = sang */
void display7SEG(int num)
{
  if (num < 0 || num > 9) return;
  for (int i = 0; i < 7; i++)
  {
    HAL_GPIO_WritePin(GPIOB, segPins[i],
                      segTable[num][i] ? GPIO_PIN_RESET : GPIO_PIN_SET);
  }
}

/* Chi bat LED 7 doan thu idx (0..3); idx = -1 la tat het.
   EN di qua PNP nen muc 0 = bat, muc 1 = tat. */
static void enableOnly(int idx)
{
  for (int i = 0; i < 4; i++)
  {
    HAL_GPIO_WritePin(GPIOA, enPins[i], (i == idx) ? GPIO_PIN_RESET : GPIO_PIN_SET);
  }
}

/* Bai 3: hien led_buffer[index] len LED 7 doan thu index */
void update7SEG(int index)
{
  switch (index)
  {
    case 0:
      // Display the first 7 SEG with led_buffer[0]
      enableOnly(-1);                  /* tat het truoc khi doi so */
      display7SEG(led_buffer[0]);
      enableOnly(0);
      break;
    case 1:
      // Display the second 7 SEG with led_buffer[1]
      enableOnly(-1);
      display7SEG(led_buffer[1]);
      enableOnly(1);
      break;
    case 2:
      // Display the third 7 SEG with led_buffer[2]
      enableOnly(-1);
      display7SEG(led_buffer[2]);
      enableOnly(2);
      break;
    case 3:
      // Display the forth 7 SEG with led_buffer[3]
      enableOnly(-1);
      display7SEG(led_buffer[3]);
      enableOnly(3);
      break;
    default:
      break;
  }
}

/* Bai 5: doi hour/minute thanh 4 chu so trong led_buffer (them so 0 neu 1 chu so) */
void updateClockBuffer(void)
{
  led_buffer[0] = hour / 10;       /* so 1 chu so thi chu so dau tu thanh 0 */
  led_buffer[1] = hour % 10;
  led_buffer[2] = minute / 10;
  led_buffer[3] = minute % 10;
}

/* ------------------------------ LED matrix ------------------------------ */

/* Chi bat cot idx (0..7); idx = -1 la tat het cac cot */
static void enableMatrixColumn(int idx)
{
  for (int i = 0; i < 8; i++)
  {
    HAL_GPIO_WritePin(GPIOA, enmPins[i], (i == idx) ? MATRIX_ENM_ON : MATRIX_ENM_OFF);
  }
}

/* Dua du lieu matrix_buffer[col] ra 8 hang roi bat cot col */
static void displayMatrixColumn(int col)
{
  enableMatrixColumn(-1);                       /* tat het cot truoc, tranh bong mo */
  for (int r = 0; r < 8; r++)
  {
    HAL_GPIO_WritePin(GPIOB, rowPins[r],
                      ((matrix_buffer[col] >> r) & 1) ? MATRIX_ROW_ON : MATRIX_ROW_OFF);
  }
  enableMatrixColumn(col);
}

void updateLEDMatrix(int index)
{
  switch (index)
  {
    case 0: displayMatrixColumn(0); break;
    case 1: displayMatrixColumn(1); break;
    case 2: displayMatrixColumn(2); break;
    case 3: displayMatrixColumn(3); break;
    case 4: displayMatrixColumn(4); break;
    case 5: displayMatrixColumn(5); break;
    case 6: displayMatrixColumn(6); break;
    case 7: displayMatrixColumn(7); break;
    default: break;
  }
}

/* Quet 1 cot, tra ve 1 khi vua quet xong cot cuoi (het 1 khung hinh) */
static int matrixScanStep(void)
{
  updateLEDMatrix(index_led_matrix);
  index_led_matrix = (index_led_matrix + 1) % MAX_LED_MATRIX;
  return (index_led_matrix == 0);
}

/* ---------------- Ham phu cho Bai 9, 10 ---------------- */

/* Moi 1s: dao DOT, tang gio:phut:giay, cap nhat led_buffer */
static void clockTick(void)
{
  HAL_GPIO_TogglePin(GPIOA, DOT_PIN);
  second++;
  if (second >= 60) { second = 0; minute++; }
  if (minute >= 60) { minute = 0; hour++; }
  if (hour >= 24)   { hour = 0; }
  updateClockBuffer();
}

/* Quet LED 7 doan tiep theo */
static void scan7SEGStep(void)
{
  if (index_led >= MAX_LED) index_led = 0;
  update7SEG(index_led++);
}

/* ========================================================================
 *  NGAT TIMER 2 - 10ms/lan. Chi chuyen tiep sang hook isr cua bai dang chon.
 *  (Khi viet bao cao "code trong HAL_TIM_PeriodElapsedCallback" thi dan
 *   noi dung ham exN_isr cua bai do vao.)
 * ===================================================================== */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance != TIM2) return;

  timer_run();   /* tick cho software timer (Bai 6+), khong anh huong bai khac */

  if (exercises[ACTIVE_EXERCISE].isr) exercises[ACTIVE_EXERCISE].isr();
}

/* ========================================================================
 *  BAI 1: LED 7 doan thu nhat hien "1", thu hai hien "2", doi moi 500ms
 * ===================================================================== */
static int ex1_counter;
static int ex1_state;

void ex1_setup(void)
{
  ex1_counter = 50;            /* 50 x 10ms = 500ms */
  ex1_state   = 1;
  enableOnly(-1);
  display7SEG(1);
  enableOnly(0);               /* bat dau: LED thu nhat hien so 1 */
}

void ex1_isr(void)
{
  ex1_counter--;
  if (ex1_counter <= 0)
  {
    ex1_counter = 50;
    enableOnly(-1);            /* tat het truoc khi doi so, tranh bong mo */
    if (ex1_state == 0) { display7SEG(1); enableOnly(0); ex1_state = 1; }
    else                { display7SEG(2); enableOnly(1); ex1_state = 0; }
  }
}

void ex1_loop(void) { }

/* ========================================================================
 *  BAI 2: 4 LED 7 doan (EN0..EN3 = PA6..PA9) + 2 LED DOT (PA4)
 *         DOT nhay moi 1s; hien 1,2,3,0 (12:30); doi LED 7 doan moi 500ms
 * ===================================================================== */
static const int ex2_digits[4] = {1, 2, 3, 0};   /* hien 12:30 */
static int ex2_scanCounter;                       /* 50 x 10ms  = 500ms */
static int ex2_dotCounter;                        /* 100 x 10ms = 1s    */
static int ex2_index;

void ex2_setup(void)
{
  ex2_scanCounter = 50;
  ex2_dotCounter  = 100;
  ex2_index       = 0;
  enableOnly(-1);
  display7SEG(ex2_digits[0]);
  enableOnly(0);               /* bat dau: LED thu nhat hien so 1 */
}

void ex2_isr(void)
{
  /* DOT (PA4): dao trang thai moi 1s */
  ex2_dotCounter--;
  if (ex2_dotCounter <= 0)
  {
    ex2_dotCounter = 100;
    HAL_GPIO_TogglePin(GPIOA, DOT_PIN);
  }

  /* Quet LED 7 doan: moi LED sang 500ms */
  ex2_scanCounter--;
  if (ex2_scanCounter <= 0)
  {
    ex2_scanCounter = 50;
    ex2_index = (ex2_index + 1) % 4;
    enableOnly(-1);                          /* tat het truoc khi doi so */
    display7SEG(ex2_digits[ex2_index]);
    enableOnly(ex2_index);
  }
}

void ex2_loop(void)  { }

/* ========================================================================
 *  BAI 3: update7SEG(index_led++) goi trong ngat, led_buffer[] chua 4 so
 * ===================================================================== */
static int ex3_scanCounter;    /* 50 x 10ms  = 500ms moi LED */
static int ex3_dotCounter;     /* 100 x 10ms = 1s            */

void ex3_setup(void)
{
  ex3_scanCounter = 50;
  ex3_dotCounter  = 100;
  index_led = 0;
  update7SEG(index_led++);     /* hien LED dau tien ngay tu luc chay */
}

void ex3_isr(void)
{
  /* DOT nhay moi 1s */
  ex3_dotCounter--;
  if (ex3_dotCounter <= 0)
  {
    ex3_dotCounter = 100;
    HAL_GPIO_TogglePin(GPIOA, DOT_PIN);
  }

  /* Quet LED 7 doan */
  ex3_scanCounter--;
  if (ex3_scanCounter <= 0)
  {
    ex3_scanCounter = 50;
    if (index_led >= MAX_LED) index_led = 0;   /* giu index_led trong 0..3 */
    update7SEG(index_led++);
  }
}

void ex3_loop(void)  { }

/* ========================================================================
 *  BAI 4: doi chu ky goi update7SEG de tan so quet 4 LED = 1Hz, DOT van 1s
 * ===================================================================== */
/* Tan so quet 4 LED = 1Hz  =>  moi LED sang 250ms = 25 tick */
static const int EX4_SCAN_TICKS = 25;
static int ex4_scanCounter;
static int ex4_dotCounter;

void ex4_setup(void)
{
  ex4_scanCounter = EX4_SCAN_TICKS;
  ex4_dotCounter  = 100;
  index_led = 0;
  update7SEG(index_led++);
}

void ex4_isr(void)
{
  /* DOT van nhay moi 1s */
  ex4_dotCounter--;
  if (ex4_dotCounter <= 0)
  {
    ex4_dotCounter = 100;
    HAL_GPIO_TogglePin(GPIOA, DOT_PIN);
  }

  ex4_scanCounter--;
  if (ex4_scanCounter <= 0)
  {
    ex4_scanCounter = EX4_SCAN_TICKS;
    if (index_led >= MAX_LED) index_led = 0;
    update7SEG(index_led++);
  }
}

void ex4_loop(void)  { }

/* ========================================================================
 *  BAI 5: dong ho so hh:mm. Khung cua de (tang giay bang HAL_Delay(1000))
 *         nam trong ex5_loop; ex5_isr lo viec quet LED 7 doan.
 * ===================================================================== */
/* Chua co yeu cau tan so quet rieng cho bai 5 -> giu nhu bai 4 (1Hz).
   Muon quet nhanh hon cho de nhin thi giam so nay (vd 1 -> moi LED 10ms). */
static const int EX5_SCAN_TICKS = 25;
static int ex5_scanCounter;
static int ex5_dotCounter;

void ex5_setup(void)
{
  hour = 15; minute = 8; second = 50;
  updateClockBuffer();
  ex5_scanCounter = EX5_SCAN_TICKS;
  ex5_dotCounter  = 100;
  index_led = 0;
  update7SEG(index_led++);
}

void ex5_isr(void)
{
  /* DOT nhay moi 1s (bai 7 moi chuyen ve main) */
  ex5_dotCounter--;
  if (ex5_dotCounter <= 0)
  {
    ex5_dotCounter = 100;
    HAL_GPIO_TogglePin(GPIOA, DOT_PIN);
  }

  ex5_scanCounter--;
  if (ex5_scanCounter <= 0)
  {
    ex5_scanCounter = EX5_SCAN_TICKS;
    if (index_led >= MAX_LED) index_led = 0;
    update7SEG(index_led++);
  }
}

void ex5_loop(void)
{
  second++;
  if (second >= 60) { second = 0; minute++; }
  if (minute >= 60) { minute = 0; hour++; }
  if (hour >= 24)   { hour = 0; }
  updateClockBuffer();
  HAL_Delay(1000);
}

/* ========================================================================
 *  BAI 6: software timer. Vi du blink LED PA5 trong de (Program 1.8) de
 *         tra loi 3 cau hoi: bo setTimer0(1000), doi thanh (1), doi thanh (10)
 * ===================================================================== */
void ex6_setup(void)
{
  setTimer0(1000);             /* dong 1 trong de - thu bo di / doi (1) / (10) */
}

void ex6_isr(void) { }         /* timer_run() da duoc goi san trong callback */

void ex6_loop(void)
{
  if (timer0_flag == 1)
  {
    HAL_GPIO_TogglePin(GPIOA, LED_PIN);
    setTimer0(2000);
  }
}

/* ========================================================================
 *  BAI 7: nang cap Bai 5 bang software timer, bo HAL_Delay,
 *         DOT (PA4) cung chuyen ve main
 * ===================================================================== */
static const int EX7_SCAN_TICKS = 25;     /* quet 1Hz nhu bai 4, 5 */
static int ex7_scanCounter;

void ex7_setup(void)
{
  hour = 15; minute = 8; second = 50;
  updateClockBuffer();
  ex7_scanCounter = EX7_SCAN_TICKS;
  index_led = 0;
  update7SEG(index_led++);
  setTimer0(1000);             /* timer 1s: tang giay + nhay DOT */
}

void ex7_isr(void)
{
  /* Ngat chi con quet LED 7 doan (bai 8 se chuyen not phan nay ve main) */
  ex7_scanCounter--;
  if (ex7_scanCounter <= 0)
  {
    ex7_scanCounter = EX7_SCAN_TICKS;
    if (index_led >= MAX_LED) index_led = 0;
    update7SEG(index_led++);
  }
}

void ex7_loop(void)
{
  if (timer0_flag == 1)
  {
    setTimer0(1000);
    HAL_GPIO_TogglePin(GPIOA, DOT_PIN);    /* DOT chuyen ve main */
    second++;
    if (second >= 60) { second = 0; minute++; }
    if (minute >= 60) { minute = 0; hour++; }
    if (hour >= 24)   { hour = 0; }
    updateClockBuffer();
  }
}

/* ========================================================================
 *  BAI 8: chuyen ca update7SEG() ve main; ngat chi con chay software timer
 * ===================================================================== */
static const int EX8_SCAN_MS = 250;       /* moi LED 250ms => quet 1Hz */

void ex8_setup(void)
{
  hour = 15; minute = 8; second = 50;
  updateClockBuffer();
  index_led = 0;
  update7SEG(index_led++);
  setTimer0(1000);             /* timer 0: tang giay + nhay DOT */
  setTimer1(EX8_SCAN_MS);      /* timer 1: quet LED 7 doan      */
}

void ex8_isr(void) { }         /* ngat chi con timer_run() (da goi san trong callback) */

void ex8_loop(void)
{
  if (timer0_flag == 1)
  {
    setTimer0(1000);
    HAL_GPIO_TogglePin(GPIOA, DOT_PIN);
    second++;
    if (second >= 60) { second = 0; minute++; }
    if (minute >= 60) { minute = 0; hour++; }
    if (hour >= 24)   { hour = 0; }
    updateClockBuffer();
  }

  if (timer1_flag == 1)
  {
    setTimer1(EX8_SCAN_MS);
    if (index_led >= MAX_LED) index_led = 0;
    update7SEG(index_led++);
  }
}

/* ========================================================================
 *  BAI 9 (them): LED matrix 8x8, updateLEDMatrix(index), hien chu 'A'
 *         ROW0..ROW7 = PB8..PB15, ENM0..ENM7 = PA2 PA3 PA10..PA15
 * ===================================================================== */
/* Chu 'A' theo COT (bit r = hang r):
 *   ..XX..    hang 0
 *   .X..X.    hang 1
 *   X....X    hang 2..3
 *   XXXXXX    hang 4
 *   X....X    hang 5..7
 */
static const uint8_t FONT_A[8] = {0x00, 0xFC, 0x12, 0x11, 0x11, 0x12, 0xFC, 0x00};

static const int EX9_SEG_MS    = 250;
static const int EX9_MATRIX_MS = 10;
void ex9_setup(void)
{
  hour = 15; minute = 8; second = 50;
  updateClockBuffer();
  index_led = 0;
  update7SEG(index_led++);

  for (int i = 0; i < 8; i++) matrix_buffer[i] = FONT_A[i];
  index_led_matrix = 0;

  setTimer0(1000);
  setTimer1(EX9_SEG_MS);
  setTimer2(EX9_MATRIX_MS);
}

void ex9_isr(void) { }

void ex9_loop(void)
{
  if (timer0_flag == 1) { setTimer0(1000);          clockTick(); }
  if (timer1_flag == 1) { setTimer1(EX9_SEG_MS);    scan7SEGStep(); }
  if (timer2_flag == 1) { setTimer2(EX9_MATRIX_MS); matrixScanStep(); }
}


static const uint8_t EX10_MSG[16] = {
    0x00, 0xFC, 0x12, 0x11, 0x11, 0x12, 0xFC, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const int EX10_SEG_MS        = 250;  /* moi LED 7 doan 250ms            */
static const int EX10_MATRIX_MS     = 10;   /* moi cot ma tran 10ms            */
static const int EX10_SHIFT_FRAMES  = 3;    /* 3 khung (3 x 80ms) dich 1 cot   */

static int ex10_offset;
static int ex10_frames;

/* Chep 8 cot tu vi tri ex10_offset cua thong diep vao matrix_buffer */
static void ex10_loadWindow(void)
{
  for (int i = 0; i < 8; i++)
    matrix_buffer[i] = EX10_MSG[(ex10_offset + i) % 16];
}

void ex10_setup(void)
{
  hour = 15; minute = 8; second = 50;
  updateClockBuffer();
  index_led = 0;
  update7SEG(index_led++);

  ex10_offset = 0;
  ex10_frames = 0;
  ex10_loadWindow();
  index_led_matrix = 0;

  setTimer0(1000);
  setTimer1(EX10_SEG_MS);
  setTimer2(EX10_MATRIX_MS);
}

void ex10_isr(void) { }

void ex10_loop(void)
{
  if (timer0_flag == 1) { setTimer0(1000);           clockTick(); }
  if (timer1_flag == 1) { setTimer1(EX10_SEG_MS);    scan7SEGStep(); }

  if (timer2_flag == 1)
  {
    setTimer2(EX10_MATRIX_MS);
    if (matrixScanStep())                      /* vua quet xong 1 khung (8 cot) */
    {
      ex10_frames++;
      if (ex10_frames >= EX10_SHIFT_FRAMES)
      {
        ex10_frames = 0;
        ex10_offset = (ex10_offset + 1) % 16;  /* dich chu sang trai 1 cot */
        ex10_loadWindow();
      }
    }
  }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
