/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
// Oled
#include "ssd1306.h"
#include "fonts.h"
#include "oled_ssd1306.h"

#include "Si7021_driver.h"
#include "Humidity_and_Temperature_SI7021_sensor.h"

#include "GPS_NEO_6M.h"

#include "Humidity_and_Temperature_AM2302_sensor.h"

#include "Flash_W25Q.h"
#include "w25qxx.h"

#include "Fingerprint_sensor_GT511C3.h"

#include "GSM_IOT_GA6.h"

#include "stdbool.h"

#include "stdio.h"
#include "string.h"               // For work with "strtok" function
#include "stdlib.h"               // For work with "atoi" function

#include "GSM_IOT_GA6.h"


extern GPGGA_data_is_ready;   // Flag. If time data is ready then print it on OLED.
extern uint8_t receive_gps_signal;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void test_flash_W25Q(void);
void generate_sound(void);
char read_one_sign_from_keyboard(void);
bool enter_a_mobile_number(char * number);

int gsm_mode(char sign);
int gps_mode(char sign);
int fingerprint_mode(char sign);
int sensors_mode(char sign);

void clearing_buffer(char *array, int size);

char set_ID_number(void);
//void gps_mode(char sign);

uint8_t GPS_buff[512];      						// main buffer for stream from GPS
//void parsing_GPS(uint8_t *GPS_buff, int size_buff);

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define OLED ON
#define H_a_T_SI7021 ON
#define I2C_SCANNER ON
#define GPS ON
#define SOUND OFF       // WARNING !!!! IF "ON" Sound UART1 CAN'T WORK !!!!
#define FINGERPRINT OFF

// main modes devices
bool GSM_MODE = false;
bool GPS_MODE = false;
bool FINGERPRINT_MODE = false;
bool SENSORS_MODE = false;

//bool EXIT_FROM_MODE = false;

bool INCOMMING_RING_OR_SMS_STATUS = false;   // It status toggle in extern interrupt

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi2;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
// -----------------------------------------------------------------------------
// Receive data from GPS module
#if GPS
	uint8_t flag = 0;					// Flag signals what GPS buffer is full
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)    // was   void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	{
		flag = 1;
	}
#endif
// ----------------------------------------------------------------------------

uint8_t interrupt_flag = 0;

/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */


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
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	#if I2C_SCANNER
  		I2C_1_scaner();
	#endif

	#if H_a_T_SI7021
  		read_T_and_H_SI7021();
	#endif

	#if OLED
  		init_oled();
	  //test_oled();
	#endif

	#if GPS
  		memset(GPS_buff ,0 ,sizeof(GPS_buff));
  		HAL_UART_Receive_DMA(&huart3, GPS_buff, 512);
	#endif

  	HAL_TIM_Base_Start_IT(&htim2);

  	// Turn on interrupt, if in RX buffer are one byte
  	// Register CR1-> RXNEIE
  	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  	// Test led
  	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);


  #if SOUND
  	uint16_t pulse = 2;
  	uint32_t period = 1;
  	uint16_t max = 200;
  	uint16_t min = 1;

  	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  	for(period = min; period < max; period++)
  	{
  		pulse = period/2;
  		buzzer_fricvency_setings(htim1, TIM_CHANNEL_1, period, pulse);
  		HAL_Delay(1);
  	}
  	if(period >= max)
  	{
  		for(period = max; period > min; period--)
  		{
  			pulse = period/2;
  			buzzer_fricvency_setings(htim1, TIM_CHANNEL_1, period, pulse);
  			HAL_Delay(1);
  		}
  	}
  	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1); // stop generation of pwm
  #endif

  int EXIT = 0;

while (1)
{
	//test_function(); 				// TEST FUNCTUION <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	//test_flash_W25Q();

	EXIT = 0;
	char sign = 0;													// Char for keyboard

	touch_bakcklight(0);											// Turn off Fingerprint modeul LED

	print_main_menu();  											// Print main menu on OLED

	do                                                            	// Wait for choice
	{
		sign = read_one_sign_from_keyboard();                     	// Read sign from keyboard

		// For debug ///////////
		//sign = '1';    // turn on GSM mode
		/////////////////////

		if(sign == '1')    							// If select GSM mode
		{
			GSM_MODE = true;
			while(GSM_MODE && (EXIT == 0))
			{
				EXIT = gsm_mode(sign);
			}
		}
		if(sign == '2')    							// If select GPS mode
		{
			GPS_MODE = true;
			while (GPS_MODE && (EXIT == 0))
			{
				EXIT = gps_mode(sign);
			}
		}
		if(sign == '3')    							// If select FINGERPRINT MODE
		{
			FINGERPRINT_MODE = true;
			while (FINGERPRINT_MODE && (EXIT == 0))
			{
				EXIT = fingerprint_mode(sign);
			}
		}
		if(sign == '4')    							// If select SENSORS MODE
		{
			SENSORS_MODE = true;
			while (SENSORS_MODE && (EXIT == 0))
			{
				EXIT = sensors_mode(sign);
			}
		}

		ssd1306_UpdateScreen();
		}while ((sign != '1') && (sign != '2') && (sign != '3') && (sign != '4'));     // Select one from 3 modes


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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 15600;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 4000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 2000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1440;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 50000;
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
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CS_M25Q_Pin|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_M25Q_Pin */
  GPIO_InitStruct.Pin = CS_M25Q_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(CS_M25Q_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA11 PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

}

/* USER CODE BEGIN 4 */


// ----------------------------------------------------------------------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
        if(htim->Instance == TIM2) //check if the interrupt comes from TIM2
        {
        	//read_T_and_H_SI7021();
        	interrupt_flag = 1;
        }
}
// ----------------------------------------------------------------------------
// Function for generate
void buzzer_fricvency_setings(TIM_HandleTypeDef timer, uint32_t channel, uint16_t period, uint16_t pulse)
{
	 HAL_TIM_PWM_Stop(&timer, channel); 					// stop generation of pwm
	 TIM_OC_InitTypeDef sConfigOC;
	 timer.Init.Prescaler = 720;
	 timer.Init.Period = period; 							// set the period duration
	 HAL_TIM_PWM_Init(&timer); 								// reinititialise with new period value
	 sConfigOC.OCMode = TIM_OCMODE_PWM1;
	 sConfigOC.Pulse = pulse; 								// set the pulse duration
	 sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	 sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	 HAL_TIM_PWM_ConfigChannel(&timer, &sConfigOC, channel);
	 HAL_TIM_PWM_Start(&timer, channel); 					// start pwm generation
}
//------------------------------------------------------------------------------
void generate_sound(void)
{
	uint16_t pulse = 2;
	uint32_t period = 1;
	uint16_t max = 200;
	uint16_t min = 1;
	for(period = min; period < max; period++)
	{
		  pulse = period/2;
		  buzzer_fricvency_setings(htim1, TIM_CHANNEL_1, period, pulse);
		  HAL_Delay(1);
	}
	if(period >= max)
	{
		  for(period = max; period > min; period--)
		  	  {
		  		  pulse = period/2;
		  		  buzzer_fricvency_setings(htim1, TIM_CHANNEL_1, period, pulse);
		  		  HAL_Delay(1);
		  	  }
	}
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1); // stop generation of pwm
}
// ---------------------------------------------------------------------------
/*
  Its function read one sing from keyboard
 */
