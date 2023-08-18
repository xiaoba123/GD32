#include "TIMER1.h"
#include "MCP4725.h" 
uint32_t TimCounter=0;  // 计数器
uint8_t k=0; 
extern uint8_t volflag;

	void Time_Init()
		{
			timer_parameter_struct timer_initpara;
			rcu_periph_clock_enable(RCU_TIMER1);
			timer_deinit(TIMER1);
			/* 定时器参数配置 */
			timer_initpara.prescaler         = 107;
			timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
			timer_initpara.counterdirection  = TIMER_COUNTER_UP;
			timer_initpara.period            = 9999;
			timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
			timer_initpara.repetitioncounter = 0;
			timer_init(TIMER1,&timer_initpara);
			nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
			nvic_irq_enable(TIMER1_IRQn, 0, 1);
			timer_interrupt_enable(TIMER1, TIMER_INT_UP);
			/* 自动加载定时器参数 */
			timer_auto_reload_shadow_enable(TIMER1);
			/* 使能定时器 */
			timer_enable(TIMER1);
		}

		
void TIMER1_IRQHandler(void)  
{

	  timer_interrupt_flag_clear(TIMER1,TIMER_FLAG_UP); //清除"更新中断标志位"		

		  TimCounter++;

		  if(TimCounter >= 500)
		  {
				TimCounter=0;	
        volflag	^=0x01;				
				++k;
				if(k>5)k=0;
				MCP4725_WriteData_Voltage(1000*k);
				
		  }

}

