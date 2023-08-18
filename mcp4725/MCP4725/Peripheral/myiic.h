#ifndef __MYIIC_H
#define __MYIIC_H
#include "gd32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//IO��������
// 
//#define SDA_IN()  {GPIOB->CRH&=0X0FFFFFFF;GPIOB->CRH|=(u32)8<<12;}    //PB11
//#define SDA_OUT() {GPIOB->CRH&=0X0FFFFFFF;GPIOB->CRH|=(u32)3<<12;}
 

//IO��������	 
#define IIC_SCL_O1    gpio_bit_write(GPIOB,GPIO_PIN_6,SET) //SCL
#define IIC_SCL_O0    gpio_bit_write(GPIOB,GPIO_PIN_6,RESET) //SCL

#define IIC_SDA_O1    gpio_bit_write(GPIOB,GPIO_PIN_7,SET) //SDA	 
#define IIC_SDA_O0    gpio_bit_write(GPIOB,GPIO_PIN_7,RESET) //SDA	 


#define READ_SDA  gpio_input_bit_get(GPIOB,GPIO_PIN_7)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void SDA_IN(void);
void SDA_OUT(void);

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  
#endif
















