/************************************************************
 * 文件：FLASH.c

************************************************************/

/************************* 头文件 *************************/

#include "FLASH.h"

/************************* 宏定义 *************************/

/************************ 变量定义 ************************/
uint32_t address = 0x00;  // 地址
uint16_t data1   = 0x01;  // 485默认地址

/************************ 函数定义 ************************/

/************************ 函数定义 ************************/

/************************************************************ 
 * 函数:       fmc_erase_pages(void)
 * 说明:       擦除FLASH页
************************************************************/
	void fmc_erase_pages(uint32_t start_addr)
		{
			fmc_unlock();   // 解锁 FLASH 操作区

			fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_WPERR|FMC_FLAG_BANK0_PGERR);     // 清除标志
			fmc_page_erase(start_addr);
			fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_WPERR|FMC_FLAG_BANK0_PGERR);     // 清除标志	
			
			fmc_lock();   // 擦除完成后，锁定主操作区
		}

/************************************************************ 
 * 函数:       fmc_program(void)
 * 说明:       往FLASH地址写入内容
************************************************************/
	void fmc_program(uint32_t start_addr,uint32_t end_addr,uint32_t DATA)
		{
			fmc_unlock();            // 在写入数据之前，解锁 FMC
			address = start_addr;    // 首地址

			fmc_word_program(address, DATA);
			fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_WPERR|FMC_FLAG_BANK0_PGERR);     // 清除标志 

			fmc_lock();             // 写入数据后，锁定FMC
		}


	
/************************************************************ 
 * 函数:       fmc_write_dataaddr(uint32_t staraddr,uint32_t endaddr,uint32_t dat)
 * 说明:       往FLASH地址写入设备各种配置信息
************************************************************/
	void fmc_write_dataaddr(uint32_t staraddr,uint32_t endaddr,uint32_t dat)
		{
			fmc_erase_pages(staraddr);            //  擦除FLASH
			fmc_program(staraddr,endaddr,dat);    //  FLASH写入
		}
			
		
/************************************************************ 
 * 函数:       fmc_write_DOdataAddr(uint32_t staraddr,uint32_t endaddr,uint32_t *data)
 * 说明:       往FLASH地址写入DO联动配置信息的数组内容
************************************************************/		
		
	void fmc_write_DOdataAddr(uint32_t staraddr,uint32_t *data)
		{
			uint8_t t=0;
			fmc_unlock();//解锁
			fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_WPERR|FMC_FLAG_BANK0_PGERR);//清除 FMC 标志
			fmc_page_erase(staraddr);//擦除
				for(t=0;t<6;t++)
					{
						fmc_word_program(staraddr+4*t,data[t]);					
					}	
			fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_WPERR|FMC_FLAG_BANK0_PGERR);//清除 FMC 标志
			fmc_lock();//上锁				 
		}		
		
			

/****************************End*****************************/