char read_one_sign_from_keyboard(void)
{
	// 2. Зробити зчитування даних з клавіатури як переривання
	char sign = 0;
	uint8_t readed_status = 0;

	readed_status = 0;
	uint8_t i = 0;

	for(i=1; i<=4; i++)
	{
		if((i == 1) && (readed_status != 1))
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1);     // For detect 123A
			HAL_Delay(1);
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9))
			{
			  	sign = 'A';
			  	readed_status = 1;
			}

			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8))
			{
			  	sign = '3';
			  	readed_status = 1;
			}

			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5))
			{
			  	sign = '2';
			  	readed_status = 1;
			}

			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4))
			{
			  	sign = '1';
			  	readed_status = 1;
			}
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);
		}

		if((i == 2) && (readed_status != 1))
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1);     // For detect 456B
			HAL_Delay(1);
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9))
			{
				sign = 'B';
				readed_status = 1;
			}

			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8))
			{
				sign = '6';
				readed_status = 1;
			}

			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5))
			{
				sign = '5';
				readed_status = 1;
			}

			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4))
			{
				sign = '4';
				readed_status = 1;
			}
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0);
		}

		if((i == 3) && (readed_status != 1))
		{
		 	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);     // For detect 789C
		 	HAL_Delay(1);
		 	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9))
		 	{
		 		sign = 'C';
		 		readed_status = 1;
		 	}

		 	else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8))
		 	{
		 		sign = '9';
		 		readed_status = 1;
		 	}

		 	else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5))
		 	{
		 		sign = '8';
		 		readed_status = 1;
		 	}

		 	else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4))
		 	{
		 		 sign = '7';
		 		readed_status = 1;
		 	}
		 	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);
		}


		if((i == 3) && (readed_status != 1))
		{
		    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);     // For detect *0#D
		    HAL_Delay(1);

		   	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9))
		   	{
		   		sign = 'D';
		   		readed_status = 1;
		   	}

		   	else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8))
		   	{
		   		 sign = '#';
		   		 readed_status = 1;
		   	}

		   	else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5))
		   	{
		   		 sign = '0';
		   		 readed_status = 1;
		   	}

		   	else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4))
		   	{
		   		 sign = '*';
		   		 readed_status = 1;
		   	}
		   	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
		}
	}
	return sign;
}
// ----------------------------------------------------------------------------
int gps_mode(char sign)
{
	// Clearn OLED
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	// Print mode in head
	char str_gps[50]={0};
	//memset(str_gps, 0 , sizeof(str_gps));
	//sprintf(str_gps,"%s", "2.GPS: waiting...");
	//strcpy(str_gps, "2.GPS: waiting...");

//	ssd1306_SetCursor(00, 00);
//	ssd1306_WriteString(str_gps, Font_7x10, White);
//	memset(str_gps, 0 , sizeof(str_gps));
//
//	ssd1306_UpdateScreen();
	print_text_on_OLED(0, 1, true, "2.GPS: waiting...");

	uint8_t broken_packet_counter = 0;

	do                                                            	// Wait on choiсe
	{
		sign = read_one_sign_from_keyboard();                       // Read sign from keyboard

		if(sign == '*')    	// If select EXIT  // Exit in main menu
		{
			// Clear all OLED
			ssd1306_Fill(Black);
			ssd1306_UpdateScreen();

			GPS_MODE = false;
			GSM_MODE = false;
			FINGERPRINT_MODE = false;
			SENSORS_MODE = false;

			return 1;   // Flag_fro exit from there
		}
		else
		{
			// Parsing data form GPS
			parsing_GPS(GPS_buff, 512);
			int select_print_data = 1;							// Flag for print GPS data on OLED
			OLED_prinr_all_data(select_print_data);

			if(GPGGA_data_is_ready == 1)						// Check if data from GPS device was correct ( parsed GPGLL line correct)
			{
				// Print the data that GPS is valid
//				memset(str_gps, 0 , sizeof(str_gps));
//				sprintf(str_gps,"%s", "1.GPS: OK            ");
//				ssd1306_SetCursor(00, 00);
//				ssd1306_WriteString(str_gps, Font_7x10, White);
				print_text_on_OLED(0, 1, false, "1.GPS: OK            ");

				broken_packet_counter = 0;
				GPGGA_data_is_ready = 0;
			}
			else
			{
				broken_packet_counter ++;
				//HAL_Delay(500);
				if((GPGGA_data_is_ready != 1) && (broken_packet_counter >= 20))
				{
					// Print the data that GPS is does not valid
					GPGGA_data_is_ready = 0;
					broken_packet_counter = 0;

//					memset(str_gps, 0 , sizeof(str_gps));
//					sprintf(str_gps,"%s", "1.GPS: NO SIGNAL  ");
//					ssd1306_SetCursor(00, 00);
//					ssd1306_WriteString(str_gps, Font_7x10, White);
					print_text_on_OLED(0, 1, false, "1.GPS: NO SIGNAL  ");
				}
			}

			ssd1306_UpdateScreen();
		}
	}while ( (sign != '*'));     // Select one from 3 modes
}
// ----------------------------------------------------------------------------
int gsm_mode(char sign)
{
	// Clearn OLED
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	// Print mode in head
	char str_gsm[50]={0};
//	sprintf(str_gsm,"%s", "1.GSM: waiting...");
//	ssd1306_SetCursor(00, 00);
//	ssd1306_WriteString(str_gsm, Font_7x10, White);
//	ssd1306_UpdateScreen();
//	memset(str_gsm, 0 , sizeof(str_gsm));
	print_text_on_OLED(0, 1, true, "1.GSM: waiting...");

	// Init GSM module
	init_GSM_uart_comunication();
	if(init_gsm_module() == HAL_OK)
	{
		// init OK
		GSM_INIT = 1;
		claen_oled_lines(true, false, false, false, false);

//		sprintf(str_gsm,"%s", "1.GSM: OK");
//		ssd1306_SetCursor(00, 00);
//		ssd1306_WriteString(str_gsm, Font_7x10, White);
//		ssd1306_UpdateScreen();
		print_text_on_OLED(0, 1, true, "1.GSM: OK");
	}
	else
	{
		// GSM didn't init
		GSM_INIT = 0;

		claen_oled_lines(true, false, false, false, false);

//		sprintf(str_gsm,"%s", "1.GSM: ERROR");
//		ssd1306_SetCursor(00, 00);
//		ssd1306_WriteString(str_gsm, Font_7x10, White);
//		ssd1306_UpdateScreen();
		print_text_on_OLED(0, 1, true, "1.GSM: ERROR");

		HAL_Delay(2000);
	}
		// END INIT GSM MODULE  //////////////////////////

    if(GSM_INIT == 1)
    {
    	int incoming_call_status = 0;					// Call status.
    	bool first_time_after_call = false;

    	int print_oled_status = 0;

		// Print GSM menu
//    	memset(str_gsm, 0 , sizeof(str_gsm));
//		sprintf(str_gsm,"%s", "1.CALL to me");
//		ssd1306_SetCursor(00, 16);
//		ssd1306_WriteString(str_gsm, Font_7x10, White);
    	print_text_on_OLED(0, 2, false, "1.CALL to me");

//		memset(str_gsm, 0 , sizeof(str_gsm));
//		sprintf(str_gsm,"%s", "2.CALL on number");
//		ssd1306_SetCursor(00, 26);
//		ssd1306_WriteString(str_gsm, Font_7x10, White);
    	print_text_on_OLED(0, 3, false, "2.CALL on number");

//		memset(str_gsm, 0 , sizeof(str_gsm));
//		sprintf(str_gsm,"%s", "3.For send SMS");
//		ssd1306_SetCursor(00, 36);
//		ssd1306_WriteString(str_gsm, Font_7x10, White);
//
//		ssd1306_UpdateScreen();
    	print_text_on_OLED(0, 4, true, "3.For send SMS");

		bool incoming_call_status_oled = false;				// Status for blinky
		char incoming_number[15] = {0};						// Buffer for incoming number
		char sign='\0';

		do
		{
			do
			{
				// Wait incoming call

				incoming_call_status = wait_incoming_call(incoming_number);

				if(incoming_call_status == 2)											// detect incoming call
				{
					sign = read_one_sign_from_keyboard();								// Read sign from keyboard
					incoming_call_status = wait_incoming_call(incoming_number);			// Read answer from GSM

					// For print one time
					if(incoming_call_status_oled == false)
					{
						claen_oled_lines(false, true, true, true, true);

//						memset(str_gsm, 0 , sizeof(str_gsm));
//						sprintf(str_gsm,"%s", "'A':pick up phone");
//						ssd1306_SetCursor(00, 36);
//						ssd1306_WriteString(str_gsm, Font_7x10, White);
						print_text_on_OLED(0, 4, false, "'A':pick up phone");

//						memset(str_gsm, 0 , sizeof(str_gsm));
//						sprintf(str_gsm,"%s", "'*':end call");
//						ssd1306_SetCursor(00, 46);
//						ssd1306_WriteString(str_gsm, Font_7x10, White);
						print_text_on_OLED(0, 5, false, "'*':end call");

//						memset(str_gsm, 0 , sizeof(str_gsm));
//						sprintf(str_gsm,"%s", "Incoming CALL...");
//						ssd1306_SetCursor(00, 16);
//						ssd1306_WriteString(str_gsm, Font_7x10, White);
						print_text_on_OLED(0, 1, false, "Incoming CALL...");

//						memset(str_gsm, 0 , sizeof(str_gsm));
//						sprintf(str_gsm,"%s", incoming_number);
//						ssd1306_SetCursor(00, 26);
//						ssd1306_WriteString(incoming_number, Font_7x10, White);
						print_text_on_OLED(0, 2, true, incoming_number);

						memset(incoming_number, 0 , sizeof(incoming_number));

//						ssd1306_UpdateScreen();

					}

					// Waiting for action on incoming call or sms
					do{
						sign = read_one_sign_from_keyboard();
						incoming_call_status = wait_incoming_call(incoming_number);			// Read answer from GSM
						HAL_Delay(200);
					}while ((sign != '*') && (sign != 'A') && (incoming_call_status != 1));

					if(sign == '*')													// Збити трубку
					{
						if(end_of_call() != 1)										// Send "end call" command in GSM module
						{
							// ERROR
						}
						else
						{
							uint8_t i, res = 0;
							for(i=0; i<=6; i++)
							{
								res = i%2;
								if(res)
								{
//									memset(str_gsm, 0 , sizeof(str_gsm));
//									sprintf(str_gsm,"%s", "CALL END");
//									ssd1306_SetCursor(00, 16);
//									ssd1306_WriteString(str_gsm, Font_7x10, White);
//
//									ssd1306_UpdateScreen();
									print_text_on_OLED(0, 2, true, "CALL END");
									HAL_Delay(200);
								}
								else
								{
									claen_oled_lines(false, true, true, true, true);		// Clean OLED
									HAL_Delay(200);
								}
							}

						}
					}

					if(sign == 'A')													// Pick up the phone
					{
						if(accepts_on_incomming_call() == 1);						// Send "pick up the phone" command in GSM module
						{
							claen_oled_lines(false, true, true, true, true);		// Clean OLED

//							sprintf(str_gsm,"%s", "SPEAK...");
//							ssd1306_SetCursor(00, 16);
//							ssd1306_WriteString(str_gsm, Font_7x10, White);
//							memset(str_gsm, 0 , sizeof(str_gsm));
							print_text_on_OLED(0, 2, false, "SPEAK...");

//							sprintf(str_gsm,"%s", "'*':end call");
//							ssd1306_SetCursor(00, 46);
//							ssd1306_WriteString(str_gsm, Font_7x10, White);
//							memset(str_gsm, 0 , sizeof(str_gsm));
//
//							ssd1306_UpdateScreen();
							print_text_on_OLED(0, 5, true, "'*':end call");

							do{
								HAL_Delay(200);
								sign = read_one_sign_from_keyboard();                      				// Read sign from keyboard
								HAL_Delay(20);
								incoming_call_status = wait_incoming_call(incoming_number);				// Read answer from GSM

								if(incoming_call_status == 1)											// Sometimes GSM module sends wrong answer (BUG)
								{
									HAL_Delay(200);
									incoming_call_status = wait_incoming_call(incoming_number);			// Read answer from GSM again
								}

							}while ((sign != '*') && (incoming_call_status == 3) );

							if(incoming_call_status == 1)												// If end call from phone
							{
								uint8_t i, res = 0;
								for(i=0; i<=6; i++)
								{
									res = i%2;
									if(res)
									{
//										memset(str_gsm, 0 , sizeof(str_gsm));
//										sprintf(str_gsm,"%s", "CALL END");
//										ssd1306_SetCursor(00, 16);
//										ssd1306_WriteString(str_gsm, Font_7x10, White);
//
//										ssd1306_UpdateScreen();
										print_text_on_OLED(0, 1, true, "CALL END");
										HAL_Delay(200);
									}
									else
									{
										claen_oled_lines(false, true, true, true, true);				// Clean OLED
										HAL_Delay(200);
									}
								}
							}
							if(sign == '*')																// If end call from GSM mode
							{
								if(end_of_call() != 1)													// Send "end call" command in GSM module
								{
																										// ERROR
								}
								else
								{
									uint8_t i, res = 0;
									for(i=0; i<=6; i++)
									{
										res = i%2;
										if(res)
										{
//											memset(str_gsm, 0 , sizeof(str_gsm));
//											sprintf(str_gsm,"%s", "CALL END");
//											ssd1306_SetCursor(00, 16);
//											ssd1306_WriteString(str_gsm, Font_7x10, White);
//
//											ssd1306_UpdateScreen();
											print_text_on_OLED(0, 1, true, "CALL END");

											HAL_Delay(200);
										}
										else
										{
											claen_oled_lines(false, true, true, true, true);		// Clean OLED
											HAL_Delay(200);
										}
									}
								}
							}
						}
					}
					incoming_call_status_oled = true;
				}
				HAL_Delay(200);
			}while (INCOMMING_RING_OR_SMS_STATUS == true);									// If "Ring" pin is in low (active) state

			// if no any incoming calls or sms
			incoming_call_status = wait_incoming_call(incoming_number);						// Read answer from GSM
			if((INCOMMING_RING_OR_SMS_STATUS == false) && (incoming_call_status == 1))
			{
				if(incoming_call_status_oled == true)										// print menu, only after incoming call or sms
				{
					claen_oled_lines(false, true, true, true, true);

					// Print GSM menu
//					sprintf(str_gsm,"%s", "1.CALL to me");
//					ssd1306_SetCursor(00, 16);
//					ssd1306_WriteString(str_gsm, Font_7x10, White);
//					memset(str_gsm, 0 , sizeof(str_gsm));
					print_text_on_OLED(0, 2, false, "1.CALL to me");


//					sprintf(str_gsm,"%s", "2.CALL on number");
//					ssd1306_SetCursor(00, 26);
//					ssd1306_WriteString(str_gsm, Font_7x10, White);
//					memset(str_gsm, 0 , sizeof(str_gsm));
					print_text_on_OLED(0, 3, false, "2.CALL on number");

//					sprintf(str_gsm,"%s", "3.For send SMS");
//					ssd1306_SetCursor(00, 36);
//					ssd1306_WriteString(str_gsm, Font_7x10, White);
//					memset(str_gsm, 0 , sizeof(str_gsm));
//
//					ssd1306_UpdateScreen();
					print_text_on_OLED(0, 4, true, "3.For send SMS");

					incoming_call_status_oled = false;
				}

				HAL_Delay(200);
				sign = read_one_sign_from_keyboard();

				if(sign == '1')																// Call to me
				{
					int call_status = call_on_mu_number();
					show_sratus_call (call_status, str_gsm, sign, 1);

					incoming_call_status_oled = true;
				}

				if(sign == '2')  															// call on number
				{
					// 1. Type mobile number.
					char number[13]={0};													// Buffer where will be save entered number
					uint8_t size_number = 0;												// How many entered digits in number
					bool entered_number_status = false;										// Status number buffer.
					entered_number_status = enter_a_mobile_number(number);					// Enter number

					for(size_number = 0; number[size_number] != '\0'; size_number++){}		// Count digits

					// 2. Call on entered number.
					if(entered_number_status == true)										// If entered all digits will be call on this number
					{
						int call_status = call_on_number(number, size_number);
						show_sratus_call(call_status, str_gsm, sign, 0);
					}
					incoming_call_status_oled = true;
				}

				if(sign == '3')	// sms
				{

				}
			}


		}while ( sign != '*');     // Select one from 3 modes

	}
    else     				// If error init GSM module. Exit from GSM menu
    {
        HAL_Delay(2000);

        // Clear all OLED
        ssd1306_Fill(Black);
        ssd1306_UpdateScreen();

        GSM_MODE = false;
        GPS_MODE = false;
        FINGERPRINT_MODE = false;
        SENSORS_MODE = false;

        return 1;  			 // Flag_fro exit from there
    }
}
// ----------------------------------------------------------------------------
int fingerprint_mode(char sign)
{
	// Clearn OLED
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	// Fingerprint code place where
	// Print modes on OLED
	char str_fingerprint[30]={0};
//	memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	sprintf(str_fingerprint,"%s", "3.FINGERPRINT");
//	ssd1306_SetCursor(00, 00);
//	ssd1306_WriteString(str_fingerprint, Font_7x10, White);
	print_text_on_OLED(0, 1, false, "3.FINGERPRINT");

	// Print meu fingerprint
//	memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	sprintf(str_fingerprint,"%s", "1. Enroll finger");
//	ssd1306_SetCursor(00, 16);
//	ssd1306_WriteString(str_fingerprint, Font_7x10, White);
	print_text_on_OLED(0, 2, false, "1. Enroll finger");

//	memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	sprintf(str_fingerprint,"%s", "2. Delete all IDs");
//	ssd1306_SetCursor(00, 26);
//	ssd1306_WriteString(str_fingerprint, Font_7x10, White);
	print_text_on_OLED(0, 3, false, "2. Delete all IDs");

//	memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	sprintf(str_fingerprint,"%s", "3. Identify");
//	ssd1306_SetCursor(00, 36);
//	ssd1306_WriteString(str_fingerprint, Font_7x10, White);
	print_text_on_OLED(0, 4, false, "3. Identify");

//	memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	sprintf(str_fingerprint,"%s", "'*' to EXIT");
//	ssd1306_SetCursor(00, 46);
//	ssd1306_WriteString(str_fingerprint, Font_7x10, White);

//	ssd1306_UpdateScreen();
	print_text_on_OLED(0, 5, true, "'*' to EXIT");

	// Init fingerprint mogule
	touch_open(0);
	int u=0;
	for(u = 0; u<=3; u++)
	{
		HAL_Delay(50);
		touch_bakcklight(0);
		HAL_Delay(50);
		touch_bakcklight(1);
	}

	do                                                            // Whaite for choise
	{
		// Place for sensors code
		HAL_Delay(200);
		sign = read_one_sign_from_keyboard();                      // Read sign from keyboard
	    if(sign == '1')
	    {
	    	// Clear all OLED
	        ssd1306_Fill(Black);
	        ssd1306_UpdateScreen();

	        // Print mode in head
//	        memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	       	sprintf(str_fingerprint,"%s", "Set ID");
//	       	ssd1306_SetCursor(00, 00);
//	       	ssd1306_WriteString(str_fingerprint, Font_7x10, White);
//	        ssd1306_UpdateScreen();
	        print_text_on_OLED(0, 1, true, "Set ID");

	        HAL_Delay(1000);
	        // Set ID
	        char ID_set = 0;
	        ID_set = set_ID_number();
	        //ID_set = ID_set - 48;

	        memset(str_fingerprint, 0 , sizeof(str_fingerprint));
	        strcpy(str_fingerprint, "You set ID: ");
	        // Add ID to the end of string
	        for(int g=0; g<=(sizeof(str_fingerprint)); g++)
	        {
	        	if(str_fingerprint[g] == '\0')
	        	{
	        		str_fingerprint[g] = ID_set;
	        		break;
	        	}
	        }

//	       	sprintf(str_fingerprint,"%s", str_fingerprint);
//	       	ssd1306_SetCursor(00, 16);
//	       	ssd1306_WriteString(str_fingerprint, Font_7x10, White);
//	       	ssd1306_UpdateScreen();
	        print_text_on_OLED(0, 2, true, str_fingerprint);

	       	HAL_Delay(500);

	        // Відобразити процес еролменшу
	        my_identification_enroll_user(ID_set);

	    	int jjj = 9999;
	     }

	     if(sign == '2')
	     {
	        // Clear all OLED
	        ssd1306_Fill(Black);
	        ssd1306_UpdateScreen();
	        // Print mode in head

	        // Ptint selected menu
//	        memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	        sprintf(str_fingerprint,"%s", "2. Delete all IDs");
//	        ssd1306_SetCursor(00, 00);
//	        ssd1306_WriteString(str_fingerprint, Font_7x10, White);
	        print_text_on_OLED(0, 1, false, "2. Delete all IDs");

//	        memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	      	sprintf(str_fingerprint,"%s", "Deleting ...");
//	      	ssd1306_SetCursor(00, 16);
//	      	ssd1306_WriteString(str_fingerprint, Font_7x10, White);
//
//	      	ssd1306_UpdateScreen();
	        print_text_on_OLED(0, 2, true, "Deleting ...");

	      	HAL_Delay(2000);

	        touch_delete_all_fingerprints();

	        claen_oled_lines(true, true, true, true, true);			// Clear previous OLED line
	      }

	      if(sign == '3')
	      {
	    	  // Clear all OLED
	          ssd1306_Fill(Black);
	          ssd1306_UpdateScreen();
	          // Print mode in head

	          // Ptint selected menu
//	          memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	          sprintf(str_fingerprint,"%s", "3. Identify");
//	          ssd1306_SetCursor(00, 00);
//	          ssd1306_WriteString(str_fingerprint, Font_7x10, White);
//	          ssd1306_UpdateScreen();
	          print_text_on_OLED(0, 1, true, "3. Identify");

	          do                                                            // Whaite for choise
	          {
//	        	  	memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	        	 	sprintf(str_fingerprint,"%s", "Put your finger..");
//	        	 	ssd1306_SetCursor(00, 16);
//	        	 	ssd1306_WriteString(str_fingerprint, Font_7x10, White);
//	        	 	ssd1306_UpdateScreen();
	        	  	print_text_on_OLED(0, 2, true, "Put your finger..");

	        	 	//HAL_Delay(500);
	        	 	char ID = 0;
	        	 	ID = identify();

	        	 	if(ID == 0)														// If no any ID
	        	 	{
	        	 		// Make blinky
	        	 		static bool triger = false;
	        	 		if(triger == false)
	        	 		{
//	        	 			memset(str_fingerprint, 0 , sizeof(str_fingerprint));
//	        	 			sprintf(str_fingerprint,"%s", "Access denied   ");
//	        	 			ssd1306_SetCursor(00, 26);
//	        	 			ssd1306_WriteString(str_fingerprint, Font_7x10, White);
//	        	 			ssd1306_UpdateScreen();
	        	 			print_text_on_OLED(0, 3, true, "Access denied   ");
	        	 			HAL_Delay(400);

	        	 			triger = true;
	        	 		}
	        	 		else
	        	 		{
	        	 			claen_oled_lines(false, false, true, false, false);
	        	 			triger = false;
	        	 			HAL_Delay(400);
	        	 		}
	        	 	}
	        	 	// Print on OLED
	        	 	if(ID >=1)														// If ID founded
	        	 	{
	        	 		claen_oled_lines(false, true, true, false, false);			// Clear previous OLED line

	        	 		memset(str_fingerprint, 0 , sizeof(str_fingerprint));
	        	 		sprintf(str_fingerprint,"%s", "Your ID: ");					// Add ID to the end of it string

	        	 		// Find end of the string
	        	 		uint8_t i =0;
	        	 		bool find_flag = true;
	        	 		do{
	        	 			i++;
	        	 		}while (str_fingerprint[i] != '\0');
	        	 		// Add ID to the end of string
	        	 		str_fingerprint[i] = ID;

	        	 		//Print ID on OLED.
//	        	 		ssd1306_SetCursor(00, 26);
//	        	 		ssd1306_WriteString(str_fingerprint, Font_7x10, White);
//	        	 		ssd1306_UpdateScreen();
	        	 		print_text_on_OLED(0, 3, true, str_fingerprint);							//,,,,,,,,,,,, <<<<<<<<<<<< where Can be problem

	        	 		HAL_Delay(2000);
	        	 	}


	               sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

	               if(sign == '*')    // If select EXIT  // Exit in main menu
	               {
	                   // Clear all OLED
	                   ssd1306_Fill(Black);
	                   ssd1306_UpdateScreen();

	                   GPS_MODE = false;
	                   GSM_MODE = false;
	                   FINGERPRINT_MODE = false;
	                   SENSORS_MODE = false;

	                   return 1;          // Flag_fro exit from there
	                }
	           }while (sign != '*');     // Select EXIT
	       }

		   if(sign == '*')    // If select EXIT  // Exit in main menu
		   {
			    // Clear all OLED
				ssd1306_Fill(Black);
				ssd1306_UpdateScreen();

				GPS_MODE = false;
				GSM_MODE = false;
				FINGERPRINT_MODE = false;
				SENSORS_MODE = false;

				return 1;          // Flag_fro exit from there
			}
		   }while ((sign != '1') && (sign != '2') && (sign != '3') && (sign != '*') );     // Select one from 3 modes
}
// ----------------------------------------------------------------------------

