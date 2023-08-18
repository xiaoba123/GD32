#include "myiic.h"
#include "systick.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 
//初始化IIC
void IIC_Init(void)
{					
        rcu_periph_clock_enable(RCU_GPIOB); 	
				gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);  // 发送引脚初始化 TX

				gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);  // 接收引脚初始化  RX
}

//SDA设置为输入
void SDA_IN (void)
{
	      rcu_periph_clock_enable(RCU_GPIOB); 
				gpio_init(GPIOB,GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);  // 接收引脚初始化  RX
}

//SDA设置为输出
void SDA_OUT(void)
{ 
	      rcu_periph_clock_enable(RCU_GPIOB); 
        gpio_init(GPIOB,GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线设置为输出
	IIC_SDA_O1;	  	  
	IIC_SCL_O1;
	delay_1us(16);
 	IIC_SDA_O0;     //START:when CLK is high,DATA change form high to low 
	delay_1us(16);
	IIC_SCL_O0;     //钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL_O0;
	IIC_SDA_O0;//STOP:when CLK is high DATA change form low to high
 	delay_1us(16);
	IIC_SCL_O1; 
	IIC_SDA_O1;//发送I2C总线结束信号
	delay_1us(16);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;

	SDA_IN();      //SDA设置为输入  
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
	IIC_SCL_O0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
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
//不产生ACK应答		    
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL_O0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)    //开始准备信号线
    {              
     //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_O1;
		else
			IIC_SDA_O0;
		txd<<=1; 	  
		delay_1us(8);   //对TEA5767这三个延时都是必须的
		IIC_SCL_O1;
		delay_1us(8); 
		IIC_SCL_O0;	
		delay_1us(8);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
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
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}



























