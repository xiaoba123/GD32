/************************************************************ 
 * �ļ���USART0.c
************************************************************/

/************************* ͷ�ļ� *************************/

#include <stdio.h>
#include "Modbus_slave.h"
#include "USART0.h"

/************************* �궨�� *************************/

/************************ �������� ************************/
uint8_t SEND_ARR[207];    //���ݷ�������
short usartFlag=0;

extern uint16_t *data;   // ��ȡ����

/************************ �������� ************************/

	int fputc(int ch, FILE *f)
		{
				usart_data_transmit(USART0, (uint8_t)ch);
				while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
				return ch;
		}

/************************************************************ 
 * ����:       USART0_config(void)
 * ˵��:       ��������
************************************************************/
	void USART0_Config(void)
		{
				rcu_periph_clock_enable(RCU_GPIOA);    // ʹ��GPIOʱ��

				rcu_periph_clock_enable(RCU_USART0);   // ʹ�ܴ���ʱ��
			
				gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);  // �������ų�ʼ�� TX

				gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  // �������ų�ʼ��  RX

				usart_deinit(USART0);    // ���ڸ�λ
				usart_word_length_set(USART0, USART_WL_8BIT);  // �ֳ�
				usart_stop_bit_set(USART0, USART_STB_1BIT);    // ֹͣλ
				usart_parity_config(USART0, USART_PM_NONE);
				usart_baudrate_set(USART0, 115200U);     // ������
				usart_receive_config(USART0, USART_RECEIVE_ENABLE);     // ����ʹ��
				usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);   // ����ʹ��
				usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
				usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
			
				nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
				nvic_irq_enable(USART0_IRQn, 1, 1);//ʹ��USART0�ж�
				usart_interrupt_enable(USART0, USART_INT_RBNE);//�����жϴ�		

				usart_enable(USART0);           // ����ʹ��
		}
/************************************************************ 
 * ����:       USART0_SendData(uint16_t *buf,uint16_t len)
 * ˵��:       ���ڷ���һ�ֽں���
************************************************************/

	void USART0_Send_Byte(uint8_t ch)
		{
			usart_data_transmit(USART0,ch);
			while(usart_flag_get(USART0, USART_FLAG_TC) == RESET);
			usart_flag_clear(USART0, USART_FLAG_TC); 
		}

/************************************************************ 
 * ����:       USART0_Send_String(uint8_t *str)  
 * ˵��:       ���ڷ����ַ�������(ATָ��)
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
 * ����:       USART0_Send_Data(u16 *buf, uint8_t len)
 * ˵��:       ���ڷ������ݺ���
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
 * ����:       USART0_IRQHandler(void)
 * ˵��:       �жϽ��պ���
************************************************************/

void USART0_IRQHandler(void)                
{
//  uint8_t Res;
//	 if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))// ���������ж��¼�
//		{
//			usartFlag=1;
//			Res =usart_data_receive(USART0);	// ��ȡ���յ�������

//		 if( g_tModS.RxStatus==1)           // �����ݰ����ڴ���
//			{
//				 return ;
//			}		
//			g_tModS.RxBuf[g_tModS.RxCount++] = Res; // ���յ������ݴ�����ջ�����
//			g_tModS.TimeOut = 0;

//			if(g_tModS.RxCount == 1)         // �Ѿ��յ��˵ڶ����ַ�����
//			{
//				g_tModS.TimeRun = 1;           // ����modbus��ʱ����ʱ
//			}
//		}
		usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
		
} 

/****************************End*****************************/

