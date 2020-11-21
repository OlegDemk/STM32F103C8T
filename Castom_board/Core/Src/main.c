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

#include "fingerprint_GT_511C3.h"

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
#define SOUND ON
#define FINGERPRINT OFF

// Ьain modes devices
bool GSM_MODE = false;
bool GPS_MODE = false;
bool FINGERPRINT_MODE = false;
bool SENSORS_MODE = false;

bool EXIT_FROM_MODE = false;

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
	// GPS receive part///////////////////////////////////////////////////////
	uint8_t flag = 0;					// Flag signals what GPS buffer is full
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)    // was   void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	{
		flag = 1;
	}
	///////////////////////////////////////////////////////////////////////////
	// GSM
#endif
// ----------------------------------------------------------------------------



//// GSM variables
//char GSM_RX_buffer[60]={0};    // buffer for receive data ansver from GSM module
//char uart_1_answer_buffer[40] = {0};
//uint8_t ansver_flag =0;        // if ansver_flag == 1 data(ansver) are in buffer GSM_RX_buffer
//uint8_t count =0;

// variable for init gsm
bool at_ready = false;
bool ast_poweron = false;
bool creg_2 = false;
bool call_ready = false;
bool creg_1 = false;
//

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
//  struct
//  {
//	  char gps_lat[11];
//	  char gps_lat_letter[2];
//	  char gps_lon[12];
//	  char gps_lon_letter[2];
//	  char gps_time[6];
//      uint8_t gps_speed;
//	  uint8_t gps_number_of_satellites_GPGGA;
//
//	  char temperature_si7021[4];
//	  int humidity_si7021;
//
//	  int temperature_am3202;
//	  int humidity_am3202;
//
//  } global_data;
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

  	  HAL_TIM_Base_Start_IT(&htim2);

	#if GPS
//  	  uint8_t GPS_buff[512];      						// main buffer for stream from GPS
  	  memset(GPS_buff ,0 ,sizeof(GPS_buff));
  	  HAL_UART_Receive_DMA(&huart3, GPS_buff, 512);
	#endif



   //choose_mode();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  #if SOUND
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
  #endif

  	int EXIT = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////