int sensors_mode(char sign)
{
		// Clear all OLED
		ssd1306_Fill(Black);
		ssd1306_UpdateScreen();
		// Sensors code place where
		// Print mode in head
		char str_sensors[50]={0};
//		memset(str_sensors, 0 , sizeof(str_sensors));
//		sprintf(str_sensors,"%s", "4.SENSORS");
//		ssd1306_SetCursor(00, 00);
//		ssd1306_WriteString(str_sensors, Font_7x10, White);
//		memset(str_sensors, 0 , sizeof(str_sensors));
		print_text_on_OLED(0, 1, false, "4.SENSORS");

		// Print meu fingerprint
//		memset(str_sensors, 0 , sizeof(str_sensors));
//		sprintf(str_sensors,"%s", "1. Run all sensors");
//		ssd1306_SetCursor(00, 16);
//		ssd1306_WriteString(str_sensors, Font_7x10, White);
//		memset(str_sensors, 0 , sizeof(str_sensors));
		print_text_on_OLED(0, 2, false, "1. Run all sensors");

//		memset(str_sensors, 0 , sizeof(str_sensors));
//		sprintf(str_sensors,"%s", "2. function 2");
//		ssd1306_SetCursor(00, 26);
//		ssd1306_WriteString(str_sensors, Font_7x10, White);
//		memset(str_sensors, 0 , sizeof(str_sensors));
		print_text_on_OLED(0, 3, false, "2. function 2");


//		memset(str_sensors, 0 , sizeof(str_sensors));
//		sprintf(str_sensors,"%s", "3. function 3");
//		ssd1306_SetCursor(00, 36);
//		ssd1306_WriteString(str_sensors, Font_7x10, White);
//		memset(str_sensors, 0 , sizeof(str_sensors));
//		ssd1306_UpdateScreen();
		print_text_on_OLED(0, 4, true, "3. function 3");


		do                                                            // Whaite for choise
			{
			// Place for sensors code

			sign = read_one_sign_from_keyboard();                      // Read sign from keyboard
            if(sign == '1')
            {
            	// Clear all OLED
            	ssd1306_Fill(Black);
            	ssd1306_UpdateScreen();

            	// Ptint selected menu
//            	memset(str_sensors, 0 , sizeof(str_sensors));
//            	sprintf(str_sensors,"%s", "1. Run all sensors");
//            	ssd1306_SetCursor(00, 00);
//            	ssd1306_WriteString(str_sensors, Font_7x10, White);
//            	memset(str_sensors, 0 , sizeof(str_sensors));
//
//            	ssd1306_UpdateScreen();
            	print_text_on_OLED(0, 1, true, "1. Run all sensors");

            	do                                                            // Whaite for choise
            	{
            		// Place for code function 1
            		// Create timer for measure
            		read_T_and_H_SI7021();
            		int select_print_data = 4;							// Flag for print sensors data on OLED
            		OLED_prinr_all_data(select_print_data);

            		sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

            		if(sign == '*')    // If select EXIT  // Exit in main menu
            		{
            			// Stop timer for measure

            			// Clear all OLED
            			ssd1306_Fill(Black);
            			ssd1306_UpdateScreen();

            			GPS_MODE = false;
            			GSM_MODE = false;
            			FINGERPRINT_MODE = false;
            			SENSORS_MODE = false;

            			return 1;          // Flag_fro exit from there
            		}
            	}while (sign != '*');     // Select EXIT
            }

            if(sign == '2')
            {
                // Clear all OLED
                ssd1306_Fill(Black);
               	ssd1306_UpdateScreen();

                // Ptint selected menu
//                memset(str_sensors, 0 , sizeof(str_sensors));
//                sprintf(str_sensors,"%s", "1. function 2");
//                ssd1306_SetCursor(00, 00);
//                ssd1306_WriteString(str_sensors, Font_7x10, White);
//                memset(str_sensors, 0 , sizeof(str_sensors));
//
//                ssd1306_UpdateScreen();
               	print_text_on_OLED(0, 1, true, "1. function 2");

                do                                                            // Whaite for choise
                {
                       // Place for code function 2

                       sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

                       if(sign == '*')    // If select EXIT  // Exit in main menu
                       {
                            // Clear all OLED
                            ssd1306_Fill(Black);
                            ssd1306_UpdateScreen();

                            GPS_MODE = false;
                            GSM_MODE = false;
                            FINGERPRINT_MODE = false;
                            SENSORS_MODE = false;

                            return 1;          // Flag_fro exit from there
                       }
                 }while (sign != '*');     // Select EXIT

                // Place for code function 2
            }

            if(sign == '3')
            {
                 // Clear all OLED
                 ssd1306_Fill(Black);
                 ssd1306_UpdateScreen();
                 // Print mode in head

                 // Ptint selected menu
//                 memset(str_sensors, 0 , sizeof(str_sensors));
//                 sprintf(str_sensors,"%s", "1. function 3");
//                 ssd1306_SetCursor(00, 00);
//                 ssd1306_WriteString(str_sensors, Font_7x10, White);
//                 memset(str_sensors, 0 , sizeof(str_sensors));
//
//                 ssd1306_UpdateScreen();
                 print_text_on_OLED(0, 1, true, "1. function 3");

                 do                                                            // Whaite for choise
                 {
                      // Place for code function 3

                      sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

                      if(sign == '*')    // If select EXIT  // Exit in main menu
                      {
                           // Clear all OLED
                           ssd1306_Fill(Black);
                           ssd1306_UpdateScreen();

                           GPS_MODE = false;
                           GSM_MODE = false;
                           FINGERPRINT_MODE = false;
                           SENSORS_MODE = false;

                           return 1;          // Flag_fro exit from there
                      }
                  }while (sign != '*');     // Select EXIT

                 // Place for code function 3
            }

			if(sign == '*')    // If select EXIT  // Exit in main menu
			{
				// Clear all OLED
				ssd1306_Fill(Black);
				ssd1306_UpdateScreen();

				GPS_MODE = false;
				GSM_MODE = false;
				FINGERPRINT_MODE = false;
				SENSORS_MODE = false;

				return 1;          // Flag_fro exit from there
			}
		}while ((sign != '1') && (sign != '2') && (sign != '3') && (sign != '*') );     // Select one from 3 modes
}
// ----------------------------------------------------------------------------
/* After output call this function show call status
 */
