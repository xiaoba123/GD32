
/************************************************************
 * ��Ȩ���Ͻ���ҵ��;������ѧϰʹ�á� 
 * �ļ���DAC.h
 * ����: 
 * ƽ̨: 
 * ΢��: 
 * Q Q: 
 * ���ںţ�
************************************************************/

#ifndef __ANIIC_H
#define __ANIIC_H


/************************* ͷ�ļ� *************************/


#include "gd32f10x.h"
#include "gd32f10x_gpio.h"
/************************* �궨�� *************************/



/************************ �������� ************************/


/************************ �������� ************************/




void ANIIC_Init(void);

void ANIIC_Start(void);
void ANIIC_Stop(void);

void ANIIC_SendAck(bit_status bitvalue);
uint8_t ANIIC_ReceiveAck(void);

void ANIIC_SendByte(uint8_t Byte);
uint8_t ANIIC_ReceiveByte(void);

void ANIIC_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t ANIIC_ReadReg(uint8_t RegAddress);
				    
#endif


/****************************End*****************************/


