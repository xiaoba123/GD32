/************************************************************
 * 文件：.c

************************************************************/

/************************* 头文件 *************************/

#include "Mod_Imple.h"

/************************* 宏定义 *************************/

/************************ 变量定义 ************************/



/************************ 函数定义 ************************/

/***********************************************************************
                 05  开关锁响应函数（修改设备类型 量程上下限）
***********************************************************************/	
	void Modbus_05_func(void)
		{	
			
		}	

		

/***********************************************************************
                            06  修改配置信息
***********************************************************************/	
	void Modbus_06_func(void)
		{	
			
		}	

		
/***********************************************************************
                        10  联动逻辑设置并写入Flash
***********************************************************************/			
		
   void Modbus_10_func(void)
		{
	
		}
		
		
	
/*
*********************************************************************************************************
*	函 数 名: State_GetPin
*	功能说明: 判断指定的管脚输出是1还是0
*	形    参: _pin : 管脚号， 0-31; 只能选1个，不能多选
*	返 回 值: 0或1
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
*	函 数 名: Check_pinOn
*	功能说明: 判断引脚是否已经置1。
*	形    参:  _no : 指示引脚号，范围 1 - n
*	返 回 值: 1表示已经工作，0表示未工作
*********************************************************************************************************
*/
// DI检测引脚输入状态 有输入 1转0  0转1
	uint8_t Check_pinOn1(uint8_t no)
{
	uint8_t pin;
  pin = no;
	if (State_GetPin(pin))
	{
		return 0;	/* 空闲 */
	}
	else
	{
		return 1;	/* 工作 */
	}
}

// DO检测引脚输出状态 有输出  1转1 0转0
uint8_t Check_pinOn2(uint8_t no)
{
	uint8_t pin;
  pin = no;
	if (State_GetPin(pin))
	{
		return 1;	/* 工作 */
	}
	else
	{
		return 0;	/* 空闲 */
	}
}


/****************************End*****************************/

