
/************************* ͷ�ļ� *************************/


#include <stdio.h>
#include "USART1.h"
#include "Modbus_slave.h"
#include "Delay.h"
/************************* �궨�� *************************/

/************************ �������� ************************/
short RS485Flag = 0;                            // 485���ձ�־λ
extern uint16_t *data;   // ��ȡ����
/************************ �������� ************************/

//	int fputc(int ch, FILE *f)
//		{
//			RS485_TX_EN();
//			usart_data_transmit(USART1, (uint8_t)ch);
//			while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
//			return ch;
//		}


/************************************************************ 
 * ����:       RS485_Init(void)
 * ˵��:       RS485��ʼ��
************************************************************/

	void RS485_Init(uint32_t baud)
		{
			usart_disable(USART1);    
			rcu_periph_clock_enable(RCU_GPIOA);    // ʹ��GPIOAʱ��
			rcu_periph_clock_enable(RCU_USART1);   // ʹ�ܴ���ʱ��

			gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);       // �շ����ƹܽ�    CS			
			gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);       // �������ų�ʼ��  TX
			gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);  // �������ų�ʼ��  RX

		
			usart_deinit(USART1);                          // ���ڸ�λ
			usart_word_length_set(USART1, USART_WL_8BIT);  // �ֳ�
			usart_stop_bit_set(USART1, USART_STB_1BIT);    // ֹͣλ
			usart_parity_config(USART1, USART_PM_NONE);    // ����żУ��
			usart_baudrate_set(USART1, baud);           // ������
			usart_receive_config(USART1, USART_RECEIVE_ENABLE);        // ����ʹ��
			usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);      // ����ʹ��
			usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
			usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);


			nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);			
			nvic_irq_enable(USART1_IRQn, 1, 1);            // ʹ�ܴ����ж�
			usart_interrupt_enable(USART1, USART_INT_RBNE);// �����ж�ʹ��		
			usart_enable(USART1);                          // ����ʹ��
			
			RS485_RX_EN();
		}
		
		
		
/************************************************************ 
 * ����:       RS485_SendData(uint16_t *buf,uint16_t len)
 * ˵��:       ���ڷ���һ�ֽں���  ������modbus����ʹ�ã�
************************************************************/	
		
	void RS485_Send_Byte(uint8_t Modbus_byte)
		{
			RS485_TX_EN();        // ʹ�ܷ���
			usart_data_transmit(USART1,Modbus_byte);
			while(usart_flag_get(USART1, USART_FLAG_TC) == RESET);
			usart_flag_clear(USART1, USART_FLAG_TC); 
			RS485_RX_EN();
		}

		
		
/************************************************************ 
 * ����:       RS485_SendData(uint16_t *buf,uint16_t len)
* ˵��:        RS485��������
************************************************************/

	void RS485_SendData(uint8_t *buf,uint8_t len)
		{
			 uint8_t t;
			 RS485_TX_EN();        // ʹ�ܷ���
			 for(t=0;t<len;t++)      
			 {           
				 while(usart_flag_get(USART1, USART_FLAG_TC) == RESET);  
				 usart_data_transmit(USART1,buf[t]);
			 }     
			 while(usart_flag_get(USART1, USART_FLAG_TC) == RESET); 
       RS485_RX_EN();   			 
		}
		
/************************************************************ 
 * ����:       USART1_IRQHandler(uint8_t dat,uint8_t cmd)
 * ˵��:       �жϽ��պ���
************************************************************/

void USART1_IRQHandler(void)
	{
		uint8_t Res;					
		if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE))
			{
				RS485Flag=1;							
				Res =usart_data_receive(USART1);	// ��ȡ���յ�������
				if( g_tModS.RxStatus==1)           // �����ݰ����ڴ���
				{
					 return ;
				}
				g_tModS.RxBuf[g_tModS.RxCount++] = Res; // ���յ������ݴ�����ջ�����
				g_tModS.TimeOut = 0;

				if(g_tModS.RxCount == 1)  // �Ѿ��յ��˵ڶ����ַ�����
				{
					g_tModS.TimeRun = 1;    // ����modbus��ʱ����ʱ
				}													
			} 
		usart_interrupt_flag_clear(USART1, USART_INT_FLAG_RBNE);
}	

/*********************************************END OF FILE**********************/



