/*
 * Fingerprint_sensor_GT511C3.c
 *
 *  Created on: Dec 14, 2020
 *      Author: odemki
 */

#include "Fingerprint_sensor_GT511C3.h"

extern UART_HandleTypeDef huart2;

unsigned char receive_data_from_fingerprint[50]={0};
uint8_t fingerprint_count_bytes = 0;
uint8_t data_from_fingerprint_module = 0;

devinfo gDeviceInfo;

SB_OEM_PKT Command_Packet;				 // extern SB_OEM_PKT Command_Packet;
//unsigned short DEVICE_ID = 0;

struct test_struct
{
	unsigned char 	Head1;
	unsigned char 	Head2;
	unsigned short	wDevId;
	unsigned int	nParam;
	unsigned short	wCmd;// or nAck
	unsigned short 	wChkSum;
} tx_structure;

// ----------------------------------------------------------------------------
void finger_print_test_function(void)
{
	//LED(1);
	HAL_Delay(500);
	open();
	int u=0;
	for(u = 0; u<10; u++)
	{
		HAL_Delay(200);
		LED(1);
		HAL_Delay(200);
		LED(0);
	}


//	while(1)
//	{
//		removeFinger();
//		HAL_Delay(100);
//	}

//	LED(1);
//	HAL_Delay(1000);
	//captureFinger();
	//HAL_Delay(1000);
//	LED(0);
}
// ----------------------------------------------------------------------------
int CalcChkSumOfCmdAckPkt( SB_OEM_PKT* pPkt )
{
	unsigned short wChkSum = 0;
	unsigned char * pBuf = (unsigned char*)pPkt;
	int i;

	for(i=0;i<(sizeof(SB_OEM_PKT)-SB_OEM_CHK_SUM_SIZE);i++)
		wChkSum += pBuf[i];

	return wChkSum;
}
// ----------------------------------------------------------------------------
int LED(unsigned char State)
{
	int rtn;
	unsigned char i;
	unsigned long tmp = 0;

	// Test structure
	struct test_struct * p_tx_structure = & tx_structure;
	p_tx_structure -> Head1 = COMMAND_START_CODE1;
	p_tx_structure -> Head2 = COMMAND_START_CODE2;
	p_tx_structure -> wDevId = DEVICE_ID;
	p_tx_structure -> nParam= State;
	p_tx_structure -> wCmd= CMOSLED;
	tmp = CalcChkSumOfCmdAckPkt(p_tx_structure);
	p_tx_structure -> wChkSum = tmp;

	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, (uint16_t)SB_OEM_PKT_SIZE, 10000);
	if(rtn != HAL_OK)
	{
		// Error connection
	}

//	if(data_from_fingerprint_module == 1)   // Data in buffer
//	{
//		if(receive_data_from_fingerprint[3] == 48)
//		{
//			fingerprint_count_bytes = 0;
//			memset(receive_data_from_fingerprint, 0 , 50);
//		}
//
//
//		//memset(receive_data_from_fingerprint, 0 , sizeof(receive_data_from_fingerprint));
//	}
//	uint8_t one = '1';
//	HAL_UART_Transmit(&huart2, &one, 1, 10000);

	//if()

//	if (data_from_fingerprint_module == 1)
//	{
//		int j= 999;
//		memset(receive_data_from_fingerprint, 0 , sizeof(receive_data_from_fingerprint));
//	}

	// Waiting answer from fingerprint module   (from interrupt)



	return HAL_OK;
}
// ---------------------------------------------------------------------------
void open(void)
{
	int rtn;
	unsigned char i;
	unsigned long tmp = 0;


	struct test_struct * p_tx_structure = & tx_structure;
	p_tx_structure -> Head1 = COMMAND_START_CODE1;
	p_tx_structure -> Head2 = COMMAND_START_CODE2;
	p_tx_structure -> wDevId = DEVICE_ID;
	p_tx_structure -> nParam= 0;
	p_tx_structure -> wCmd= OPEN;
	tmp = CalcChkSumOfCmdAckPkt(p_tx_structure);
	p_tx_structure -> wChkSum = tmp;
		//int temp = p_tx_structure -> Head1;
	//	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, SB_OEM_PKT_SIZE, 10000);

	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, (uint16_t)SB_OEM_PKT_SIZE, 10000);
	if(rtn != HAL_OK)
	{
		// Error connection
	}
	///////////////////////////////
	unsigned char DataBuffer[12];

	if(data_from_fingerprint_module == 1)   // Data in buffer
		{
//			if(receive_data_from_fingerprint[3] == 48)
//			{
//
//				fingerprint_count_bytes = 0;
//				memset(receive_data_from_fingerprint, 0 , 50);
//			}


			//memset(receive_data_from_fingerprint, 0 , sizeof(receive_data_from_fingerprint));
		}
}
// ---------------------------------------------------------------------------
void removeFinger(void)
{
	int rtn;
	unsigned char i;
	unsigned long tmp = 0;

	struct test_struct * p_tx_structure = & tx_structure;
	p_tx_structure -> Head1 = COMMAND_START_CODE1;
	p_tx_structure -> Head2 = COMMAND_START_CODE2;
	p_tx_structure -> wDevId = DEVICE_ID;
	p_tx_structure -> nParam= ZERO;
	p_tx_structure -> wCmd= ISPRESSFINGER;
	tmp = CalcChkSumOfCmdAckPkt(p_tx_structure);
	p_tx_structure -> wChkSum = tmp;
		//int temp = p_tx_structure -> Head1;
		//	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, SB_OEM_PKT_SIZE, 10000);

	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, (uint16_t)SB_OEM_PKT_SIZE, 10000);
	if(rtn != HAL_OK)
	{
			// Error connection
	}

	if(data_from_fingerprint_module == 1)   // Data in buffer
	{
//				if(receive_data_from_fingerprint[3] == 48)
//				{
//
//					fingerprint_count_bytes = 0;
//					memset(receive_data_from_fingerprint, 0 , 50);
//				}


				//memset(receive_data_from_fingerprint, 0 , sizeof(receive_data_from_fingerprint));
	}

}

//OPEN
//
//int rtn;
//	unsigned char i;
//	unsigned long tmp = 0;
//
//	struct test_struct * p_tx_structure = & tx_structure;
//	p_tx_structure -> Head1 = COMMAND_START_CODE1;
//	p_tx_structure -> Head2 = COMMAND_START_CODE2;
//	p_tx_structure -> wDevId = DEVICE_ID;
//	p_tx_structure -> nParam= 0;
//	p_tx_structure -> wCmd= OPEN;
//	tmp = CalcChkSumOfCmdAckPkt(p_tx_structure);
//	p_tx_structure -> wChkSum = tmp;
//	//int temp = p_tx_structure -> Head1;
//	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, SB_OEM_PKT_SIZE, 10000);
//
//	HAL_Delay(1000);





