/*
 * oled_ssd1306.c
 *
 *  Created on: Sep 1, 2020
 *      Author: odemki
 */
#include "ssd1306.h"
#include "fonts.h"

extern TIM_HandleTypeDef htim1;

// -----------------------------------------------------------------------------------
void init_oled(void)
{
	ssd1306_Init();
	HAL_Delay(500);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
}
// -----------------------------------------------------------------------------------
void test_oled(void)
{
	uint8_t i = 0;
	for (i=0; i<=46; i++)     // 64 - 18
	{
		//HAL_TIM_Base_Stop_IT(&htim1);

		ssd1306_SetCursor(0,i);
		ssd1306_WriteString("HELLO !!!",Font_11x18,White);
		ssd1306_UpdateScreen();

		//HAL_TIM_Base_Start_IT(&htim1);

		HAL_Delay(10);
	}
	if(i >= 46)
	{
		for(i = 46; i>0; i--)
		{
			//HAL_TIM_Base_Stop_IT(&htim1);

			ssd1306_SetCursor(0,i);
			ssd1306_WriteString("HELLO !!!",Font_11x18,White);
			ssd1306_UpdateScreen();

			//HAL_TIM_Base_Start_IT(&htim1);

			HAL_Delay(10);
		}
	}
}
// -----------------------------------------------------------------------------------
