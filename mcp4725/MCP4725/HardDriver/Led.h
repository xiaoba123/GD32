/************************************************************
 * 文件：.c

************************************************************/
#ifndef _LED_H
#define _LED_H
/************************* 头文件 *************************/

#include "gd32f10x.h"


/************************* 宏定义 *************************/
#define    Manual_LED_ON()    gpio_bit_reset(GPIOC, GPIO_PIN_1) 	 // 低电平   开灯 
#define    Manual_LED_OFF()   gpio_bit_set(GPIOC, GPIO_PIN_1)     // 高电平   关灯 

#define    Auto_LED_ON()      gpio_bit_reset(GPIOC, GPIO_PIN_2) 	   // 低电平   开灯 
#define    Auto_LED_OFF()     gpio_bit_set(GPIOC, GPIO_PIN_2)       // 高电平   关灯 

#define    Mix_LED_ON()       gpio_bit_reset(GPIOC, GPIO_PIN_3) 	   // 低电平   开灯 
#define    Mix_LED_OFF()      gpio_bit_set(GPIOC, GPIO_PIN_3)        // 高电平   关灯
/************************ 变量定义 ************************/

/************************ 函数定义 ************************/

void Led_Init(void);
void Procedure_Work(uint8_t a);
void ManMode_Work(uint8_t a);
void AutoMode_Work(uint8_t a);
/****************************End*****************************/
#endif


