
/************************* 头文件 *************************/


#include <stdio.h>
#include "USART1.h"
#include "Modbus_slave.h"
#include "Delay.h"
/************************* 宏定义 *************************/

/************************ 变量定义 ************************/
short RS485Flag = 0;                            // 485接收标志位
extern uint16_t *data;   // 读取数据
/************************ 函数定义 ************************/

//	int fputc(int ch, FILE *f)
//		{
//			RS485_TX_EN();
//			usart_data_transmit(USART1, (uint8_t)ch);
//			while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
//			return ch;
//		}


/************************************************************ 
 * 函数:       RS485_Init(void)
 * 说明:       RS485初始化
************************************************************/

	void RS485_Init(uint32_t baud)
		{
			usart_disable(USART1);    
			rcu_periph_clock_enable(RCU_GPIOA);    // 使能GPIOA时钟
			rcu_periph_clock_enable(RCU_USART1);   // 使能串口时钟

			gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);       // 收发控制管脚    CS			
			gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);       // 发送引脚初始化  TX
			gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);  // 接收引脚初始化  RX

		
			usart_deinit(USART1);                          // 串口复位
			usart_word_length_set(USART1, USART_WL_8BIT);  // 字长
			usart_stop_bit_set(USART1, USART_STB_1BIT);    // 停止位
			usart_parity_config(USART1, USART_PM_NONE);    // 无奇偶校验
			usart_baudrate_set(USART1, baud);           // 波特率
			usart_receive_config(USART1, USART_RECEIVE_ENABLE);        // 接收使能
			usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);      // 发送使能
			usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
			usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);


			nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);			
			nvic_irq_enable(USART1_IRQn, 1, 1);            // 使能串口中断
			usart_interrupt_enable(USART1, USART_INT_RBNE);// 接收中断使能		
			usart_enable(USART1);                          // 串口使能
			
			RS485_RX_EN();
		}
		
		
		
/************************************************************ 
 * 函数:       RS485_SendData(uint16_t *buf,uint16_t len)
 * 说明:       串口发送一字节函数  （发送modbus命令使用）
************************************************************/	
		
	void RS485_Send_Byte(uint8_t Modbus_byte)
		{
			RS485_TX_EN();        // 使能发送
			usart_data_transmit(USART1,Modbus_byte);
			while(usart_flag_get(USART1, USART_FLAG_TC) == RESET);
			usart_flag_clear(USART1, USART_FLAG_TC); 
			RS485_RX_EN();
		}

		
		
/************************************************************ 
 * 函数:       RS485_SendData(uint16_t *buf,uint16_t len)
* 说明:        RS485发送数据
************************************************************/

	void RS485_SendData(uint8_t *buf,uint8_t len)
		{
			 uint8_t t;
			 RS485_TX_EN();        // 使能发送
			 for(t=0;t<len;t++)      
			 {           
				 while(usart_flag_get(USART1, USART_FLAG_TC) == RESET);  
				 usart_data_transmit(USART1,buf[t]);
			 }     
			 while(usart_flag_get(USART1, USART_FLAG_TC) == RESET); 
       RS485_RX_EN();   			 
		}
		
/************************************************************ 
 * 函数:       USART1_IRQHandler(uint8_t dat,uint8_t cmd)
 * 说明:       中断接收函数
************************************************************/

void USART1_IRQHandler(void)
	{
		uint8_t Res;					
		if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE))
			{
				RS485Flag=1;							
				Res =usart_data_receive(USART1);	// 读取接收到的数据
				if( g_tModS.RxStatus==1)           // 有数据包正在处理
				{
					 return ;
				}
				g_tModS.RxBuf[g_tModS.RxCount++] = Res; // 接收到的数据存入接收缓冲区
				g_tModS.TimeOut = 0;

				if(g_tModS.RxCount == 1)  // 已经收到了第二个字符数据
				{
					g_tModS.TimeRun = 1;    // 开启modbus定时器计时
				}													
			} 
		usart_interrupt_flag_clear(USART1, USART_INT_FLAG_RBNE);
}	

/*********************************************END OF FILE**********************/