void show_sratus_call(int call_status, char *str_gsm, char sign, uint8_t where_call)
{
	if (where_call == 1)   // If call to me
	{
		// 1. Clean OLED
//		int h = 16;
//		char str[30] = {0};
//		while(h != 46)
//		{
//			sprintf(str,"%s", "                    ");
//			ssd1306_SetCursor(00, h);
//			ssd1306_WriteString(str, Font_7x10, White);
//			memset(str, 0 , sizeof(str));
//
//			h = h +10;
//		}
//		ssd1306_UpdateScreen();
		claen_oled_lines(false, true, true, true, true);
		HAL_Delay(500);
	}

	if (call_status == 1)
	{
//		sprintf(str_gsm,"%s", "STATUS: Call out...");
//		ssd1306_SetCursor(00, 36);
//		ssd1306_WriteString(str_gsm, Font_7x10, White);
		print_text_on_OLED(0, 4, false, "STATUS: Call out...");

//		sprintf(str_gsm,"%s", "#: for end call");
//		ssd1306_SetCursor(00, 46);
//		ssd1306_WriteString(str_gsm, Font_7x10, White);
//		memset(str_gsm, 0 , sizeof(str_gsm));
//
//		ssd1306_UpdateScreen();
		print_text_on_OLED(0, 5, true, "#: for end call");
	}
	else
	{
//		sprintf(str_gsm,"%s", "STATUS: ERROR CALL");
//		ssd1306_SetCursor(00, 36);
//		ssd1306_WriteString(str_gsm, Font_7x10, White);
//		memset(str_gsm, 0 , sizeof(str_gsm));
//		ssd1306_UpdateScreen();
		print_text_on_OLED(0, 4, true, "STATUS: ERROR CALL");
	}
	// wait ansver from GSM module

	int gsm_out_call_stattus = 0;
	while ((sign != '#') && (gsm_out_call_stattus != 1) && (gsm_out_call_stattus != 2)
	&& (gsm_out_call_stattus != 3))
	{
		sign = read_one_sign_from_keyboard();
		gsm_out_call_stattus = wait_ansver_after_make_call_in_blok_mode();
	}

	if (sign == '#')
	{
		end_of_call();

//		sprintf(str_gsm,"%s", "STATUS: CALL END   ");
//		ssd1306_SetCursor(00, 36);
//		ssd1306_WriteString(str_gsm, Font_7x10, White);
//		memset(str_gsm, 0 , sizeof(str_gsm));
//		ssd1306_UpdateScreen();
		print_text_on_OLED(0, 4, true, "STATUS: CALL END   ");

		HAL_Delay(2000);
	}

	switch (gsm_out_call_stattus)
	{
		case 1:
			// Телефон збили
//			sprintf(str_gsm,"%s", "STATUS: BUSY           ");
//			ssd1306_SetCursor(00, 36);
//			ssd1306_WriteString(str_gsm, Font_7x10, White);
//			memset(str_gsm, 0 , sizeof(str_gsm));
//			ssd1306_UpdateScreen();
			print_text_on_OLED(0, 4, true, "STATUS: BUSY           ");

			HAL_Delay(2000);
			break;

		case 2:
			// Телефон не підняли
//			sprintf(str_gsm,"%s", "STATUS: NO ANSWER    ");
//			ssd1306_SetCursor(00, 36);
//			ssd1306_WriteString(str_gsm, Font_7x10, White);
//			memset(str_gsm, 0 , sizeof(str_gsm));
//			ssd1306_UpdateScreen();
			print_text_on_OLED(0, 4, true, "STATUS: NO ANSWER    ");

			HAL_Delay(2000);
			break;

		case 3:
			// Телефон підняли збили
//			sprintf(str_gsm,"%s", "STATUS: CALL END   ");
//			ssd1306_SetCursor(00, 36);
//			ssd1306_WriteString(str_gsm, Font_7x10, White);
//			memset(str_gsm, 0 , sizeof(str_gsm));
//			ssd1306_UpdateScreen();
			print_text_on_OLED(0, 4, true, "STATUS: CALL END   ");

			HAL_Delay(2000);
			break;

		}
}
// ----------------------------------------------------------------------------
char set_ID_number(void)
{
	char set_ID = 0;
	char sing_number = 0;
	bool entered_number_status = false;

	do{
		do{
			set_ID = read_one_sign_from_keyboard();
//			if(set_ID != 0)
//			{
//				set_ID = set_ID - 48;
//			}
		}while((set_ID <1) && (set_ID >9));

		HAL_Delay(200);
	}while(set_ID <= 0 );

	return set_ID;
}
// ----------------------------------------------------------------------------
/*	Function can writes mobile number.
 * For example 380XXXXXXXXX.
 * And delete wrong entered digits.
 * If entered all digits function finish work.
 */
