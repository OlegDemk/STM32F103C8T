/*
 * GSM_IOT_GA6.c
 *
 *  Created on: Oct 8, 2020
 *      Author: odemki
 */
#include "stdio.h"
#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;

char call_to_my_mobile_number[]="ATD+380931482354\r\n";

// -------------------------------------------------------------------
// Transmet commands
void call_from_GSM_module(void)
{
//	uint8_t size=0;
//	char str[40]={0};
//	uint8_t size_mas = sizeof(str);
//	memset(str, 0 , sizeof(str));
//
//	sprintf(str,call_to_my_mobile_number);      							// AT command
//	size=sizeof(str);
//	HAL_UART_Transmit(&huart1 , (uint8_t *)str, size, 0xFFFF);



//	uint8_t size=0;
//		char str[40]={0};
//		uint8_t size_mas = sizeof(str);
//		memset(str, 0 , sizeof(str));
//
//		sprintf(str,"AT\r\n");      							// AT command
//		size=sizeof(str);
//		HAL_UART_Transmit(&huart1 , (uint8_t *)str, size, 0xFFFF);
}
// -------------------------------------------------------------------
void send_sms_from_GSM(void)
{
//	uint8_t size=0;
//	char str[40]={0};
//	uint8_t size_mas = sizeof(str);
//	memset(str, 0 , sizeof(str));
//
//	sprintf(str, "AT+CMGS=1");      							// AT command
//	size=sizeof(str);
//	HAL_UART_Transmit(&huart1 , (uint8_t *)str, size, 0xFFFF);
//
//	HAL_Delay(2000);
//
//	sprintf(str, "");      							// AT command
//    size=sizeof(str);
//    HAL_UART_Transmit(&huart1 , (uint8_t *)str, size, 0xFFFF);


}
// -------------------------------------------------------------------














//
