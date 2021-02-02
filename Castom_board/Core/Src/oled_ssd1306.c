/*
 * oled_ssd1306.c
 *
 *  Created on: Sep 1, 2020
 *      Author: odemki
 */
#include "ssd1306.h"
#include "fonts.h"
#include <string.h>
#include "stdbool.h"

#include "main.h"

#include "GSM_IOT_GA6.h"

extern TIM_HandleTypeDef htim1;

extern int GPGGA_data_is_ready;   				// Flag. If time data is ready then print it on OLED.

void print_GPS_data(void);
void print_all_sensors_data(void);
void print_GSM_data(void);
void print_fingerprint_data(void);

// -----------------------------------------------------------------------------------
void init_oled(void)
{
	ssd1306_Init();
	HAL_Delay(500);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
}
// -----------------------------------------------------------------------------------
void OLED_prinr_all_data(int  select_print_data)
{
	switch (select_print_data)
	{
		case 1:
			print_GPS_data();
			break;
		case 2:
			print_GSM_data();
			break;
		case 3:
			print_fingerprint_data();
			break;
		case 4:
			print_all_sensors_data();
			break;
	}

//    // Print all data in OLED
    ssd1306_UpdateScreen();
}
// -----------------------------------------------------------------------------------
void print_main_menu(void)
{
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();

	char str[50]={0};
	// Print message
	sprintf(str,"%s", " SELECT MODE...");
	ssd1306_SetCursor(00, 00);
	ssd1306_WriteString(str, Font_7x10, White);

	sprintf(str,"%s", "1.GSM MODE");
	ssd1306_SetCursor(00, 16);
	ssd1306_WriteString(str, Font_7x10, White);
	memset(str, 0 , sizeof(str));

	sprintf(str,"%s", "2.GPS MODE");
	ssd1306_SetCursor(00, 26);
	ssd1306_WriteString(str, Font_7x10, White);
	memset(str, 0 , sizeof(str));

	sprintf(str,"%s", "3.FINGERPRINT MODE");
	ssd1306_SetCursor(00, 36);
	ssd1306_WriteString(str, Font_7x10, White);
	memset(str, 0 , sizeof(str));

	sprintf(str,"%s", "4.SENSORS MODE");
	ssd1306_SetCursor(00, 46);
	ssd1306_WriteString(str, Font_7x10, White);
	memset(str, 0 , sizeof(str));

	ssd1306_UpdateScreen();
}
// -----------------------------------------------------------------------------------
void print_GPS_data(void)
{
	// 1. Print Lat and Lon ////////////////////////////////
	char str[50]={0};
	memset(str, 0 , sizeof(str));

	// Print data from GPS module  ( Lat )
	char lat_str[]="Lat:";
	memcpy(str, lat_str, sizeof(lat_str));
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString(str, Font_7x10, White);
	memset(str, 0 , sizeof(str));
	// Print data
	ssd1306_SetCursor(30, 16);
	ssd1306_WriteString(gps_latitude, Font_7x10, White);

	// Print data from GPS module  ( Lon )
	char lon_str[]="Lon:";
	memcpy(str, lon_str, sizeof(lon_str));
	ssd1306_SetCursor(0, 26);
	ssd1306_WriteString(str, Font_7x10, White);
	memset(str, 0 , sizeof(str));

	ssd1306_SetCursor(30, 26);
	ssd1306_WriteString(gps_lontitude, Font_7x10, White);

	// 2. Print time  ////////////////////////////////
	if (GPGGA_data_is_ready == 1)
	{
		char hours[2]={0};
		char minutes[2]={0};
		char semicolum = ':';

		// divide hours and minutes
		uint8_t i=0;
		for(i=0; i<=3; i++)
		{
			if(i<=1)
			{
				hours[i] = gps_time[i];
			}
			else
			{
				minutes[i-2] = gps_time[i];
			}
		}
		// Add 3 hours
		int int_hours = 0;
		uint8_t dozens = 0;
		uint8_t units = 0;

		dozens = hours[0];
		units = hours[1];
		dozens = dozens - 48;
		units = units - 48;

		int_hours = units +(dozens *10);
		int_hours = int_hours+2;

		dozens = (int_hours/10) + 48;
		units = (int_hours%10) + 48;
		hours[0] = dozens;
		hours[1] = units;

		if(int_hours == 24)
		{
			hours[0] = '0';
			hours[1] = '0';
		}
		if(int_hours == 25)
		{
			hours[0] = '0';
			hours[1] = '1';
		}
		if(int_hours == 26)
		{
			hours[0] = '0';
			hours[1] = '2';
		}

		if((int_hours >=0) && (int_hours<=9))
		{
			hours[0] = '0';
			hours[1] = int_hours +48;
		}

		// connect  hours and minutes with colon
		//i=5;
		for(i=0; i<5; i++)
		{
			if(i<=1)
			{
				str[i] = hours[i];
			}
			else
			{
				if(i==2)
				{
					str[i] = semicolum;
				}
				else
				{
					str[i]=minutes[i-3];
				}
			}
		}

		ssd1306_SetCursor(0, 36);
		ssd1306_WriteString(str, Font_7x10, White);
	}

	// 3. Print number of satalits  ////////////////////////////////
	char numbers_of_satalits_str[]="N:  ";
	memcpy(str, numbers_of_satalits_str, sizeof(numbers_of_satalits_str));
	ssd1306_SetCursor(40, 36);
	ssd1306_WriteString(str, Font_7x10, White);

	memset(str, 0 , sizeof(str));
	ssd1306_SetCursor(55, 36);
	ssd1306_WriteString(gps_number_of_satellites, Font_7x10, White);

	// 4. Print number of satalits  ////////////////////////////////
	//gps_speed
	char speed_str[]="S:    ";
	memcpy(str, speed_str, sizeof(speed_str));
	ssd1306_SetCursor(75, 36);
	ssd1306_WriteString(str, Font_7x10, White);

	memset(str, 0 , sizeof(str));
	ssd1306_SetCursor(90, 36);
	ssd1306_WriteString(gps_speed, Font_7x10, White);
}
// -----------------------------------------------------------------------------------
void print_all_sensors_data(void)
{
	char str_1[40]={0};

	// 1. Print data from si7021 sensor
	strcpy(str_1, "1.si7021 ");
	strcat(str_1, temperature_si7021);
	strcat(str_1, humidity_si7021);

	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString(str_1, Font_7x10, White);

	memset(str_1, 0 , sizeof(str_1));

	// 2. Print data from AM2302
	strcpy(str_1, "2.AM2302 ");
	strcat(str_1, temperature_am3202);
	strcat(str_1, humidity_am3202);

	ssd1306_SetCursor(0, 26);
	ssd1306_WriteString(str_1, Font_7x10, White);

	memset(str_1, 0 , sizeof(str_1));

	// 3. Print data from 9066
	strcpy(str_1, "2.9066 ");

	ssd1306_SetCursor(0, 36);
	ssd1306_WriteString(str_1, Font_7x10, White);

	memset(str_1, 0 , sizeof(str_1));
}
// -----------------------------------------------------------------------------------
void print_GSM_data(void)
{

}
// -----------------------------------------------------------------------------------
void print_fingerprint_data(void)
{

}
// -----------------------------------------------------------------------------------
void claen_oled_lines(bool first, bool second, bool third, bool fourth, bool fifth)
{
	char str[32] = {0};
	sprintf(str,"%s", "                          ");

	if(first == true)
	{
		ssd1306_SetCursor(00, 00);
		ssd1306_WriteString(str, Font_7x10, White);
	}
	if(second == true)
	{
		ssd1306_SetCursor(00, 16);
		ssd1306_WriteString(str, Font_7x10, White);
	}
	if(third == true)
	{
		ssd1306_SetCursor(00, 26);
		ssd1306_WriteString(str, Font_7x10, White);
	}
	if(fourth == true)
	{
		ssd1306_SetCursor(00, 36);
		ssd1306_WriteString(str, Font_7x10, White);
	}
	if(fifth == true)
	{
		ssd1306_SetCursor(00, 46);
		ssd1306_WriteString(str, Font_7x10, White);
	}
	ssd1306_UpdateScreen();
}
// -----------------------------------------------------------------------------------
// Print any text on OLED
void print_text_on_OLED(uint8_t column, uint8_t row, bool update_oled, char text[])
{
	char message_buffer[30] = {0};

	switch (row)
	{
		case 1:
			ssd1306_SetCursor(column, 0);
		break;

		case 2:
			ssd1306_SetCursor(column, 16);
		break;

		case 3:
			ssd1306_SetCursor(column, 26);
		break;

		case 4:
			ssd1306_SetCursor(column, 16);
		break;

		case 5:
			ssd1306_SetCursor(column, 16);
		break;
	}
	ssd1306_WriteString(text, Font_7x10, White);

	if(update_oled == true)
	{
		ssd1306_UpdateScreen();
	}
}
// ----------------------------------------------------------------------------








//
