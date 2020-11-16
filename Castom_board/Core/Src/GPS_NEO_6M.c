/*
 * GPS_NEO_6M.c
 *
 *  Created on: Sep 10, 2020
 *      Author: odemki
 */

#include "stdio.h"
#include "stm32f1xx_hal.h"
#include "string.h"               // For work with "strtok" function
#include "stdlib.h"               // For work with "atoi" function
#include "main.h"

extern UART_HandleTypeDef huart1;
extern uint8_t flag;

struct GPS
{
	char lat[11];
	char lat_letter[2];
	char lon[12];
	char lon_letter[2];
	char time[6];

	char speed[4];

	char number_of_satellites_GPGGA[2];

} GPS_data;


//---------------------------------------------------------------------
// Function print all non parsed data from GSM module
void simple_read_stream_data_from_GPS (uint8_t *GPS_buff, int size_buff)    // <<<<<<<<< NAME FUNCTION !!!!!
{
	if(flag == 1)
	{
		HAL_UART_Transmit(&huart1, GPS_buff, 512, 200);
		memset(GPS_buff, 0, 512);
		flag = 0;
	}
}
//---------------------------------------------------------------------
// Checksum
int nmea0183_checksum(char *msg)
{
	int checksum = 0;
	int j = 0;

	for(j = 1; j < strlen(msg) - 4; j++)
	{
		checksum = checksum^(unsigned)msg[j];
	}
	return checksum;
}

