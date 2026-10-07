/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ACTIVE_EXERCISE   5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void exercise1(void);
void exercise2(void);
void exercise3(void);
void exercise4(void);
void exercise5(void);
void exercise6(void);
void exercise7(void);
void exercise8(void);
void exercise9(void);
void exercise10(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  /* USER CODE BEGIN 2 */

#if   ACTIVE_EXERCISE == 1
  exercise1();
#elif ACTIVE_EXERCISE == 2
  exercise2();
#elif ACTIVE_EXERCISE == 3
  exercise3();
#elif ACTIVE_EXERCISE == 4
  exercise4();
#elif ACTIVE_EXERCISE == 5
  exercise5();
#elif ACTIVE_EXERCISE == 6
  exercise6();
#elif ACTIVE_EXERCISE == 7
  exercise7();
#elif ACTIVE_EXERCISE == 8
  exercise8();
#elif ACTIVE_EXERCISE == 9
  exercise9();
#elif ACTIVE_EXERCISE == 10
  exercise10();
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  * @brief GPIO Initialization Function
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
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|LED_RED_Pin|LED_YELLOW_Pin|LED_GREEN_Pin
                          |LED_RED2_Pin|LED_YELLOW2_Pin|LED_GREEN2_Pin|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_15
                          |GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA4 LED_RED_Pin LED_YELLOW_Pin LED_GREEN_Pin
                           LED_RED2_Pin LED_YELLOW2_Pin LED_GREEN2_Pin PA11
                           PA12 PA13 PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|LED_RED_Pin|LED_YELLOW_Pin|LED_GREEN_Pin
                          |LED_RED2_Pin|LED_YELLOW2_Pin|LED_GREEN2_Pin|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB15
                           PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_15
                          |GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */


void exercise1(void)
{
  while (1)
  {
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_RESET);
    HAL_Delay(2000);
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_SET);
    HAL_Delay(2000);
  }
}


void exercise2(void)
{
  while (1)
  {
	    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_SET);
	    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
	    HAL_Delay(5000);
	    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_SET);
	    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
	    HAL_Delay(3000);
	    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
	    HAL_Delay(2000);
  }
}


void exercise3(void)
{
  while (1)
  {
	  /*  1 (3s): N-S GREEN, E-W RED */
	      HAL_GPIO_WritePin(LED_RED_GPIO_Port,     LED_RED_Pin,     GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port,  LED_YELLOW_Pin,  GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,   LED_GREEN_Pin,   GPIO_PIN_RESET);
	      HAL_GPIO_WritePin(LED_RED2_GPIO_Port,    LED_RED2_Pin,    GPIO_PIN_RESET);
	      HAL_GPIO_WritePin(LED_YELLOW2_GPIO_Port, LED_YELLOW2_Pin, GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_GREEN2_GPIO_Port,  LED_GREEN2_Pin,  GPIO_PIN_SET);
	      HAL_Delay(3000);

	      /*  2 (2s): N-S YELLOW, E-W RED */
	      HAL_GPIO_WritePin(LED_RED_GPIO_Port,     LED_RED_Pin,     GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port,  LED_YELLOW_Pin,  GPIO_PIN_RESET);
	      HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,   LED_GREEN_Pin,   GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_RED2_GPIO_Port,    LED_RED2_Pin,    GPIO_PIN_RESET);
	      HAL_GPIO_WritePin(LED_YELLOW2_GPIO_Port, LED_YELLOW2_Pin, GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_GREEN2_GPIO_Port,  LED_GREEN2_Pin,  GPIO_PIN_SET);
	      HAL_Delay(2000);

	      /*  3 (3s): N-S RED, E-W GREEN */
	      HAL_GPIO_WritePin(LED_RED_GPIO_Port,     LED_RED_Pin,     GPIO_PIN_RESET);
	      HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port,  LED_YELLOW_Pin,  GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,   LED_GREEN_Pin,   GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_RED2_GPIO_Port,    LED_RED2_Pin,    GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_YELLOW2_GPIO_Port, LED_YELLOW2_Pin, GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_GREEN2_GPIO_Port,  LED_GREEN2_Pin,  GPIO_PIN_RESET);
	      HAL_Delay(3000);

	      /*  4 (2s): N-S RED, E-W YELLOW */
	      HAL_GPIO_WritePin(LED_RED_GPIO_Port,     LED_RED_Pin,     GPIO_PIN_RESET);
	      HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port,  LED_YELLOW_Pin,  GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,   LED_GREEN_Pin,   GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_RED2_GPIO_Port,    LED_RED2_Pin,    GPIO_PIN_SET);
	      HAL_GPIO_WritePin(LED_YELLOW2_GPIO_Port, LED_YELLOW2_Pin, GPIO_PIN_RESET);
	      HAL_GPIO_WritePin(LED_GREEN2_GPIO_Port,  LED_GREEN2_Pin,  GPIO_PIN_SET);
	      HAL_Delay(2000);
  }
}


