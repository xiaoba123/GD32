#include "ANIIC0.h"
#include "systick.h"


#define ANIIC_ADDRESS		0xC2   //从机地址

//IO操作函数	 

#define SCL_PORT       GPIOB
#define SDA_PORT       GPIOB
#define SCL_PIN        GPIO_PIN_6
#define SDA_PIN        GPIO_PIN_7


/************************************************************ 
 * 函数:       ANIIC_Init(void)
 * 说明:       软件IIC初始化函数
************************************************************/

//初始化IIC
void ANIIC_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
  gpio_init(SCL_PORT,GPIO_MODE_OUT_OD,GPIO_OSPEED_50MHZ,SCL_PIN | SDA_PIN);
	gpio_bit_set(SCL_PORT, SCL_PIN | SDA_PIN);
}



//操作时钟线
void ANIIC_W_SCL(bit_status bitvalue)
{
   gpio_bit_write(SCL_PORT,SCL_PIN,bitvalue);
	 delay_1us(10);
}

//操作数据线
void ANIIC_W_SDA(bit_status bitvalue)
{
   gpio_bit_write(SDA_PORT,SDA_PIN,bitvalue);
	 delay_1us(10);
}

//数据线读数据
uint8_t ANIIC_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = gpio_input_bit_get(SDA_PORT, SDA_PIN);
	delay_1us(10);
	return BitValue;
}



/************************************************************ 
 * 函数:       ANIIC_Start(void)
 * 说明:       产生起始信号
************************************************************/

//产生IIC起始信号    SCL为高时 SDA从高变低
void ANIIC_Start(void)
{
	ANIIC_W_SDA(SET);
	ANIIC_W_SCL(SET);
	ANIIC_W_SDA(RESET);
	ANIIC_W_SCL(RESET);
}

/************************************************************ 
 * 函数:       ANIIC_Stop(void)
 * 说明:       产生停止信号
************************************************************/

//产生IIC停止信号    SCL为高时 SDA从低变高
void ANIIC_Stop(void)
{
	ANIIC_W_SDA(RESET);
	ANIIC_W_SCL(SET);
	ANIIC_W_SDA(SET);
}


/************************************************************ 
 * 函数:       ANIIC_SendAck(bit_status bitvalue)
 * 说明:       发送应答信号

发送值：SET，  不给从机应答
        RESET，给从机响应应答

************************************************************/
void  ANIIC_SendAck(bit_status bitvalue)
{
	ANIIC_W_SDA(bitvalue);
	ANIIC_W_SCL(SET);
	ANIIC_W_SCL(RESET);

}




/************************************************************ 
 * 函数:       ANIIC_ReceiveAck(void)
 * 说明:       发送应答信号

发送值：SET，  接受非响应应答
        RESET，接受应答成功

************************************************************/

uint8_t ANIIC_ReceiveAck(void)
{
	uint8_t AckBit;
	ANIIC_W_SDA(SET);// 释放SDA控制权给从机
	ANIIC_W_SCL(SET);
	AckBit = ANIIC_R_SDA();
	ANIIC_W_SCL(RESET);
	return AckBit;
}


/************************************************************ 
 * 函数:       ANIIC_SendByte(uint8_t Byte)
 * 说明:       IIC发送一个字节
************************************************************/


void ANIIC_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{

		if((Byte & (0x80 >> i))) // 高位先行
			ANIIC_W_SDA(SET);
		else
			ANIIC_W_SDA(RESET);
		ANIIC_W_SCL(SET);        // 释放SCL 
		ANIIC_W_SCL(RESET);
	}
}


/************************************************************ 
 * 函数:       ANIIC_ReceiveByte(void)
 * 说明:       IIC接受一个字节
************************************************************/
uint8_t ANIIC_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	ANIIC_W_SDA(SET);         // 释放SDA给从机控制
	for (i = 0; i < 8; i ++)
	{
		ANIIC_W_SCL(SET);       // 主机在高电平期间读取
		if (ANIIC_R_SDA() == 1)
    {
			Byte |= (0x80 >> i);
		}
		ANIIC_W_SCL(RESET);
	}		
	return Byte;
}


/************************************************************ 
 * 函数:       ANIIC_WriteReg(uint8_t RegAddress, uint8_t Data)
 * 说明:       IIC写从机指定寄存器
************************************************************/
void ANIIC_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	ANIIC_Start();
	
	ANIIC_SendByte(ANIIC_ADDRESS);      // 从机地址  最后的一位读写位是0 代表写
	ANIIC_ReceiveAck();        // 只接受 不处理应答
	ANIIC_SendByte(RegAddress);// 从机寄存器地址
	ANIIC_ReceiveAck();        
	ANIIC_SendByte(Data);      // 待写入数据  可以写多个数据 地址自动增加只需循环发数据
	ANIIC_ReceiveAck();        
	
	ANIIC_Stop(); 
}


/************************************************************ 
 * 函数:       ANIIC_ReadReg(uint8_t RegAddress)
 * 说明:       IIC读从机指定寄存器
************************************************************/

uint8_t ANIIC_ReadReg(uint8_t RegAddress)
{
	uint8_t data=0;
	ANIIC_Start();
	
	ANIIC_SendByte(ANIIC_ADDRESS);      // 从机地址
	ANIIC_ReceiveAck();        // 只接受 不处理应答
	ANIIC_SendByte(RegAddress);// 从机寄存器地址
	ANIIC_ReceiveAck(); 
	
	ANIIC_Start();
  ANIIC_SendByte(ANIIC_ADDRESS|0x01);  //从机地址或上1  就是把读写位改为1  1 代表读  
	ANIIC_ReceiveAck(); 
	
	data=ANIIC_ReceiveByte();
	ANIIC_SendAck(SET);      // 主机不给从机应答  从机不再发送数据  若要继续接受数据 则需改为发送一个应答 
	                         // 然后主机循环接收数据 每次接受都要给应答 直到最后一次接受一个不给应答
	ANIIC_Stop(); 
	return data;
	
}

