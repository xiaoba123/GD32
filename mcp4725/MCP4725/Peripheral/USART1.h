#ifndef _RS485_H
#define _RS485_H

/************************* ͷ�ļ� *************************/
#include "gd32f10x.h"

/************************* �궨�� *************************/
//�������ģʽ,����Ҫ����ʱ�ȴ� 485 ����������
#define   RS485_RX_EN() 	 RS485_delay(100);gpio_bit_reset(GPIOA,GPIO_PIN_1);RS485_delay(100);

//���뷢��ģʽ,����Ҫ����ʱ�ȴ� 485 ����������
#define   RS485_TX_EN() 	 RS485_delay(100);gpio_bit_set(GPIOA,GPIO_PIN_1);RS485_delay(100);
/************************ �������� ************************/
 
/************************ �������� ************************/
void RS485_Send_Byte(uint8_t Modbus_byte);
void RS485_SendData(uint8_t *buf,uint8_t len);
void RS485_Config(void);    // Ƭѡ���ų�ʼ��
void RS485_Init(uint32_t baud);      // RS485 ��ʼ��
#endif
