/*
 * oled_ssd1306.c
 *
 *  Created on: Sep 1, 2020
 *      Author: odemki
 */
#include "ssd1306.h"
#include "fonts.h"

#include "main.h"

#include "GSM_IOT_GA6.h"

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
void OLED_prinr_all_data(int GPS_SELECTED)
{
    // Print all data in OLED
    if(GPS_SELECTED == 1)
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
    //    	if(GSM_SELECTED == 0)
    //    	{
    //
    //    	}
    //    	if(FINGERPEINT_SELECTED == 0)
    //    	{
    //
    //    	}
    //    	if(SENSORS_SELECTED == 0)
    //        {
    //
    //        }

    ssd1306_UpdateScreen();
}


//	// Print temperature and humidity 'si7021'
//	ssd1306_SetCursor(0, 0);
//	ssd1306_WriteString(temperature_si7021, Font_7x10, White);
//
//	ssd1306_SetCursor(35, 0);
//	ssd1306_WriteString(humidity_si7021, Font_7x10, White);
//    //////////////////////////////////////////////////////////////////////////////////////

//	// Print GPS data
//	char str[50]={0};
//    memset(str, 0 , sizeof(str));
//
//	// Print data from GPS module  ( Lat )
//    ssd1306_SetCursor(0, 16);
//	ssd1306_WriteString(gps_latitude, Font_7x10, White);
//
//	// Print data from GPS module  ( Lon )
//	ssd1306_SetCursor(0, 26);
//    ssd1306_WriteString(gps_lontitude, Font_7x10, White);
//
//    // Print data from GPS module  ( Speed )
//    memset(str, 0 , sizeof(str));
//    sprintf(str,"%skm/h\0", gps_speed);
//    ssd1306_SetCursor(75, 16);
//    ssd1306_WriteString(str, Font_7x10, White);
//
//    // Print data from GPS module  ( Number of satalits)
//    memset(str, 0 , sizeof(str));
//    int satalits = atoi(gps_number_of_satellites);
//    if(satalits<=9)                 // clean zero sign
//    {
//    	 sprintf(str,"N:%s\0", "  ");
//    	 ssd1306_SetCursor(95, 26);
//    	 ssd1306_WriteString(str, Font_7x10, White);
//    }
//    sprintf(str,"N:%d\0", satalits);
//    ssd1306_SetCursor(95, 26);
//    ssd1306_WriteString(str, Font_7x10, White);
//
//    // Print data from GPS module (Time)
//    char hours[2]={0};
//    char minutes[2]={0};
//    char semicolum = ':';
//
//    // divide hours and minutes
//    uint8_t i=0;
//    for(i=0; i<=3; i++)
//    {
//    	if(i<=1)
//    	{
//    		hours[i] = gps_time[i];
//    	}
//    	else
//    	{
//    		minutes[i-2] = gps_time[i];
//    	}
//    }
//    // Add 3 hours
//    int int_hours = 0;
//    uint8_t dozens = 0;
//	uint8_t units = 0;
//
//	dozens = hours[0];
//	units = hours[1];
//	dozens = dozens - 48;
//	units = units - 48;
//
//	int_hours = units +(dozens *10);
//	int_hours = int_hours+3;
//
//	dozens = (int_hours/10) + 48;
//	units = (int_hours%10) + 48;
//	hours[0] = dozens;
//	hours[1] = units;
//
//	if(int_hours == 24)
//	{
//		hours[0] = '0';
//		hours[1] = '0';
//	}
//	if(int_hours == 25)
//	{
//		hours[0] = '0';
//		hours[1] = '1';
//	}
//	if(int_hours == 26)
//	{
//		hours[0] = '0';
//		hours[1] = '2';
//	}
//
//	if((int_hours >=0) && (int_hours<=9))
//	{
//		hours[0] = '0';
//		hours[1] = int_hours +48;
//	}
//
//    // connect  hours and minutes with colon
//    //i=5;
//    for(i=0; i<5; i++)
//    {
//    	if(i<=1)
//    	{
//    		str[i] = hours[i];
//    	}
//    	else
//    	{
//    		if(i==2)
//    		{
//    			str[i] = semicolum;
//    		}
//    		else
//    		{
//    			str[i]=minutes[i-3];
//    		}
//    	}
//    }
//
//    ssd1306_SetCursor(90, 00);
//    ssd1306_WriteString(str, Font_7x10, White);


//    }


    //////////////////////////////////////////////////////////////////////////////////////
    // GSM line
//    memset(str, 0, sizeof(str));
//
//    sprintf(str,"%s", "                        ");
//    ssd1306_SetCursor(00, 36);
//    ssd1306_WriteString(str, Font_7x10, White);
//    ssd1306_UpdateScreen();

//    memset(str, 0 , sizeof(str));
//    if(GSM_INIT == 1)
//    {
//     	sprintf(str,"%s", "GSM: READY STATE");
//     	ssd1306_SetCursor(00, 36);
//     	ssd1306_WriteString(str, Font_7x10, White);
//    }
//    if(GSM_INIT == 0)
//    {
//    	sprintf(str,"%s", "GSM: ERR STATE");
//     	ssd1306_SetCursor(00, 36);
//     	ssd1306_WriteString(str, Font_7x10, White);
//    }
    ////////////////////////////////
//
//    if(GSM_STATE == CALLING)
//    {
//       	sprintf(str,"%s", "Calling ...");
//        ssd1306_SetCursor(00, 46);
//        ssd1306_WriteString(str, Font_7x10, White);
//    }
//    if(GSM_STATE == BUSY)
//    {
//        sprintf(str,"%s", "BUSY");
//        ssd1306_SetCursor(00, 46);
//        ssd1306_WriteString(str, Font_7x10, White);
//    }
//    if(GSM_STATE == RING)
//    {
//        sprintf(str,"%s", "RING...");
//        ssd1306_SetCursor(00, 46);
//        ssd1306_WriteString(str, Font_7x10, White);
//    }
//    if(GSM_STATE == NOTHING)
//    {
//           sprintf(str,"%s", "                        ");
//           ssd1306_SetCursor(00, 46);
//           ssd1306_WriteString(str, Font_7x10, White);
//    }


    //GSM_STATE = CALLING;




      //////////////////////////////////////////////////////////////////////////////////////
//
//    // Test line 2
//    memset(str, 0 , sizeof(str));
//    sprintf(str,"%s", "TEST 0123456789");
//    ssd1306_SetCursor(00, 46);
//    ssd1306_WriteString(str, Font_7x10, White);


//	ssd1306_UpdateScreen();
//
//}