static const uint16_t segPins[7] = {
		GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8,
		    GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11
};

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

void display7SEG(int num)
{
    if (num < 0 || num > 9) return;

    for (int i = 0; i < 7; i++)
    {
        GPIO_PinState state = segTable[num][i] ? GPIO_PIN_RESET : GPIO_PIN_SET;
        HAL_GPIO_WritePin(GPIOA, segPins[i], state);
    }
}

void exercise4(void)
{
  int counter = 0;
  while (1)
  {
    if (counter >= 10) counter = 0;
    display7SEG(counter++);
    HAL_Delay(1000);
  }
}


void exercise5(void)
{
  while (1)
  {
    /* Phase 1 (3s): Group1 GREEN, Group2 RED */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    for (int t = 3; t >= 1; t--) { display7SEG(t); HAL_Delay(1000); }

    /* Phase 2 (2s): Group1 YELLOW, Group2 RED */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    for (int t = 2; t >= 1; t--) { display7SEG(t); HAL_Delay(1000); }

    /* Phase 3 (3s): Group1 RED, Group2 GREEN */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    for (int t = 3; t >= 1; t--) { display7SEG(t); HAL_Delay(1000); }

    /* Phase 4 (2s): Group1 RED, Group2 YELLOW */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    for (int t = 2; t >= 1; t--) { display7SEG(t); HAL_Delay(1000); }
  }
}


static const uint16_t clockPins6[12] = {
     GPIO_PIN_15, GPIO_PIN_4,  GPIO_PIN_5,
    GPIO_PIN_6,  GPIO_PIN_7,  GPIO_PIN_8,  GPIO_PIN_9,
    GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13,GPIO_PIN_14
};

void exercise6(void)
{

  for (int i = 0; i < 12; i++)
    HAL_GPIO_WritePin(GPIOA, clockPins6[i], GPIO_PIN_SET);

  while (1)
  {
    for (int i = 0; i < 12; i++)
    {
      HAL_GPIO_WritePin(GPIOA, clockPins6[i], GPIO_PIN_RESET);
      HAL_Delay(1000);
      HAL_GPIO_WritePin(GPIOA, clockPins6[i], GPIO_PIN_SET);
    }
  }
}

void clearAllClock(void)
{
  for (int i = 0; i < 12; i++)
    HAL_GPIO_WritePin(GPIOA, clockPins6[i], GPIO_PIN_SET);
}

void exercise7(void)
{
  for (int i = 0; i < 12; i++){
    HAL_GPIO_WritePin(GPIOA, clockPins6[i], GPIO_PIN_RESET);}
  HAL_Delay(2000);

  clearAllClock();

  while (1)
  {
  }
}



void setNumberOnClock(int num)
{
  if (num < 0 || num > 11) return;
  HAL_GPIO_WritePin(GPIOA, clockPins6[num], GPIO_PIN_RESET);
}

void exercise8(void)
{
  int num = 0;
  while (1)
  {
    clearAllClock();
    setNumberOnClock(num);
    HAL_Delay(1000);
    num++;
    if (num > 11) num = 0;
  }
}


void clearNumberOnClock(int num)
{
  if (num < 0 || num > 11) return;
  HAL_GPIO_WritePin(GPIOA, clockPins6[num], GPIO_PIN_SET);
}

void exercise9(void)
{
  while (1)
  {
    for (int i = 0; i < 12; i++){
      HAL_GPIO_WritePin(GPIOA, clockPins6[i], GPIO_PIN_RESET);}
    HAL_Delay(1000);

    for (int i = 0; i < 12; i++)
    {
      clearNumberOnClock(i);
      HAL_Delay(500);
    }
  }
}

void exercise10(void)
{
  int h = 0, m = 0, s = 0;

  while (1)
  {
    int hourPos   = h % 12;
    int minutePos = m / 5;
    int secondPos = s / 5;

    clearAllClock();
    setNumberOnClock(hourPos);
    setNumberOnClock(minutePos);
    setNumberOnClock(secondPos);

    HAL_Delay(1000);

    s++;
    if (s >= 60) { s = 0; m++; }
    if (m >= 60) { m = 0; h++; }
    if (h >= 12) { h = 0; }
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
