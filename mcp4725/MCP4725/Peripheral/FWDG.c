/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：FWDG.c
************************************************************/

/************************* 头文件 *************************/

#include "FWDG.h"

/************************* 宏定义 *************************/

/************************ 变量定义 ************************/

/************************ 函数定义 ************************/


/************************************************************ 
 * 函数:       FWDG_Config(void)
 * 说明:       FWDG配置
************************************************************/
// 超时时间=周期*初值=（分频系数/40k）*rlr=(2^PR寄存器的值)*4*rlr(装载值)
void FWDG_Config(void)
{
	  /* 使能 40k 时钟 */
  rcu_osci_on(RCU_IRC40K);  // IRC40K时钟打开
	/* 看门狗计数时钟配置: 40KHz(IRC40K) / 64 = 0.625 KHz */
	fwdgt_config(625*4, FWDGT_PSC_DIV64);  // 看门狗时间设置 Tout=装载值*周期（1/频率）
	fwdgt_enable();  // 看门狗使能
}



/****************************End*****************************/

