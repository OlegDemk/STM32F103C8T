/*
 * Humidity_and_Temperature_SI7021_sensor.c
 *
 *  Created on: Sep 2, 2020
 *      Author: odemki
 */
#include <math.h>
#include "stdio.h"
#include "stm32f1xx_hal.h"
#include "Si7021_driver.h"

#include "main.h"

extern UART_HandleTypeDef huart1;
// H and T
// --------------------------------------------------------------------------------
void read_firmware(void)
{
	int8_t firmware_version = 0;
    char str[30]={0};
    uint8_t size=0;

	firmware_version = r_firmware_rev_Si7021();

	// Print in COM port
	sprintf(str,"version: %d\r\n", firmware_version);      										// convert   in  str
	size=sizeof(str);
	HAL_UART_Transmit(&huart1 , (uint8_t *)str, size, 0xFFFF);
	memset(str, 0 , sizeof(str));
}
// --------------------------------------------------------------------------------
void read_T_and_H_SI7021(void)
{
	uint8_t size=0;
	char str[10]={0};
	int8_t STATUS = -1;
	float data = 0;

	// Read temperature ///////////////////////////////
	STATUS = r_single_Si7021(&data, Temperature);
	if(STATUS != 0)       	// If error
	{
		while(1){}
	}

	// For avoid float print
	char *tmpSign = (data < 0) ? "-" : "";
	float tmpVal = (data < 0) ? -data : data;

	int tmpInt1 = tmpVal;
	float tmpFrac = (data - tmpInt1);
	int tmpInt2 = trunc(tmpFrac * 10000);

	// Print Temperature in COM port
	sprintf(str,"T:%s%d.%04d C\r\n", tmpSign, tmpInt1, tmpInt2);      										// convert   in  str
	size=sizeof(str);
	HAL_UART_Transmit(&huart1 , (uint8_t *)str, size, 0xFFFF);
	memset(str, 0 , sizeof(str));

	// Save in global variable
	sprintf(str,"T:%s%d\0", tmpSign, tmpInt1);
	strcpy(temperature_si7021, str);
	memset(str, 0 , sizeof(str));

	//// Read humidity `///////////////////////////////////
	STATUS = -1;
	STATUS = r_single_Si7021(&data, Humidity);
	if(STATUS != 0)       	// If error
	{
		while(1){}
	}

	//// For avoid float print
	//// char *tmpSign = (data < 0) ? "-" : "";
	tmpVal = (data < 0) ? -data : data;

	tmpInt1 = tmpVal;
	tmpFrac = (data - tmpInt1);
	tmpInt2 = trunc(tmpFrac * 10000);

	// Print in COM port
	sprintf(str,"H:%d.%01d %% \r\n", tmpInt1, tmpInt2);      										// convert   in  str
	size=sizeof(str);
	HAL_UART_Transmit(&huart1 , (uint8_t *)str, size, 0xFFFF);
	memset(str, 0 , sizeof(str));

	// Save in global variable
	sprintf(str,"H:%d\0", tmpInt1);
	strcpy(humidity_si7021, str);
	memset(str, 0 , sizeof(str));
}
// --------------------------------------------------------------------------------

