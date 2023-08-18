
/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：DAC.h
 * 作者: 
 * 平台: 
 * 微信: 
 * Q Q: 
 * 公众号：
************************************************************/

#ifndef __ANIIC_H
#define __ANIIC_H


/************************* 头文件 *************************/


#include "gd32f10x.h"
#include "gd32f10x_gpio.h"
/************************* 宏定义 *************************/



/************************ 变量定义 ************************/


/************************ 函数定义 ************************/




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


