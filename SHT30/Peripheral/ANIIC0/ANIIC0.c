#include "ANIIC0.h"
#include "systick.h"


#define ANIIC_ADDRESS		0xC2   //�ӻ���ַ

//IO��������	 

#define SCL_PORT       GPIOB
#define SDA_PORT       GPIOB
#define SCL_PIN        GPIO_PIN_6
#define SDA_PIN        GPIO_PIN_7


/************************************************************ 
 * ����:       ANIIC_Init(void)
 * ˵��:       ���IIC��ʼ������
************************************************************/

//��ʼ��IIC
void ANIIC_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
  gpio_init(SCL_PORT,GPIO_MODE_OUT_OD,GPIO_OSPEED_50MHZ,SCL_PIN | SDA_PIN);
	gpio_bit_set(SCL_PORT, SCL_PIN | SDA_PIN);
}



//����ʱ����
void ANIIC_W_SCL(bit_status bitvalue)
{
   gpio_bit_write(SCL_PORT,SCL_PIN,bitvalue);
	 delay_1us(10);
}

//����������
void ANIIC_W_SDA(bit_status bitvalue)
{
   gpio_bit_write(SDA_PORT,SDA_PIN,bitvalue);
	 delay_1us(10);
}

//�����߶�����
uint8_t ANIIC_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = gpio_input_bit_get(SDA_PORT, SDA_PIN);
	delay_1us(10);
	return BitValue;
}



/************************************************************ 
 * ����:       ANIIC_Start(void)
 * ˵��:       ������ʼ�ź�
************************************************************/

//����IIC��ʼ�ź�    SCLΪ��ʱ SDA�Ӹ߱��
void ANIIC_Start(void)
{
	ANIIC_W_SDA(SET);
	ANIIC_W_SCL(SET);
	ANIIC_W_SDA(RESET);
	ANIIC_W_SCL(RESET);
}

/************************************************************ 
 * ����:       ANIIC_Stop(void)
 * ˵��:       ����ֹͣ�ź�
************************************************************/

//����IICֹͣ�ź�    SCLΪ��ʱ SDA�ӵͱ��
void ANIIC_Stop(void)
{
	ANIIC_W_SDA(RESET);
	ANIIC_W_SCL(SET);
	ANIIC_W_SDA(SET);
}


/************************************************************ 
 * ����:       ANIIC_SendAck(bit_status bitvalue)
 * ˵��:       ����Ӧ���ź�

����ֵ��SET��  �����ӻ�Ӧ��
        RESET�����ӻ���ӦӦ��

************************************************************/
void  ANIIC_SendAck(bit_status bitvalue)
{
	ANIIC_W_SDA(bitvalue);
	ANIIC_W_SCL(SET);
	ANIIC_W_SCL(RESET);

}




/************************************************************ 
 * ����:       ANIIC_ReceiveAck(void)
 * ˵��:       ����Ӧ���ź�

����ֵ��SET��  ���ܷ���ӦӦ��
        RESET������Ӧ��ɹ�

************************************************************/

uint8_t ANIIC_ReceiveAck(void)
{
	uint8_t AckBit;
	ANIIC_W_SDA(SET);// �ͷ�SDA����Ȩ���ӻ�
	ANIIC_W_SCL(SET);
	AckBit = ANIIC_R_SDA();
	ANIIC_W_SCL(RESET);
	return AckBit;
}


/************************************************************ 
 * ����:       ANIIC_SendByte(uint8_t Byte)
 * ˵��:       IIC����һ���ֽ�
************************************************************/


void ANIIC_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{

		if((Byte & (0x80 >> i))) // ��λ����
			ANIIC_W_SDA(SET);
		else
			ANIIC_W_SDA(RESET);
		ANIIC_W_SCL(SET);        // �ͷ�SCL 
		ANIIC_W_SCL(RESET);
	}
}


/************************************************************ 
 * ����:       ANIIC_ReceiveByte(void)
 * ˵��:       IIC����һ���ֽ�
************************************************************/
uint8_t ANIIC_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	ANIIC_W_SDA(SET);         // �ͷ�SDA���ӻ�����
	for (i = 0; i < 8; i ++)
	{
		ANIIC_W_SCL(SET);       // �����ڸߵ�ƽ�ڼ��ȡ
		if (ANIIC_R_SDA() == 1)
    {
			Byte |= (0x80 >> i);
		}
		ANIIC_W_SCL(RESET);
	}		
	return Byte;
}


/************************************************************ 
 * ����:       ANIIC_WriteReg(uint8_t RegAddress, uint8_t Data)
 * ˵��:       IICд�ӻ�ָ���Ĵ���
************************************************************/
void ANIIC_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	ANIIC_Start();
	
	ANIIC_SendByte(ANIIC_ADDRESS);      // �ӻ���ַ  ����һλ��дλ��0 ����д
	ANIIC_ReceiveAck();        // ֻ���� ������Ӧ��
	ANIIC_SendByte(RegAddress);// �ӻ��Ĵ�����ַ
	ANIIC_ReceiveAck();        
	ANIIC_SendByte(Data);      // ��д������  ����д������� ��ַ�Զ�����ֻ��ѭ��������
	ANIIC_ReceiveAck();        
	
	ANIIC_Stop(); 
}


/************************************************************ 
 * ����:       ANIIC_ReadReg(uint8_t RegAddress)
 * ˵��:       IIC���ӻ�ָ���Ĵ���
************************************************************/

uint8_t ANIIC_ReadReg(uint8_t RegAddress)
{
	uint8_t data=0;
	ANIIC_Start();
	
	ANIIC_SendByte(ANIIC_ADDRESS);      // �ӻ���ַ
	ANIIC_ReceiveAck();        // ֻ���� ������Ӧ��
	ANIIC_SendByte(RegAddress);// �ӻ��Ĵ�����ַ
	ANIIC_ReceiveAck(); 
	
	ANIIC_Start();
  ANIIC_SendByte(ANIIC_ADDRESS|0x01);  //�ӻ���ַ����1  ���ǰѶ�дλ��Ϊ1  1 �����  
	ANIIC_ReceiveAck(); 
	
	data=ANIIC_ReceiveByte();
	ANIIC_SendAck(SET);      // ���������ӻ�Ӧ��  �ӻ����ٷ�������  ��Ҫ������������ �����Ϊ����һ��Ӧ�� 
	                         // Ȼ������ѭ���������� ÿ�ν��ܶ�Ҫ��Ӧ�� ֱ�����һ�ν���һ������Ӧ��
	ANIIC_Stop(); 
	return data;
	
}

