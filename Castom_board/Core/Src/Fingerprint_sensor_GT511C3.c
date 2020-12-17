/*
 * Fingerprint_sensor_GT511C3.c
 *
 *  Created on: Dec 14, 2020
 *      Author: odemki
 */



#include <stdint.h>
#include <stdlib.h>

#include "Fingerprint_sensor_GT511C3.h"


extern UART_HandleTypeDef huart2;

//unsigned char receive_data_from_fingerprint[50]={0};
uint8_t fingerprint_count_bytes = 0;
uint8_t data_from_fingerprint_module = 0;


//////////////////////////////////////
// MAIN FINGERPRINT FUNCTION
void finger_print_test_function(void)
{
	while(1)
	{
		//LED(1);
			HAL_Delay(500);
			int u=0;
			for(u = 0; u<10; u++)
			{
				HAL_Delay(100);
				touch_bakcklight(0);
				HAL_Delay(100);
				touch_bakcklight(1);
			}
			while (!touch_is_press_finger())
			{
					HAL_Delay(100);
			}
			int f = 9999;
	}

}
// ----------------------------------------------------------------------------
void touch_send(uint8_t* packet, int len)
{
	//HAL_UART_Transmit(&huart1,packet,len,1000);
	int rtn;
	rtn = HAL_UART_Transmit(&huart2, packet, (uint16_t)len, 10000);
	if(rtn != HAL_OK)
	{
		// Error connection
	}
}
// ----------------------------------------------------------------------------
int touch_rcv(uint8_t* packet, int len,int timeout)
{
	HAL_UART_Receive(&huart2,packet,len,timeout);
}
// ----------------------------------------------------------------------------
uint16_t get_checksum(uint8_t *package, int len)
{
    int i = 0;
    uint16_t checksum = 0;
    for (; i < len - 2; i++)
    {
        checksum += *(package + i);
    }
    return checksum;
}
// ----------------------------------------------------------------------------
void create_command_package(uint32_t param, uint16_t command,uint8_t* package)
{
    uint16_t checksum;
    package[0] = 0x55;
    package[1] = 0xAA;
    package[2] = 0x01;
    package[3] = 0x00;
    package[4] = param & 0xFF;
    package[5] = (param >> 8) & 0xFF;
    package[6] = (param >> 16) & 0xFF;
    package[7] = (param >> 24) & 0xFF;
    package[8] = command & 0xFF;
    package[9] = (command >> 8) & 0xFF;
    checksum = get_checksum(package, COMMAND_PACKET_LEN);
    package[10] = (checksum)&0xFF;
    package[11] = (checksum >> 8) & 0xFF;
}
// ----------------------------------------------------------------------------
int rcv_ack(uint8_t* packet, int len,int timeout)
{
	uint16_t calc_checksum;
	uint16_t recv_checksum;
	touch_rcv(packet,len,timeout);
	recv_checksum = packet[len - 2]  | packet[len - 1] << 8;
	calc_checksum = get_checksum(packet,len);
	if(recv_checksum != calc_checksum){
		return 9;
	}else if(packet[8] != ACK){
		return 9;
	}
	return HAL_OK;
}

// ----------------------------------------------------------------------------
/*
 * Turn off LED 1
 * Turn ON LED 1
 */
void touch_bakcklight(int value)
{
	create_command_package(value, CMOSLED_CMD, command_packet);
	touch_send(command_packet,COMMAND_PACKET_LEN);

	if(rcv_ack(response_packet,REPONSE_PACKET_LEN,1000) != HAL_OK){
			//Error Handling
			return;
		}
		if(value != 0){
			printf("Touch: Backlight ON");
		}else{
			printf("Touch: Backlight OFF");
		}
}
// ----------------------------------------------------------------------------
// LED must be turn on for finger detection detection
int touch_is_press_finger(){
	create_command_package(1, ISPRESSFINGER_CMD, command_packet);
	touch_send(command_packet,COMMAND_PACKET_LEN);
	if(rcv_ack(response_packet,REPONSE_PACKET_LEN,1000) != HAL_OK){
		//Error Handling
		return -1;
	}
	if(response_packet[4] == 0 && response_packet[5] == 0 && response_packet[6] == 0 && response_packet[7] == 0){
		printf("Touch: Finger Press");
		return 1;
	}
	printf("Touch: No Finger Press");
	return 0;
}