//---------------------------------------------------------------------
void parsing_GPGLL_line(char *str_GPGLL)
{
	char nmeaSnt[49];										// Main buffer for GPGLL line
	int size = sizeof(nmeaSnt);
	memset(nmeaSnt, 0, size);

	//Copy to  "*" з str_GPGLL в nmeaSnt
	int i=0;
	for(i=0; (str_GPGLL[i] != '*') && (i < 50)  ; i++)
	{
		nmeaSnt[i]=str_GPGLL[i];    						// copy bytes from str_GPGLL in nmeaSnt
	}
	// Check check sum
	char smNmbr[3]={0};     								// array for checksum
	char *rawSum;

	// Find "*"  (after '*' are checksum two numbers)
	rawSum = strstr(str_GPGLL, "*");                       	// Find "*" in nmeaSnt. (Find start checksum number)
	memcpy(smNmbr, &rawSum[1], 2);							// Copy checksum
	smNmbr[2]='\0';											// Add and of line '\0' sing

	uint8_t intSum = nmea0183_checksum(nmeaSnt);			// Checksum
	char hex[3];
	sprintf(hex, "%x", intSum);

	if(strstr(smNmbr, hex) != NULL)                         // Check if checksum line equal checksum after '*'
	{
		// Parsing string
		int i = 0;
		int count = 0;                                      // Count of elements

		while(nmeaSnt[i] != '\0')							// not and of line
		{
			while(nmeaSnt[i] == ',')						// ',' pointed on start element
			{
				int g=0;
				char str[12]={0};
				i++;										// Count of chars in nmeaSnt[i] array

				switch (count)
				{
					case 0:
						for(g=0; g<10; g++)
						{
							GPS_data.lat[g] = nmeaSnt[i+g];
						}
						GPS_data.lat[10] = '\0';

						// Save in global variable
						memset(str, 0 , sizeof(str));
						sprintf(str,"%s", GPS_data.lat);
						strcpy(gps_lat, str);

						// Convert lat in coordinate for google mups
						// 1. Separate degrees from minutes
						float integer_part_lat = 0;
						float fractional_part_lat = 0;
						char integer_part_char_lat[3]={0};
						char fractional_part_char_lat[9]={0};
						uint8_t k=0;

						for(k=0; k<=11; k++)
						{
							if(k<=1)
							{
								integer_part_char_lat[k] = GPS_data.lat[k];        	  // Degrees
							}
							else
							{
								fractional_part_char_lat[k-2] = GPS_data.lat[k];      // Minutes
							}
						}

						// 2. Convert string in numbers
						integer_part_lat = atoi(integer_part_char_lat);          // save int part
						fractional_part_lat = atof(fractional_part_char_lat);	 // float part
 						// 3. Convert in degrees
						fractional_part_lat = (fractional_part_lat/60);
						integer_part_lat = integer_part_lat + fractional_part_lat;
						// 4. Convert in string and save it in global variable
                        gcvt(integer_part_lat, 11, gps_latitude );
                        gps_latitude[9] = '\0';

						i = i+g;
						break;

					case 1:
						for (g =0; g<1; g++)
						{
							GPS_data.lat_letter[g] = nmeaSnt[i+g];
						}
						GPS_data.lat_letter[2] = '\0';
						i = i+g;
						break;

					case 2:
						for(g=0; g<11; g++)
						{
							GPS_data.lon[g] = nmeaSnt[i+g];
						}
						GPS_data.lon[11] = '\0';

						// Save in global variable
						memset(str, 0 , sizeof(str));
						sprintf(str,"%s", GPS_data.lon);
						strcpy(gps_lon, str);

						// Convert lon in coordinate for google mups
						// // 1. Separate degrees from minutes
						float integer_part_lon = 0;
						float fractional_part_lon = 0;
						char integer_part_char_lon[3]={0};
						char fractional_part_char_lon[10]={0};

						for(k=0; k<=11; k++)
						{
							if(k<=2)
							{
								integer_part_char_lon[k] = GPS_data.lon[k];
							}
							else
							{
								fractional_part_char_lon[k-3] = GPS_data.lon[k];
							}
						}

						// 2. Convert string in numbers
						integer_part_lon = atoi(integer_part_char_lon);          // save int part
						fractional_part_lon = atof(fractional_part_char_lon);	 // float part
						// 3. Convert in degrees
						fractional_part_lon = (fractional_part_lon/60);
						integer_part_lon = integer_part_lon + fractional_part_lon;
						// 4. Convert in string and save it in global variable
					    gcvt(integer_part_lon, 11, gps_lontitude );
					    gps_lontitude[10] = '\0';

						i = i+g;
						break;

					case 3:
						for(g=0; g<1; g++)
						{
							GPS_data.lon_letter[g] = nmeaSnt[i+g];
						}
						GPS_data.lon_letter[2] = '\0';
						i = i+g;
						break;

					case 4:
						for(g=0; g<6; g++)
						{
							GPS_data.time[g] = nmeaSnt[i+g];
						}
						GPS_data.time[6] = '\0';
						i = i+g;

						// Save time in global variable
						memset(str, 0 , sizeof(str));
						sprintf(str,"%s", GPS_data.time);
						strcpy(gps_time, str);
						break;
				}
				count++;
			}
			i++;
		}
	}
}
//---------------------------------------------------------------------
void parsing_GPGGA_line(char *str_GPGGA)
{
	    char nmeaSnt[70];
		int size = sizeof(nmeaSnt);
		memset(nmeaSnt, 0, size);
		char number_of_satalits_str[4] = {0};

		//Copy to  "*" з str_GPGGA в nmeaSnt
		int i=0;

		for(i=0; (str_GPGGA[i] != '*') && (i < 70)  ; i++)
		{
			nmeaSnt[i]=str_GPGGA[i];
		}

		// Check check sum
		char smNmbr[3]={0};     								// array for checksum
		char *rawSum;
		char not_used[12] = {0};

		// Find "*"
		rawSum = strstr(str_GPGGA, "*");                       	// Find "*" in nmeaSnt. (Find start checksum number)
		memcpy(smNmbr, &rawSum[1], 2);							// Copy checksum
		smNmbr[2]='\0';											// Add and of line '\0' sing

        // PROBLEM: Checksum !=
		char hex[3];
		memset(hex, 0 , sizeof(hex));
		uint8_t intSum = nmea0183_checksum(nmeaSnt);			// Checksum
		sprintf(hex, "%x", intSum);

//		if(strstr(smNmbr, hex) != NULL)
//		{
			// Parsing string

			i = 0;
			int count = 0;

			while(nmeaSnt[i] != '\0')
			{
				while(nmeaSnt[i] == ',')
				{
					int g=0;
					i++;
					switch (count)
					{
						case 0:                     // Time
							for(g=0; g<8; g++)
							{
								not_used[g] = nmeaSnt[i+g];
							}
							not_used[9] = '\0';
							i = i+g;

							break;

						case 1:   					// Lat
							for (g =0; g<10; g++)
							{
								not_used[g] = nmeaSnt[i+g];
							}
							not_used[10] = '\0';
							i = i+g;
							break;

						case 2:						// N
							for(g=0; g<1; g++)
							{
								not_used[g] = nmeaSnt[i+g];
							}
							not_used[1] = '\0';
							i = i+g;
							break;

						case 3:                     // Lat
							for(g=0; g<11; g++)
							{
								not_used[g] = nmeaSnt[i+g];
							}
							not_used[11] = '\0';
							i = i+g;
							break;

						case 4:                      // E
							for(g=0; g<1; g++)
							{
								not_used[g] = nmeaSnt[i+g];
							}
							not_used[1] = '\0';
							i = i+g;
							break;

						case 5:                      // Fix Quality
							for(g=0; g<1; g++)
							{
								not_used[g] = nmeaSnt[i+g];
							}
							not_used[1] = '\0';
							i = i+g;
							break;

						case 6:                      // Number of Satellites
							memset(number_of_satalits_str, 0 , sizeof(number_of_satalits_str));    // clean buffer

							for(g=0; g<2; g++)
							{
								number_of_satalits_str[g] = nmeaSnt[i+g];
							}
							number_of_satalits_str[2] = '\0';
							// Convert two digits string on number
							uint8_t dozens_number_of_satalits = (uint8_t)number_of_satalits_str[0] - 48;
							dozens_number_of_satalits = dozens_number_of_satalits *10;
							uint8_t units_number_of_satalits = (uint8_t)number_of_satalits_str[1]  - 48;
							uint8_t digit_number_of_satalites = dozens_number_of_satalits + units_number_of_satalits;

							// Chesk if data id number
							if((digit_number_of_satalites >= 3 || digit_number_of_satalites <= 14))   // Check if data correct
							{
								// Convert uint8_t in char
								char buff_char_number_of_satalites[3]={0};
								memset(buff_char_number_of_satalites, 0 ,sizeof(buff_char_number_of_satalites));
								buff_char_number_of_satalites[0] = (char)((dozens_number_of_satalits/10) + 48);
								buff_char_number_of_satalites[1] = (char)units_number_of_satalits + 48;
								buff_char_number_of_satalites[2] = '\0';

								// write in global variable
								strcpy(gps_number_of_satellites, buff_char_number_of_satalites);
							}
							else
							{
								char error_message[] = "Er";
								strcpy(gps_number_of_satellites, error_message);
							}

							break;
					}
					count++;
				}
				i++;
			}
		//}
}


