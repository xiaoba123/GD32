#ifndef _RS485_H
#define _RS485_H

/************************* 头文件 *************************/
#include "gd32f10x.h"

/************************* 宏定义 *************************/
//进入接收模式,必须要有延时等待 485 处理完数据
#define   RS485_RX_EN() 	 RS485_delay(100);gpio_bit_reset(GPIOA,GPIO_PIN_1);RS485_delay(100);

//进入发送模式,必须要有延时等待 485 处理完数据
#define   RS485_TX_EN() 	 RS485_delay(100);gpio_bit_set(GPIOA,GPIO_PIN_1);RS485_delay(100);
/************************ 变量定义 ************************/
 
/************************ 函数定义 ************************/
void RS485_Send_Byte(uint8_t Modbus_byte);
void RS485_SendData(uint8_t *buf,uint8_t len);
void RS485_Config(void);    // 片选引脚初始化
void RS485_Init(uint32_t baud);      // RS485 初始化
#endif