////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//devinfo gDeviceInfo;
//
//SB_OEM_PKT Command_Packet;				 // extern SB_OEM_PKT Command_Packet;
////unsigned short DEVICE_ID = 0;
//
//struct test_struct
//{
//	unsigned char 	Head1;
//	unsigned char 	Head2;
//	unsigned short	wDevId;
//	unsigned int	nParam;
//	unsigned short	wCmd;// or nAck
//	unsigned short 	wChkSum;
//} tx_structure;
//
//// ----------------------------------------------------------------------------
//void finger_print_test_function(void)
//{
//	//LED(1);
//	HAL_Delay(500);
//	open();
//	int u=0;
//	for(u = 0; u<10; u++)
//	{
//		HAL_Delay(200);
//		LED(1);
//		HAL_Delay(200);
//		LED(0);
//	}
//
//
////	while(1)
////	{
////		removeFinger();
////		HAL_Delay(100);
////	}
//
////	LED(1);
////	HAL_Delay(1000);
//	//captureFinger();
//	//HAL_Delay(1000);
////	LED(0);
//}
//// ----------------------------------------------------------------------------
//int CalcChkSumOfCmdAckPkt( SB_OEM_PKT* pPkt )
//{
//	unsigned short wChkSum = 0;
//	unsigned char * pBuf = (unsigned char*)pPkt;
//	int i;
//
//	for(i=0;i<(sizeof(SB_OEM_PKT)-SB_OEM_CHK_SUM_SIZE);i++)
//		wChkSum += pBuf[i];
//
//	return wChkSum;
//}
//// ----------------------------------------------------------------------------
//int LED(unsigned char State)
//{
//	int rtn;
//	unsigned char i;
//	unsigned long tmp = 0;
//
//	// Test structure
//	struct test_struct * p_tx_structure = & tx_structure;
//	p_tx_structure -> Head1 = COMMAND_START_CODE1;
//	p_tx_structure -> Head2 = COMMAND_START_CODE2;
//	p_tx_structure -> wDevId = DEVICE_ID;
//	p_tx_structure -> nParam= State;
//	p_tx_structure -> wCmd= CMOSLED;
//	tmp = CalcChkSumOfCmdAckPkt(p_tx_structure);
//	p_tx_structure -> wChkSum = tmp;
//
//	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, (uint16_t)SB_OEM_PKT_SIZE, 10000);
//	if(rtn != HAL_OK)
//	{
//		// Error connection
//	}
//
////	if(data_from_fingerprint_module == 1)   // Data in buffer
////	{
////		if(receive_data_from_fingerprint[3] == 48)
////		{
////			fingerprint_count_bytes = 0;
////			memset(receive_data_from_fingerprint, 0 , 50);
////		}
////
////
////		//memset(receive_data_from_fingerprint, 0 , sizeof(receive_data_from_fingerprint));
////	}
////	uint8_t one = '1';
////	HAL_UART_Transmit(&huart2, &one, 1, 10000);
//
//	//if()
//
////	if (data_from_fingerprint_module == 1)
////	{
////		int j= 999;
////		memset(receive_data_from_fingerprint, 0 , sizeof(receive_data_from_fingerprint));
////	}
//
//	// Waiting answer from fingerprint module   (from interrupt)
//
//
//
//	return HAL_OK;
//}
//// ---------------------------------------------------------------------------
//void open(void)
//{
//	int rtn;
//	unsigned char i;
//	unsigned long tmp = 0;
//
//
//	struct test_struct * p_tx_structure = & tx_structure;
//	p_tx_structure -> Head1 = COMMAND_START_CODE1;
//	p_tx_structure -> Head2 = COMMAND_START_CODE2;
//	p_tx_structure -> wDevId = DEVICE_ID;
//	p_tx_structure -> nParam= 0;
//	p_tx_structure -> wCmd= OPEN;
//	tmp = CalcChkSumOfCmdAckPkt(p_tx_structure);
//	p_tx_structure -> wChkSum = tmp;
//		//int temp = p_tx_structure -> Head1;
//	//	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, SB_OEM_PKT_SIZE, 10000);
//
//	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, (uint16_t)SB_OEM_PKT_SIZE, 10000);
//	if(rtn != HAL_OK)
//	{
//		// Error connection
//	}
//	///////////////////////////////
//	unsigned char DataBuffer[12];
//
//	if(data_from_fingerprint_module == 1)   // Data in buffer
//		{
////			if(receive_data_from_fingerprint[3] == 48)
////			{
////
////				fingerprint_count_bytes = 0;
////				memset(receive_data_from_fingerprint, 0 , 50);
////			}
//
//
//			//memset(receive_data_from_fingerprint, 0 , sizeof(receive_data_from_fingerprint));
//		}
//}
//// ---------------------------------------------------------------------------
//void removeFinger(void)
//{
//	int rtn;
//	unsigned char i;
//	unsigned long tmp = 0;
//
//	struct test_struct * p_tx_structure = & tx_structure;
//	p_tx_structure -> Head1 = COMMAND_START_CODE1;
//	p_tx_structure -> Head2 = COMMAND_START_CODE2;
//	p_tx_structure -> wDevId = DEVICE_ID;
//	p_tx_structure -> nParam= ZERO;
//	p_tx_structure -> wCmd= ISPRESSFINGER;
//	tmp = CalcChkSumOfCmdAckPkt(p_tx_structure);
//	p_tx_structure -> wChkSum = tmp;
//		//int temp = p_tx_structure -> Head1;
//		//	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, SB_OEM_PKT_SIZE, 10000);
//
//	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, (uint16_t)SB_OEM_PKT_SIZE, 10000);
//	if(rtn != HAL_OK)
//	{
//			// Error connection
//	}
//
//	if(data_from_fingerprint_module == 1)   // Data in buffer
//	{
////				if(receive_data_from_fingerprint[3] == 48)
////				{
////
////					fingerprint_count_bytes = 0;
////					memset(receive_data_from_fingerprint, 0 , 50);
////				}
//
//
//				//memset(receive_data_from_fingerprint, 0 , sizeof(receive_data_from_fingerprint));
//	}
//
//}
//
////OPEN
////
////int rtn;
////	unsigned char i;
////	unsigned long tmp = 0;
////
////	struct test_struct * p_tx_structure = & tx_structure;
////	p_tx_structure -> Head1 = COMMAND_START_CODE1;
////	p_tx_structure -> Head2 = COMMAND_START_CODE2;
////	p_tx_structure -> wDevId = DEVICE_ID;
////	p_tx_structure -> nParam= 0;
////	p_tx_structure -> wCmd= OPEN;
////	tmp = CalcChkSumOfCmdAckPkt(p_tx_structure);
////	p_tx_structure -> wChkSum = tmp;
////	//int temp = p_tx_structure -> Head1;
////	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, SB_OEM_PKT_SIZE, 10000);
////
////	HAL_Delay(1000);





