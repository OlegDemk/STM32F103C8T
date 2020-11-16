/*
 * GSM_IOT_GA6.h
 *
 *  Created on: Oct 8, 2020
 *      Author: odemki
 */

#ifndef INC_GSM_IOT_GA6_H_
#define INC_GSM_IOT_GA6_H_

void init_GSM_uart_comunication(void);
int init_gsm_module(void);

int call_on_number(void);

void parsing_ansver_from_GSM(void);
void if_RING_from_GSM(void);
void if_RING_OUT_from_GSM(void);
void wait_ansver_after_make_call_in_blok_mode(void);
int end_of_call(void);

//enum GSM
//{
//	CALLING = 1,
//	NO_ANSWER,
//	BUSY,
//	NO_CARRIER,
//	RING,
//	NOTHING,
//}GSM_STATE;


uint8_t GSM_INIT;

#endif /* INC_GSM_IOT_GA6_H_ */
