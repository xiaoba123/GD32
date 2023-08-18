#include "myiic.h"
#include "systick.h"
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
 
//��ʼ��IIC
void IIC_Init(void)
{					
        rcu_periph_clock_enable(RCU_GPIOB); 	
				gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);  // �������ų�ʼ�� TX

				gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);  // �������ų�ʼ��  RX
}

//SDA����Ϊ����
void SDA_IN (void)
{
	      rcu_periph_clock_enable(RCU_GPIOB); 
				gpio_init(GPIOB,GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);  // �������ų�ʼ��  RX
}

//SDA����Ϊ���
void SDA_OUT(void)
{ 
	      rcu_periph_clock_enable(RCU_GPIOB); 
        gpio_init(GPIOB,GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda������Ϊ���
	IIC_SDA_O1;	  	  
	IIC_SCL_O1;
	delay_1us(16);
 	IIC_SDA_O0;     //START:when CLK is high,DATA change form high to low 
	delay_1us(16);
	IIC_SCL_O0;     //ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL_O0;
	IIC_SDA_O0;//STOP:when CLK is high DATA change form low to high
 	delay_1us(16);
	IIC_SCL_O1; 
	IIC_SDA_O1;//����I2C���߽����ź�
	delay_1us(16);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;

	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA_O1;delay_1us(4);	   
	IIC_SCL_O1;delay_1us(4);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_O0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL_O0;
	SDA_OUT();
	IIC_SDA_O0;
	delay_1us(8);
	IIC_SCL_O1;
	delay_1us(8);
	IIC_SCL_O0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL_O0;
	SDA_OUT();
	IIC_SDA_O1;
	delay_1us(8);
	IIC_SCL_O1;
	delay_1us(8);
	IIC_SCL_O0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL_O0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)    //��ʼ׼���ź���
    {              
     //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_O1;
		else
			IIC_SDA_O0;
		txd<<=1; 	  
		delay_1us(8);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_O1;
		delay_1us(8); 
		IIC_SCL_O0;	
		delay_1us(8);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL_O0; 
        delay_1us(8);
				IIC_SCL_O1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_1us(4); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}



























