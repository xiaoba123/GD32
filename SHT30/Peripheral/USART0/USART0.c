/************************************************************ 
 * 文件：USART0.c
************************************************************/

/************************* 头文件 *************************/

#include <stdio.h>
#include "Modbus_slave.h"
#include "USART0.h"

/************************* 宏定义 *************************/

/************************ 变量定义 ************************/
uint8_t SEND_ARR[207];    //数据发送数组
short usartFlag=0;

extern uint16_t *data;   // 读取数据

/************************ 函数定义 ************************/

	int fputc(int ch, FILE *f)
		{
				usart_data_transmit(USART0, (uint8_t)ch);
				while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
				return ch;
		}

/************************************************************ 
 * 函数:       USART0_config(void)
 * 说明:       串口配置
************************************************************/
	void USART0_Config(void)
		{
				rcu_periph_clock_enable(RCU_GPIOA);    // 使能GPIO时钟

				rcu_periph_clock_enable(RCU_USART0);   // 使能串口时钟
			
				gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);  // 发送引脚初始化 TX

				gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  // 接收引脚初始化  RX

				usart_deinit(USART0);    // 串口复位
				usart_word_length_set(USART0, USART_WL_8BIT);  // 字长
				usart_stop_bit_set(USART0, USART_STB_1BIT);    // 停止位
				usart_parity_config(USART0, USART_PM_NONE);
				usart_baudrate_set(USART0, 115200U);     // 波特率
				usart_receive_config(USART0, USART_RECEIVE_ENABLE);     // 接收使能
				usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);   // 发送使能
				usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
				usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
			
				nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
				nvic_irq_enable(USART0_IRQn, 1, 1);//使能USART0中断
				usart_interrupt_enable(USART0, USART_INT_RBNE);//接收中断打开		

				usart_enable(USART0);           // 串口使能
		}
/************************************************************ 
 * 函数:       USART0_SendData(uint16_t *buf,uint16_t len)
 * 说明:       串口发送一字节函数
************************************************************/

	void USART0_Send_Byte(uint8_t ch)
		{
			usart_data_transmit(USART0,ch);
			while(usart_flag_get(USART0, USART_FLAG_TC) == RESET);
			usart_flag_clear(USART0, USART_FLAG_TC); 
		}

/************************************************************ 
 * 函数:       USART0_Send_String(uint8_t *str)  
 * 说明:       串口发送字符串函数(AT指令)
************************************************************/

 void USART0_Send_String(uint8_t *str) 
	 { 
		unsigned int k=0;
			do 
			{
				USART0_Send_Byte(*(str + k));
				k++;
			} 
			while(*(str + k)!='\0');		 		
	 }
		


/************************************************************ 
 * 函数:       USART0_Send_Data(u16 *buf, uint8_t len)
 * 说明:       串口发送数据函数
************************************************************/	 
		
	void USART0_Send_Data(uint8_t *buf, uint8_t len)
		{	
			 uint8_t t;
			 for(t=0;t<len;t++)      
			 {           
				 while(usart_flag_get(USART0, USART_FLAG_TC) == RESET);  
				 usart_data_transmit(USART0,buf[t]);
			 }     
			 while(usart_flag_get(USART0, USART_FLAG_TC) == RESET); 			

		}

 
		
		
		
/************************************************************ 
 * 函数:       USART0_IRQHandler(void)
 * 说明:       中断接收函数
************************************************************/

void USART0_IRQHandler(void)                
{
//  uint8_t Res;
//	 if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))// 发生串口中断事件
//		{
//			usartFlag=1;
//			Res =usart_data_receive(USART0);	// 读取接收到的数据

//		 if( g_tModS.RxStatus==1)           // 有数据包正在处理
//			{
//				 return ;
//			}		
//			g_tModS.RxBuf[g_tModS.RxCount++] = Res; // 接收到的数据存入接收缓冲区
//			g_tModS.TimeOut = 0;

//			if(g_tModS.RxCount == 1)         // 已经收到了第二个字符数据
//			{
//				g_tModS.TimeRun = 1;           // 开启modbus定时器计时
//			}
//		}
		usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
		
} 

/****************************End*****************************/