while (1)
{
	EXIT = 0;

	char str[50]={0};
	char sign = 0;

	print_main_menu();  // Print main menu on OLED

	do                                                            // Whaite for choise
	{
		sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

		// For debug ///////////
		//sign = '1';
		/////////////////////

		if(sign == '1')    // If select GSM mode
		{
			GPS_MODE = true;
		}
		if(sign == '2')    // If select GPS mode
		{
			GSM_MODE = true;
		}
		if(sign == '3')    // If select GPS mode
		{
			FINGERPRINT_MODE = true;
		}
		if(sign == '4')    // If select GPS mode
		{
			SENSORS_MODE = true;
		}

		ssd1306_UpdateScreen();
		}while ((sign != '1') && (sign != '2') && (sign != '3') && (sign != '4'));     // Select one from 3 modes

    // MAIN MODES

	while(GSM_MODE && (EXIT == 0))  /////////////////////////////////////////////////////////////////////
	{
		// EXIT = gsm_mode(sign);

		ssd1306_Fill(Black);
		ssd1306_UpdateScreen();
		// Print mode in head
		sprintf(str,"%s", "2.GSM: waiting...");
		ssd1306_SetCursor(00, 00);
		ssd1306_WriteString(str, Font_7x10, White);
		ssd1306_UpdateScreen();


		// Init GSM module////////////////////////////
		init_GSM_uart_comunication();
		if(init_gsm_module() == HAL_OK)
		{
			// init OK
			GSM_INIT = 1;

			sprintf(str,"%s", "                    ");
			ssd1306_SetCursor(00, 00);
			ssd1306_WriteString(str, Font_7x10, White);
			ssd1306_UpdateScreen();

			sprintf(str,"%s", "2.GSM: OK");
			ssd1306_SetCursor(00, 00);
			ssd1306_WriteString(str, Font_7x10, White);
			ssd1306_UpdateScreen();
		}
		else
		{
			// GSM didn't init
			GSM_INIT = 0;

			sprintf(str,"%s", "                    ");
			ssd1306_SetCursor(00, 00);
			ssd1306_WriteString(str, Font_7x10, White);
			ssd1306_UpdateScreen();

			sprintf(str,"%s", "2.GSM: ERROR");
			ssd1306_SetCursor(00, 00);
			ssd1306_WriteString(str, Font_7x10, White);
			ssd1306_UpdateScreen();
		}
		// END INIT GSM MODULE  //////////////////////////

        if(GSM_INIT == 1)
        {
			// Print GSM menu
			sprintf(str,"%s", "1.For make CALL");
			ssd1306_SetCursor(00, 16);
			ssd1306_WriteString(str, Font_7x10, White);
			memset(str, 0 , sizeof(str));

			sprintf(str,"%s", "2.For send SMS");
			ssd1306_SetCursor(00, 26);
			ssd1306_WriteString(str, Font_7x10, White);
			memset(str, 0 , sizeof(str));

			ssd1306_UpdateScreen();

			// 2. Написати функції які.
			// 		1. Звонить і має прийняти однв з відповідей:
			//          1. Телефон збили
			//			2. Телефон не підняли
			//			3. Телефон піднятий
			//      В момент звінка можна кнопкою збити вихідний звінок
			//
			//      2. Збити вихідний звінок
			//
			do                                                            // Whaite for choise
			{
				sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

				if(sign == '1')    // If select Call mode
				{

				}
				if(sign == '2')    // If select SMS mode
				{


				}
				if(sign == '*')    // If select EXIT  // Exit in main menu
				{
					EXIT = 1;      // Flag_fro exit from there
					// Clear all OLED
					ssd1306_Fill(Black);
					ssd1306_UpdateScreen();

					GPS_MODE = false;
					GSM_MODE = false;
					FINGERPRINT_MODE = false;
					SENSORS_MODE = false;
				}

				}while ((sign != '1') && (sign != '2') && (sign != '*') );     // Select one from 3 modes
			}
        else
        {
        	HAL_Delay(2000);

        	// Exit from GSM menu
        	EXIT = 1;      // Flag_fro exit from there
        	// Clear all OLED
        	ssd1306_Fill(Black);
        	ssd1306_UpdateScreen();

        	GPS_MODE = false;
        	GSM_MODE = false;
        	FINGERPRINT_MODE = false;
        	SENSORS_MODE = false;
        }

	}
	while (GPS_MODE && (EXIT == 0)) /////////////////////////////////////////////////////////////////////
	{
		EXIT = gps_mode(sign);
	}

	while (FINGERPRINT_MODE && (EXIT == 0)) /////////////////////////////////////////////////////////////////////
	{
		ssd1306_Fill(Black);
		ssd1306_UpdateScreen();
		// Fingerprint code place where
		// Print mode in head
		memset(str, 0 , sizeof(str));
		sprintf(str,"%s", "3.FINGERPRINT");
		ssd1306_SetCursor(00, 00);
		ssd1306_WriteString(str, Font_7x10, White);
		memset(str, 0 , sizeof(str));

		// Print meu fingerprint
		memset(str, 0 , sizeof(str));
		sprintf(str,"%s", "1. function 1");
		ssd1306_SetCursor(00, 16);
		ssd1306_WriteString(str, Font_7x10, White);
		memset(str, 0 , sizeof(str));

		memset(str, 0 , sizeof(str));
		sprintf(str,"%s", "2. function 2");
		ssd1306_SetCursor(00, 26);
		ssd1306_WriteString(str, Font_7x10, White);
		memset(str, 0 , sizeof(str));

		memset(str, 0 , sizeof(str));
		sprintf(str,"%s", "3. function 3");
		ssd1306_SetCursor(00, 36);
		ssd1306_WriteString(str, Font_7x10, White);
		memset(str, 0 , sizeof(str));

		ssd1306_UpdateScreen();

		do                                                            // Whaite for choise
		{
			// Place for sensors code

			sign = read_one_sign_from_keyboard();                      // Read sign from keyboard
		    if(sign == '1')
		    {
		    	// Clear all OLED
		        ssd1306_Fill(Black);
		        ssd1306_UpdateScreen();
		        // Print mode in head

		        // Ptint selected menu
		        memset(str, 0 , sizeof(str));
		        sprintf(str,"%s", "1. function 1");
		        ssd1306_SetCursor(00, 00);
		        ssd1306_WriteString(str, Font_7x10, White);
		        memset(str, 0 , sizeof(str));

		        ssd1306_UpdateScreen();

		        do                                                            // Whaite for choise
		        {
		            // Place for code function 1

		            sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

		            if(sign == '*')    // If select EXIT  // Exit in main menu
		            {
		            	EXIT = 1;      // Flag_fro exit from there
		            	// Clear all OLED
		            	ssd1306_Fill(Black);
		            	ssd1306_UpdateScreen();

		            	GPS_MODE = false;
		            	GSM_MODE = false;
		            	FINGERPRINT_MODE = false;
		            	SENSORS_MODE = false;
		            }
		         }while (sign != '*');     // Select EXIT
		     }

		     if(sign == '2')
		     {
		        // Clear all OLED
		        ssd1306_Fill(Black);
		        ssd1306_UpdateScreen();
		        // Print mode in head

		        // Ptint selected menu
		        memset(str, 0 , sizeof(str));
		        sprintf(str,"%s", "1. function 2");
		        ssd1306_SetCursor(00, 00);
		        ssd1306_WriteString(str, Font_7x10, White);
		        memset(str, 0 , sizeof(str));

		        ssd1306_UpdateScreen();

		        do                                                            // Whaite for choise
		        {
		        	// Place for code function 2

		        	sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

		            if(sign == '*')    // If select EXIT  // Exit in main menu
		            {
		             	EXIT = 1;      // Flag_fro exit from there
		                // Clear all OLED
		                ssd1306_Fill(Black);
		                ssd1306_UpdateScreen();

		                GPS_MODE = false;
		                GSM_MODE = false;
		                FINGERPRINT_MODE = false;
		                SENSORS_MODE = false;
		            }
		         }while (sign != '*');     // Select EXIT
		      }

		      if(sign == '3')
		      {
		    	  // Clear all OLED
		          ssd1306_Fill(Black);
		          ssd1306_UpdateScreen();
		          // Print mode in head

		          // Ptint selected menu
		          memset(str, 0 , sizeof(str));
		          sprintf(str,"%s", "1. function 3");
		          ssd1306_SetCursor(00, 00);
		          ssd1306_WriteString(str, Font_7x10, White);
		          memset(str, 0 , sizeof(str));

		          ssd1306_UpdateScreen();

		          do                                                            // Whaite for choise
		          {
		        	  // Place for code function 3

		               sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

		               if(sign == '*')    // If select EXIT  // Exit in main menu
		               {
		                   EXIT = 1;      // Flag_fro exit from there
		                   // Clear all OLED
		                   ssd1306_Fill(Black);
		                   ssd1306_UpdateScreen();

		                   GPS_MODE = false;
		                   GSM_MODE = false;
		                   FINGERPRINT_MODE = false;
		                   SENSORS_MODE = false;
		                }
		           }while (sign != '*');     // Select EXIT
		       }

			   if(sign == '*')    // If select EXIT  // Exit in main menu
			   {
				   EXIT = 1;      // Flag_fro exit from there
				    // Clear all OLED
					ssd1306_Fill(Black);
					ssd1306_UpdateScreen();

					GPS_MODE = false;
					GSM_MODE = false;
					FINGERPRINT_MODE = false;
					SENSORS_MODE = false;
				}
			   }while ((sign != '1') && (sign != '2') && (sign != '3') && (sign != '*') );     // Select one from 3 modes

	}

	while (SENSORS_MODE && (EXIT == 0))  /////////////////////////////////////////////////////////////////////
	{
		ssd1306_Fill(Black);
		ssd1306_UpdateScreen();
		// Sensors code place where
		// Print mode in head
		memset(str, 0 , sizeof(str));
		sprintf(str,"%s", "4.SENSORS");
		ssd1306_SetCursor(00, 00);
		ssd1306_WriteString(str, Font_7x10, White);
		memset(str, 0 , sizeof(str));

		// Print meu fingerprint
		memset(str, 0 , sizeof(str));
		sprintf(str,"%s", "1. Run all sensors");
		ssd1306_SetCursor(00, 16);
		ssd1306_WriteString(str, Font_7x10, White);
		memset(str, 0 , sizeof(str));

		memset(str, 0 , sizeof(str));
		sprintf(str,"%s", "2. function 2");
		ssd1306_SetCursor(00, 26);
		ssd1306_WriteString(str, Font_7x10, White);
		memset(str, 0 , sizeof(str));

		memset(str, 0 , sizeof(str));
		sprintf(str,"%s", "3. function 3");
		ssd1306_SetCursor(00, 36);
		ssd1306_WriteString(str, Font_7x10, White);
		memset(str, 0 , sizeof(str));

		ssd1306_UpdateScreen();

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
            	memset(str, 0 , sizeof(str));
            	sprintf(str,"%s", "1. Run all sensors");
            	ssd1306_SetCursor(00, 00);
            	ssd1306_WriteString(str, Font_7x10, White);
            	memset(str, 0 , sizeof(str));

            	ssd1306_UpdateScreen();

            	do                                                            // Whaite for choise
            	{
            		// Place for code function 1

            		sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

            		if(sign == '*')    // If select EXIT  // Exit in main menu
            		{
            			EXIT = 1;      // Flag_fro exit from there
            			// Clear all OLED
            			ssd1306_Fill(Black);
            			ssd1306_UpdateScreen();

            			GPS_MODE = false;
            			GSM_MODE = false;
            			FINGERPRINT_MODE = false;
            			SENSORS_MODE = false;
            		}
            	}while (sign != '*');     // Select EXIT
            }

            if(sign == '2')
            {
                // Clear all OLED
                ssd1306_Fill(Black);
               	ssd1306_UpdateScreen();

                // Ptint selected menu
                memset(str, 0 , sizeof(str));
                sprintf(str,"%s", "1. function 2");
                ssd1306_SetCursor(00, 00);
                ssd1306_WriteString(str, Font_7x10, White);
                memset(str, 0 , sizeof(str));

                ssd1306_UpdateScreen();

                do                                                            // Whaite for choise
                {
                       // Place for code function 2

                       sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

                       if(sign == '*')    // If select EXIT  // Exit in main menu
                       {
                            EXIT = 1;      // Flag_fro exit from there
                            // Clear all OLED
                            ssd1306_Fill(Black);
                            ssd1306_UpdateScreen();

                            GPS_MODE = false;
                            GSM_MODE = false;
                            FINGERPRINT_MODE = false;
                            SENSORS_MODE = false;
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
                 memset(str, 0 , sizeof(str));
                 sprintf(str,"%s", "1. function 3");
                 ssd1306_SetCursor(00, 00);
                 ssd1306_WriteString(str, Font_7x10, White);
                 memset(str, 0 , sizeof(str));

                 ssd1306_UpdateScreen();

                 do                                                            // Whaite for choise
                 {
                      // Place for code function 3

                      sign = read_one_sign_from_keyboard();                      // Read sign from keyboard

                      if(sign == '*')    // If select EXIT  // Exit in main menu
                      {
                           EXIT = 1;      // Flag_fro exit from there
                           // Clear all OLED
                           ssd1306_Fill(Black);
                           ssd1306_UpdateScreen();

                           GPS_MODE = false;
                           GSM_MODE = false;
                           FINGERPRINT_MODE = false;
                           SENSORS_MODE = false;
                      }
                  }while (sign != '*');     // Select EXIT

                 // Place for code function 3
            }

			if(sign == '*')    // If select EXIT  // Exit in main menu
			{
				EXIT = 1;      // Flag_fro exit from there
				// Clear all OLED
				ssd1306_Fill(Black);
				ssd1306_UpdateScreen();

				GPS_MODE = false;
				GSM_MODE = false;
				FINGERPRINT_MODE = false;
				SENSORS_MODE = false;
			}
		}while ((sign != '1') && (sign != '2') && (sign != '3') && (sign != '*') );     // Select one from 3 modes

	}



///////////////////////////////////////////////////////////////////////////////////////
	    // MAIN LOOP
//        if(interrupt_flag == 1)
//        {
//        	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
//
//
//        	read_T_and_H_SI7021();
//        	test_flash_W25Q();
//
//			#if GPS
//        		parsing_GPS(GPS_buff, 512);
//        		HAL_Delay(500);
//			# else
//        		HAL_Delay(500);
//			#endif
//
//			#if OLED
//        		OLED_prinr_all_data();
//			#endif
//
//        	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
//
//			#if FINGERPRINT
//        		fingerprint_test();
//			#endif
//
//        	interrupt_flag = 0;
//        }
//////////////////////////////////////////////////////////////////////////////////
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
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

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
	 HAL_TIM_PWM_Stop(&timer, channel); // stop generation of pwm
	 TIM_OC_InitTypeDef sConfigOC;
	 timer.Init.Prescaler = 720;
	 timer.Init.Period = period; // set the period duration
	 HAL_TIM_PWM_Init(&timer); // reinititialise with new period value
	 sConfigOC.OCMode = TIM_OCMODE_PWM1;
	 sConfigOC.Pulse = pulse; // set the pulse duration
	 sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	 sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	 HAL_TIM_PWM_ConfigChannel(&timer, &sConfigOC, channel);
	 HAL_TIM_PWM_Start(&timer, channel); // start pwm generation
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
	  //
	  //HAL_Delay(100);

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
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1);     // For detect 123A
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
		 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);     // For detect 123A
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
		   		 		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);     // For detect 123A
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
int gps_mode(sign)
{
	// Clearn OLED
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	// Print mode in head
	char str_gps[50]={0};
	memset(str_gps, 0 , sizeof(str_gps));
	sprintf(str_gps,"%s", "1.GPS: waiting...");
	ssd1306_SetCursor(00, 00);
	ssd1306_WriteString(str_gps, Font_7x10, White);
	memset(str_gps, 0 , sizeof(str_gps));

	ssd1306_UpdateScreen();

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
			int GPS_SELECTED = 1;							// Flag for print GPS data on OLED
			OLED_prinr_all_data(GPS_SELECTED);

			if(GPGGA_data_is_ready == 1)					// Check if data from GPS device was correct ( parsed GPGLL line correct)
			{
				// Print the data that GPS is valid
				memset(str_gps, 0 , sizeof(str_gps));
				sprintf(str_gps,"%s", "1.GPS: OK            ");
				ssd1306_SetCursor(00, 00);
				ssd1306_WriteString(str_gps, Font_7x10, White);
				memset(str_gps, 0 , sizeof(str_gps));

				//receive_gps_signal = 0;
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

					memset(str_gps, 0 , sizeof(str_gps));
					sprintf(str_gps,"%s", "1.GPS: NO SIGNAL  ");
					ssd1306_SetCursor(00, 00);
					ssd1306_WriteString(str_gps, Font_7x10, White);
					memset(str_gps, 0 , sizeof(str_gps));
				}
			}

			ssd1306_UpdateScreen();
		}
	}while ( (sign != '*'));     // Select one from 3 modes
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
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