//---------------------------------------------------------------------
void parsing_GPVTG_line(char *str_GPVTG)
{
	char nmeaSnt[49];
	int size = sizeof(nmeaSnt);
	memset(nmeaSnt, 0, size);

	char str[6]={0};

	//Copy to  "*" з str_GPVTG в nmeaSnt
	int i=0;

	for(i=0; (str_GPVTG[i] != '*') && (i < 50)  ; i++)
	{
		nmeaSnt[i]=str_GPVTG[i];    // Read 49 bytes
	}

	// Check check sum
	char smNmbr[3]={0};     								// array for checksum
	char *rawSum;

	// Find "*"
	rawSum = strstr(str_GPVTG, "*");                       	// Find "*" in nmeaSnt. (Find start checksum number)
	memcpy(smNmbr, &rawSum[1], 2);							// Copy checksum
	smNmbr[2]='\0';											// Add and of line '\0' sing

//	// PROBLEM: Checksum !=

	i = 0;
	int count = 0;

	uint8_t speed_data_readed = 1;
	while(speed_data_readed == 1)
	{
		if(nmeaSnt[i] == ',')
		{
			count++;
		}
		if(count == 7)				// Find seven part (speed)
		{
			i++;
			uint8_t k = 0;
			char speed_test_variable[4]={0};    // for debug
			uint8_t error = 0;
			do
			{
				//GPS_data.speed[k] = nmeaSnt[i+k];
				if(((nmeaSnt[i+k]-48) >= 0) && ((nmeaSnt[i+k]-48)  <= 9))   // Ok
				{
					speed_test_variable[k] = nmeaSnt[i+k];
				}
				else	// if error value. delete value.
				{
					// Error
					error = 1;
					memset(speed_test_variable, 0 ,sizeof(speed_test_variable));
				}
				k++;
			}while ((k < 3) && (nmeaSnt[i+k] != '.') && (error != 1));    // Problem where !!!!

			///////////// Test
//			if(speed_test_variable[0] == '1')
//			{
//				int j=0;
//				j = 100+100;
//			}
			///////// end test

			// Save in global variable
			memset(str, 0 , sizeof(str));
			//sprintf(str,"%s", GPS_data.speed);
			sprintf(str,"%s", speed_test_variable);
			strcpy(gps_speed, str);

			speed_data_readed = 0;
		}
		i++;
	}
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
// Parsing only three lines from NMIA
// 1. From GPGLL - coordinates
// 2. From GPGGA - number of satellites
// 3. From GPVTG - speed
void parsing_GPS(uint8_t *GPS_buff, int size_buff)
{
	char buffStr[512];										 // Main buffer in this function

	if(flag == 1)    										 // If data from GPS module in buffer
	{
		memset(buffStr, 0, 512);        					 // Clean buffStr
		sprintf(buffStr, "%s", GPS_buff);					 // Save in buffStr string from biff

		char *str_GPGLL;
		char *str_GPGGA;
		char *str_GPVTG;
		char *str_answer;

		// Find $GPGLL in buffStr
		str_GPGLL = strstr(buffStr, "$GPGLL");    // $GPGLL,4948.72578,N,02359.72468,E,151729.00,A,A*6C\r
		if(str_GPGLL != NULL)
		{
			parsing_GPGLL_line(str_GPGLL);
		}

		// Find $GPGGA in buffStr
		str_GPGGA = strstr(buffStr, "$GPGGA");    //$GPGGA,154423.00,4948.74034,N,02359.70944,E,1,05A,154422.00,4948.74052,N,02359.70973,E,1,05,3.71,351.1,M,34.8,M,,*53\r\n
		if(str_GPGGA != NULL)
		{
			parsing_GPGGA_line(str_GPGGA);
		}

		// Find $GPVTG in buffStr
		str_GPVTG = strstr(buffStr, "$GPVTG");    // $GPVTG,,T,,M,0.036,N,0.066,K,A*26\r\n
		if(str_GPVTG != NULL)
		{
			parsing_GPVTG_line(str_GPVTG);
		}

		flag = 0;			// Flag = 0 means: Data was readed
	}
}
//---------------------------------------------------------------------

















//
