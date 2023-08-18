/************************************************************
 * �ļ���Implement.c

************************************************************/

/************************* ͷ�ļ� *************************/

#include "Implement.h"

/************************* �궨�� *************************/

/************************ �������� ************************/


/************************ �������� ************************/


/************************************************************ 
 * ����:       System_Init(void)
 * ˵��:       ϵͳ��ʼ��
************************************************************/

	void System_Init(void)
		{
			rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);
			rcu_periph_clock_enable(RCU_AF);   //�ܽŸ���ʱ��alternate function clockʹ��
			systick_config();

			delay_1ms(200);
		}

/************************************************************ 
 * ����:       Implement(void)
 * ˵��:       ִ�к���
************************************************************/

	void Implement(void)
		{
 
			MCP4725_Init();   //MCP4725��ʼ��	
			while(1)
			{

						MCP4725_WriteData_Voltage(4000);				

				}
	   }


/****************************End*****************************/