bool enter_a_mobile_number(char * number)
{
	bool entered_number_status = false;
	char str[30] = {0};
	// 1. Clean OLED
	claen_oled_lines(false, true, true, true, true);
	HAL_Delay(500);
	//

    // 2. Type numbers
//	sprintf(str,"%s", "Enter number:");
//	ssd1306_SetCursor(00, 16);
//	ssd1306_WriteString(str, Font_7x10, White);
//	memset(str, 0 , sizeof(str));
//
//	ssd1306_UpdateScreen();
	print_text_on_OLED(0, 2, true, "Enter number:");

	uint8_t k = 0;
	char sing_number;
	//char number[13]={0};
	do{
		sing_number = read_one_sign_from_keyboard();
		if ((sing_number != '\0') && (sing_number != 'D'))  	// Enter one digit
		{
			number[k] = sing_number;
			k++;

			// print all mumbers
//			sprintf(str,"%s", number);
//			ssd1306_SetCursor(00, 26);
//			ssd1306_WriteString(str, Font_7x10, White);
//			memset(str, 0 , sizeof(str));
//			ssd1306_UpdateScreen();
			print_text_on_OLED(0, 3, true, number);

		}
		if((sing_number == 'D') && (k >=1))						// Delete one digit
		{
			k--;
			number[k] = ' ';

//			sprintf(str,"%s", number);
//			ssd1306_SetCursor(00, 26);
//			ssd1306_WriteString(str, Font_7x10, White);
//			memset(str, 0 , sizeof(str));
//			ssd1306_UpdateScreen();
			print_text_on_OLED(0, 3, true, number);
		}

		if(k == 12)												// If all digits was entered
		{
			entered_number_status = true;						// Status. All digit entered
			//break;
		}

		HAL_Delay(200);
	}while ((sing_number != '*') && (entered_number_status != true));

	return entered_number_status;
}
//// ----------------------------------------------------------------------------
//void claen_oled_lines(bool first, bool second, bool third, bool fourth, bool fifth)
//{
//	char str[32] = {0};
//	sprintf(str,"%s", "                          ");
//
//	if(first == true)
//	{
//		ssd1306_SetCursor(00, 00);
//		ssd1306_WriteString(str, Font_7x10, White);
//	}
//	if(second == true)
//	{
//		ssd1306_SetCursor(00, 16);
//		ssd1306_WriteString(str, Font_7x10, White);
//	}
//	if(third == true)
//	{
//		ssd1306_SetCursor(00, 26);
//		ssd1306_WriteString(str, Font_7x10, White);
//	}
//	if(fourth == true)
//	{
//		ssd1306_SetCursor(00, 36);
//		ssd1306_WriteString(str, Font_7x10, White);
//	}
//	if(fifth == true)
//	{
//		ssd1306_SetCursor(00, 46);
//		ssd1306_WriteString(str, Font_7x10, White);
//	}
//	ssd1306_UpdateScreen();
//}
// ----------------------------------------------------------------------------
/* Interrupt can detect signal "ring" from "ring" pin from GSM module.
 * That STATUS must check timer every 0.5 sec
 * If INCOMMING_RING_OR_SMS_STATUS == true
 * 		turn on "incoming call" function
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_1)
	{
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == GPIO_PIN_RESET)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
			INCOMMING_RING_OR_SMS_STATUS = true;				// Check it status if it is 'true' we have incoming ring or sms
		}
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == GPIO_PIN_SET)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
			INCOMMING_RING_OR_SMS_STATUS = false;
		}
		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

	}
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function to avoid 'sprintf() function'
// buffer: where save.
// string: what need to save.
// size: size of string.
void write_data_in_buffer(char * buffer, char *string, uint8_t size)
{
	for(uint8_t i = 0; i<= size; i++)
	{
		if(string[i] != '\0')
		{
			buffer[i] = string[i];
		}
	}
}
// ----------------------------------------------------------------------------
//void test_function(void)
//{
//	print_text_on_OLED(0, 1, true, "TEST STRING");
//	HAL_Delay(100);
//}
// ----------------------------------------------------------------------------




/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
