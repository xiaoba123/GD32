#include "Sht30.h"
#include "ANIIC0.h"
#include "systick.h"

	float Temperature=0;
	float Humidity=0;
/*�����λSHT3x*/
void SHT3x_SoftReset(void)                    
{
    //SHT3x_WriteByte(0x30,0xA2);    //���³�ʼ��SHT3x
}

/*���ų�ʼ��*/
void SHT3x_Init(void)
{
  
  ANIIC_Init();
  

	ANIIC_Start();
	ANIIC_SendByte(0x44<<1|0);//0Ϊд
	ANIIC_ReceiveAck();
	ANIIC_SendByte(0x21);
	ANIIC_ReceiveAck();
	ANIIC_SendByte(0x30);
	ANIIC_ReceiveAck();
	ANIIC_Stop();

	//SHT3x_WriteByte(0X27,0X21); //�������ݲɼ�ģʽ��ÿ��10�Σ�Medium Repeatability��
}

void ReadSHT3x(uint8_t addr)
{


  uint16_t tem,hum;
	uint16_t buff[6];

	
	ANIIC_Start();
	ANIIC_SendByte(addr<<1 | 0);//д7λI2C�豸��ַ��0��Ϊдȡλ,1Ϊ��ȡλ
	ANIIC_ReceiveAck();
	ANIIC_SendByte(0x2C);
	ANIIC_ReceiveAck();
	ANIIC_SendByte(0x06);
	ANIIC_ReceiveAck();
	ANIIC_Stop();
	delay_1ms(50);
	ANIIC_Start();
	ANIIC_SendByte(addr<<1 | 1);//д7λI2C�豸��ַ��0��Ϊдȡλ,1Ϊ��ȡλ
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
	
	tem = ((buff[0]<<8) | buff[1]);//�¶�ƴ��
	hum = ((buff[3]<<8) | buff[4]);//ʪ��ƴ��
	
	/*ת��ʵ���¶�*/
	Temperature= (175.0*(float)tem/65535.0-45.0) ;  // T = -45 + 175 * tem / (2^16-1)
	Humidity= (100.0*(float)hum/65535.0);           // RH = hum*100 / (2^16-1)
	
	if((Temperature>=-20)&&(Temperature<=125)&&(Humidity>=0)&&(Humidity<=100))//���˴�������
	{
//		humiture[0]=Temperature;
//		humiture[2]=Humidity;
//		sprintf(humiture_buff1,"%6.2f*C %6.2f%%",Temperature,Humidity);//111.01*C 100.01%������2λС����
	}

	hum=0;
	tem=0;


}


