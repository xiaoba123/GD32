#include "Sht30.h"
#include "ANIIC0.h"
#include "systick.h"

	float Temperature=0;
	float Humidity=0;
/*软件复位SHT3x*/
void SHT3x_SoftReset(void)                    
{
    //SHT3x_WriteByte(0x30,0xA2);    //重新初始化SHT3x
}

/*引脚初始化*/
void SHT3x_Init(void)
{
  
  ANIIC_Init();
  

	ANIIC_Start();
	ANIIC_SendByte(0x44<<1|0);//0为写
	ANIIC_ReceiveAck();
	ANIIC_SendByte(0x21);
	ANIIC_ReceiveAck();
	ANIIC_SendByte(0x30);
	ANIIC_ReceiveAck();
	ANIIC_Stop();

	//SHT3x_WriteByte(0X27,0X21); //周期数据采集模式（每秒10次，Medium Repeatability）
}

void ReadSHT3x(uint8_t addr)
{


  uint16_t tem,hum;
	uint16_t buff[6];

	
	ANIIC_Start();
	ANIIC_SendByte(addr<<1 | 0);//写7位I2C设备地址加0作为写取位,1为读取位
	ANIIC_ReceiveAck();
	ANIIC_SendByte(0x2C);
	ANIIC_ReceiveAck();
	ANIIC_SendByte(0x06);
	ANIIC_ReceiveAck();
	ANIIC_Stop();
	delay_1ms(50);
	ANIIC_Start();
	ANIIC_SendByte(addr<<1 | 1);//写7位I2C设备地址加0作为写取位,1为读取位
	if(ANIIC_ReceiveAck()==0)
	{
		buff[0]=ANIIC_ReceiveByte();
		ANIIC_SendAck(RESET);
		buff[1]=ANIIC_ReceiveByte();
		ANIIC_SendAck(RESET);
		buff[2]=ANIIC_ReceiveByte();
		ANIIC_SendAck(RESET);
		buff[3]=ANIIC_ReceiveByte();
		ANIIC_SendAck(RESET);
		buff[4]=ANIIC_ReceiveByte();
		ANIIC_SendAck(RESET);
		buff[5]=ANIIC_ReceiveByte();
		ANIIC_SendAck(SET);
		ANIIC_Stop();
	}
	
	tem = ((buff[0]<<8) | buff[1]);//温度拼接
	hum = ((buff[3]<<8) | buff[4]);//湿度拼接
	
	/*转换实际温度*/
	Temperature= (175.0*(float)tem/65535.0-45.0) ;  // T = -45 + 175 * tem / (2^16-1)
	Humidity= (100.0*(float)hum/65535.0);           // RH = hum*100 / (2^16-1)
	
	if((Temperature>=-20)&&(Temperature<=125)&&(Humidity>=0)&&(Humidity<=100))//过滤错误数据
	{
//		humiture[0]=Temperature;
//		humiture[2]=Humidity;
//		sprintf(humiture_buff1,"%6.2f*C %6.2f%%",Temperature,Humidity);//111.01*C 100.01%（保留2位小数）
	}

	hum=0;
	tem=0;


}


