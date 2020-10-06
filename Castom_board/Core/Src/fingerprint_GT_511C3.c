/*
 * fingerprint_GT_511C3.c
 *
 *  Created on: Oct 1, 2020
 *      Author: odemki
 */

#include "stdio.h"
#include "stm32f1xx_hal.h"

#include "fingerprint_GT_511C3.h"

extern UART_HandleTypeDef huart2;
SB_OEM_PKT Command_Packet;


int CalcChkSumOfCmdAckPkt( SB_OEM_PKT* pPkt )
{
	unsigned short wChkSum = 0;
	unsigned char * pBuf = (unsigned char*)pPkt;
	int i;

	for(i=0;i<(sizeof(SB_OEM_PKT)-SB_OEM_CHK_SUM_SIZE);i++)
		wChkSum += pBuf[i];

	return wChkSum;
}
//--------------------------------------------------------------------------------
short SendCommand(unsigned char * ucData,unsigned char length)
{
	// Origin ////////////////////////////////////////////
//	unsigned char i;
//	unsigned char tmp;
//	for(i = 0; i < length; i++)
//	{
//		SERIAL_PutChar(*(ucData + i));
//	}
	//////////////////////////////////////////////////////
	//HAL_UART_Transmit(&huart2, uint8_t *pData, uint16_t Size, uint32_t Timeout);

//	char str3[100]={0};
//		uint8_t size_mas=sizeof(str3);
//		uint8_t i=0;
//		sprintf(str3,"I2C_1 SEARCH DEVISES... \r\n");      										// convert   in  str
//		size=sizeof(str3);
//		HAL_UART_Transmit(&huart1 , (uint8_t *)str3, size, 0xFFFF);

	return OK;
}
//--------------------------------------------------------------------------------
int fingerprint_test(void)
{
//		int Flag = 0;
//		unsigned char Buffer[12];
//	//	unsigned char DataBuff[26];
//		int rtn;
//		int i,j;
//		unsigned long tmp = 0;
//		/*********Open Command***********/
//		Command_Packet.Head1 = COMMAND_START_CODE1;
//		Command_Packet.Head2 = COMMAND_START_CODE2;
//		Command_Packet.wDevId = DEVICE_ID;
//		Command_Packet.nParam = Flag;
//		Command_Packet.wCmd = OPEN;
//		tmp = CalcChkSumOfCmdAckPkt(&Command_Packet);
//		Command_Packet.wChkSum = tmp;
//		/*********Open Command***********/
//
//		int Status = 0;
//		Status = HAL_UART_Transmit(&huart2, &Command_Packet.Head1, SB_OEM_PKT_SIZE, 0xFFFF );
//		if(Status != HAL_OK)
//		{
//			while(1)
//			{
//				// error
//			}
//		}
//		rtn = SendCommand(&Command_Packet.Head1,SB_OEM_PKT_SIZE);
//		if(rtn == FAIL)
//		{
//			return FAIL;
//		}

//		rtn = ReceiveCommand(&Command_Packet.Head1,SB_OEM_PKT_SIZE);
//		if(rtn == FAIL)
//			return FAIL;
//
//		if(Command_Packet.wCmd != ACK)
//			return Command_Packet.nParam;
//
//		rtn = CheckChkSumOfRpsAckPkt(&Command_Packet);
//		if(rtn != OK)
//			return rtn;
//
//		if(Flag)
//		{
//			rtn = ReceiveCommand(&Command_Packet.Head1,SB_OEM_PKT_SIZE);
//			if(rtn == FAIL)
//				return FAIL;
//
//			rtn = CalcChkSumOfDataPkt(&Command_Packet.Head1,SB_OEM_PKT_SIZE);
//			if(rtn != OK)
//				return rtn;
//			else
//			{
//				memcpy(DataBuffer,Command_Packet,SB_OEM_PKT_SIZE);
//			}
//		}
//		return OK;
	}









//
