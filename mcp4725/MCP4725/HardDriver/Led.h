/************************************************************
 * �ļ���.c

************************************************************/
#ifndef _LED_H
#define _LED_H
/************************* ͷ�ļ� *************************/

#include "gd32f10x.h"


/************************* �궨�� *************************/
#define    Manual_LED_ON()    gpio_bit_reset(GPIOC, GPIO_PIN_1) 	 // �͵�ƽ   ���� 
#define    Manual_LED_OFF()   gpio_bit_set(GPIOC, GPIO_PIN_1)     // �ߵ�ƽ   �ص� 

#define    Auto_LED_ON()      gpio_bit_reset(GPIOC, GPIO_PIN_2) 	   // �͵�ƽ   ���� 
#define    Auto_LED_OFF()     gpio_bit_set(GPIOC, GPIO_PIN_2)       // �ߵ�ƽ   �ص� 

#define    Mix_LED_ON()       gpio_bit_reset(GPIOC, GPIO_PIN_3) 	   // �͵�ƽ   ���� 
#define    Mix_LED_OFF()      gpio_bit_set(GPIOC, GPIO_PIN_3)        // �ߵ�ƽ   �ص�
/************************ �������� ************************/

/************************ �������� ************************/

void Led_Init(void);
void Procedure_Work(uint8_t a);
void ManMode_Work(uint8_t a);
void AutoMode_Work(uint8_t a);
/****************************End*****************************/
#endif


