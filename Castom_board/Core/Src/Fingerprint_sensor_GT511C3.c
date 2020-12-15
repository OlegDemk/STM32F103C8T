/*
 * Fingerprint_sensor_GT511C3.c
 *
 *  Created on: Dec 14, 2020
 *      Author: odemki
 */

#include "Fingerprint_sensor_GT511C3.h"

extern UART_HandleTypeDef huart2;

devinfo gDeviceInfo;

SB_OEM_PKT Command_Packet;				 // extern SB_OEM_PKT Command_Packet;
//unsigned short DEVICE_ID = 0;

// ----------------------------------------------------------------------------
void finger_print_test_function(void)
{
	LED(1);
	HAL_Delay(500);
	LED(0);
	HAL_Delay(500);
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
//	SB_OEM_PKT* Command_Packet;
//	unsigned char Buffer[12];
//	unsigned char DataBuff[26];
	int rtn;
	unsigned char i;
	unsigned long tmp = 0;

	struct test_struct
	{
		unsigned char 	Head1;
		unsigned char 	Head2;
		unsigned short	wDevId;
		unsigned int	nParam;
		unsigned short	wCmd;// or nAck
		unsigned short 	wChkSum;
	} tx_structure;

	//tx_structure.Head1 = 1;

//	Command_Packet = Buffer;
	/*********Change Baudrate Command***********/
//	tx_structure.Head1 = COMMAND_START_CODE1;
//	tx_structure.Head2 = COMMAND_START_CODE2;
//	tx_structure.wDevId = DEVICE_ID;
//	tx_structure.nParam = State;
//	tx_structure.wCmd = CMOSLED;
//	tmp = CalcChkSumOfCmdAckPkt(&Command_Packet);
//	tx_structure.wChkSum = tmp;
	/*********Change Baudrate Command***********/

	//DEVICE_ID++;

	// OPEN
//	Command_Packet.Head1 = COMMAND_START_CODE1;
//		Command_Packet.Head2 = COMMAND_START_CODE2;
//		Command_Packet.wDevId = DEVICE_ID;
//		Command_Packet.nParam = Flag;
//		Command_Packet.wCmd = OPEN;
//		tmp = CalcChkSumOfCmdAckPkt(&Command_Packet);
//		Command_Packet.wChkSum = tmp;

	//unsigned short DEVICE_ID = 1;
	//DEVICE_ID =0;

	struct test_struct * p_tx_structure = & tx_structure;
	p_tx_structure -> Head1 = COMMAND_START_CODE1;
	p_tx_structure -> Head2 = COMMAND_START_CODE2;
	p_tx_structure -> wDevId = DEVICE_ID;
	p_tx_structure -> nParam= 0;
	p_tx_structure -> wCmd= OPEN;
	tmp = CalcChkSumOfCmdAckPkt(p_tx_structure);
	p_tx_structure -> wChkSum = tmp;
	//int temp = p_tx_structure -> Head1;
	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, SB_OEM_PKT_SIZE, 10000);

	HAL_Delay(1000);

	tmp = 0;
	// Test structure
	//struct test_struct * p_tx_structure = & tx_structure;
	p_tx_structure -> Head1 = COMMAND_START_CODE1;
	p_tx_structure -> Head2 = COMMAND_START_CODE2;
	p_tx_structure -> wDevId = DEVICE_ID;
	p_tx_structure -> nParam= State;
	p_tx_structure -> wCmd= CMOSLED;
	tmp = CalcChkSumOfCmdAckPkt(p_tx_structure);
	p_tx_structure -> wChkSum = tmp;
	//int temp = p_tx_structure -> Head1;


	rtn = HAL_UART_Transmit(&huart2, p_tx_structure, SB_OEM_PKT_SIZE, 10000);
//
//	HAL_Delay(500);



//	if(DEVICE_ID == 1000)
//	{
//		int k =999;
//
//	}

	//tx_structure.Head1 = 0x01;
	//rtn = HAL_UART_Transmit(&huart2, &tx_structure.Head1, SB_OEM_PKT_SIZE, 1000);

	//rtn = SendCommand(&Command_Packet.Head1,SB_OEM_PKT_SIZE);

// Test structure
	//	struct test_struct * p_tx_structure = & tx_structure;
	//	p_tx_structure -> Head1 = 1;
	//	int temp = p_tx_structure -> Head1;

	// Test send Data   work
//	uint8_t test_transmit = 2;
//	uint8_t *p_test_transmit = &test_transmit;
//	rtn = HAL_UART_Transmit(&huart2, p_test_transmit, 1, 1000);

	// Work too
//	uint8_t test_transmit[3] = {1,2,3};
//	rtn = HAL_UART_Transmit(&huart2, test_transmit, 3, 1000);





//	rtn = HAL_UART_Transmit(&huart2, Command_Packet.Head1, SB_OEM_PKT_SIZE, 1000);
//	if(rtn == HAL_ERROR)
//	{
//		return HAL_ERROR;
//	}
//	rtn = ReceiveCommand(&Command_Packet.Head1,SB_OEM_PKT_SIZE);
//	if(rtn == FAIL)
//		return FAIL;
//
//	if(Command_Packet.wCmd != ACK)
//		return Command_Packet.nParam;
//
//	rtn = CheckChkSumOfRpsAckPkt(&Command_Packet);
//	if(rtn != OK)
//		return rtn;

	return HAL_OK;
}
