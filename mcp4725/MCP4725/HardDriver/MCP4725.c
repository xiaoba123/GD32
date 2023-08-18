#include "MCP4725.h" 
#include "USART0.h"	
#include "systick.h"
//////////////////////////////////////////////////////////////////////////////////	 
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
uint16_t MCP4725_DAC_Value=5000;
//��ʼ��IIC�ӿ�
void MCP4725_Init(void)
{
	IIC_Init();
//	MCP4725_WriteData_Voltage(MCP4725_DAC_Value);//��ʼ3V
}

//ʹ�ÿ���ģʽд����дDAC�Ĵ���
void MCP4725_WriteData_Voltage(uint16_t Vout)   //��ѹ��λmV
	
{
  uint8_t temp;
	uint16_t Dn;
	if(Vout>=5000) Vout=4999;
	Dn = ( 4096 * Vout) / VREF_5V;
	temp = (0x0F00 & Dn) >> 8;  //12λ����
	
	IIC_Start();
	IIC_Send_Byte(0XC2);      //����Ѱַ����������1100�� ��ַλA2��A1��A0Ϊ 0 �� 0 �� 1��-> 1100 0010
  IIC_Wait_Ack();	 
  IIC_Send_Byte(temp); 	
  IIC_Wait_Ack();	 
  IIC_Send_Byte(Dn);        //����8λ�͵�DAC�Ĵ���
	IIC_Wait_Ack();	
  IIC_Stop();//����һ��ֹͣ����  	
	delay_1ms(10);	
}
 
void MCP4725_WriteData_Digital(uint16_t data)   //12λ������
{
  uint8_t data_H=0,data_L=0;
	data_H = ( 0x0F00 & data) >> 8;
	data_L = 0X00FF & data ;
	IIC_Start();
	IIC_Send_Byte(0XC0);      //����Ѱַ����������1100�� ��ַλA2��A1��A0Ϊ 0 �� 0 �� 0��-> 1100 0000
  IIC_Wait_Ack();	 
  IIC_Send_Byte(data_H); 	
  IIC_Wait_Ack();	 
  IIC_Send_Byte(data_L);
	IIC_Wait_Ack();	
  IIC_Stop();//����һ��ֹͣ����  	
	delay_1ms(10);	
}












