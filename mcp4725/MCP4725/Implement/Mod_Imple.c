/************************************************************
 * �ļ���.c

************************************************************/

/************************* ͷ�ļ� *************************/

#include "Mod_Imple.h"

/************************* �궨�� *************************/

/************************ �������� ************************/



/************************ �������� ************************/

/***********************************************************************
                 05  ��������Ӧ�������޸��豸���� ���������ޣ�
***********************************************************************/	
	void Modbus_05_func(void)
		{	
			
		}	

		

/***********************************************************************
                            06  �޸�������Ϣ
***********************************************************************/	
	void Modbus_06_func(void)
		{	
			
		}	

		
/***********************************************************************
                        10  �����߼����ò�д��Flash
***********************************************************************/			
		
   void Modbus_10_func(void)
		{
	
		}
		
		
	
/*
*********************************************************************************************************
*	�� �� ��: State_GetPin
*	����˵��: �ж�ָ���Ĺܽ������1����0
*	��    ��: _pin : �ܽźţ� 0-31; ֻ��ѡ1�������ܶ�ѡ
*	�� �� ֵ: 0��1
*********************************************************************************************************
*/
uint8_t State_GetPin(uint32_t pin)
	{
		uint8_t TEM;
		switch(pin)
		{
// DI1-8			
//			case 0x0001:TEM=gpio_input_bit_get(GPIOD,GPIO_PIN_8);break;
//			case 0x0002:TEM=gpio_input_bit_get(GPIOD,GPIO_PIN_9);break;
//			case 0x0003:TEM=gpio_input_bit_get(GPIOD,GPIO_PIN_10);break;
//			case 0x0004:TEM=gpio_input_bit_get(GPIOD,GPIO_PIN_11);break;
//			case 0x0005:TEM=gpio_input_bit_get(GPIOD,GPIO_PIN_12);break;
//			case 0x0006:TEM=gpio_input_bit_get(GPIOD,GPIO_PIN_13);break;
//			case 0x0007:TEM=gpio_input_bit_get(GPIOD,GPIO_PIN_14);break;
//			case 0x0008:TEM=gpio_input_bit_get(GPIOD,GPIO_PIN_15);break;	
//			case 0x0009:TEM=gpio_input_bit_get(GPIOC,GPIO_PIN_6);break;
//			case 0x000A:TEM=gpio_input_bit_get(GPIOC,GPIO_PIN_7);break;
//			case 0x000B:TEM=gpio_input_bit_get(GPIOC,GPIO_PIN_8);break;
//			case 0x000C:TEM=gpio_input_bit_get(GPIOC,GPIO_PIN_9);break;
//			case 0x000D:TEM=gpio_input_bit_get(GPIOA,GPIO_PIN_8);break;
//			case 0x000E:TEM=gpio_input_bit_get(GPIOA,GPIO_PIN_11);break;
//			case 0x000F:TEM=gpio_input_bit_get(GPIOA,GPIO_PIN_12);break;
//			case 0x0010:TEM=gpio_input_bit_get(GPIOA,GPIO_PIN_15);break;				
			
// DO1-8
//			case 0x0011:TEM=gpio_output_bit_get(GPIOE,GPIO_PIN_0);break;
//			case 0x0012:TEM=gpio_output_bit_get(GPIOB,GPIO_PIN_9);break;
//			case 0x0013:TEM=gpio_output_bit_get(GPIOB,GPIO_PIN_8);break;
//			case 0x0014:TEM=gpio_output_bit_get(GPIOB,GPIO_PIN_7);break;
//			case 0x0015:TEM=gpio_output_bit_get(GPIOB,GPIO_PIN_6);break;
//			case 0x0016:TEM=gpio_output_bit_get(GPIOB,GPIO_PIN_5);break;
//			case 0x0017:TEM=gpio_output_bit_get(GPIOB,GPIO_PIN_4);break;
//			case 0x0018:TEM=gpio_output_bit_get(GPIOB,GPIO_PIN_3);break;	
//			case 0x0019:TEM=gpio_output_bit_get(GPIOD,GPIO_PIN_7);break;
//			case 0x001A:TEM=gpio_output_bit_get(GPIOD,GPIO_PIN_6);break;
//			case 0x001B:TEM=gpio_output_bit_get(GPIOD,GPIO_PIN_5);break;
//			case 0x001C:TEM=gpio_output_bit_get(GPIOD,GPIO_PIN_4);break;
//			case 0x001D:TEM=gpio_output_bit_get(GPIOD,GPIO_PIN_3);break;
//			case 0x001E:TEM=gpio_output_bit_get(GPIOD,GPIO_PIN_2);break;
//			case 0x001F:TEM=gpio_output_bit_get(GPIOD,GPIO_PIN_1);break;
//			case 0x0020:TEM=gpio_output_bit_get(GPIOD,GPIO_PIN_0);break;					
			
		}
		return TEM;
	}

	

	
/*
*********************************************************************************************************
*	�� �� ��: Check_pinOn
*	����˵��: �ж������Ƿ��Ѿ���1��
*	��    ��:  _no : ָʾ���źţ���Χ 1 - n
*	�� �� ֵ: 1��ʾ�Ѿ�������0��ʾδ����
*********************************************************************************************************
*/
// DI�����������״̬ ������ 1ת0  0ת1
	uint8_t Check_pinOn1(uint8_t no)
{
	uint8_t pin;
  pin = no;
	if (State_GetPin(pin))
	{
		return 0;	/* ���� */
	}
	else
	{
		return 1;	/* ���� */
	}
}

// DO����������״̬ �����  1ת1 0ת0
uint8_t Check_pinOn2(uint8_t no)
{
	uint8_t pin;
  pin = no;
	if (State_GetPin(pin))
	{
		return 1;	/* ���� */
	}
	else
	{
		return 0;	/* ���� */
	}
}


/****************************End*****************************/

