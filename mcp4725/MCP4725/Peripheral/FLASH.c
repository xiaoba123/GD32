/************************************************************
 * �ļ���FLASH.c

************************************************************/

/************************* ͷ�ļ� *************************/

#include "FLASH.h"

/************************* �궨�� *************************/

/************************ �������� ************************/
uint32_t address = 0x00;  // ��ַ
uint16_t data1   = 0x01;  // 485Ĭ�ϵ�ַ

/************************ �������� ************************/

/************************ �������� ************************/

/************************************************************ 
 * ����:       fmc_erase_pages(void)
 * ˵��:       ����FLASHҳ
************************************************************/
	void fmc_erase_pages(uint32_t start_addr)
		{
			fmc_unlock();   // ���� FLASH ������

			fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_WPERR|FMC_FLAG_BANK0_PGERR);     // �����־
			fmc_page_erase(start_addr);
			fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_WPERR|FMC_FLAG_BANK0_PGERR);     // �����־	
			
			fmc_lock();   // ������ɺ�������������
		}

/************************************************************ 
 * ����:       fmc_program(void)
 * ˵��:       ��FLASH��ַд������
************************************************************/
	void fmc_program(uint32_t start_addr,uint32_t end_addr,uint32_t DATA)
		{
			fmc_unlock();            // ��д������֮ǰ������ FMC
			address = start_addr;    // �׵�ַ

			fmc_word_program(address, DATA);
			fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_WPERR|FMC_FLAG_BANK0_PGERR);     // �����־ 

			fmc_lock();             // д�����ݺ�����FMC
		}


	
/************************************************************ 
 * ����:       fmc_write_dataaddr(uint32_t staraddr,uint32_t endaddr,uint32_t dat)
 * ˵��:       ��FLASH��ַд���豸����������Ϣ
************************************************************/
	void fmc_write_dataaddr(uint32_t staraddr,uint32_t endaddr,uint32_t dat)
		{
			fmc_erase_pages(staraddr);            //  ����FLASH
			fmc_program(staraddr,endaddr,dat);    //  FLASHд��
		}
			
		
/************************************************************ 
 * ����:       fmc_write_DOdataAddr(uint32_t staraddr,uint32_t endaddr,uint32_t *data)
 * ˵��:       ��FLASH��ַд��DO����������Ϣ����������
************************************************************/		
		
	void fmc_write_DOdataAddr(uint32_t staraddr,uint32_t *data)
		{
			uint8_t t=0;
			fmc_unlock();//����
			fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_WPERR|FMC_FLAG_BANK0_PGERR);//��� FMC ��־
			fmc_page_erase(staraddr);//����
				for(t=0;t<6;t++)
					{
						fmc_word_program(staraddr+4*t,data[t]);					
					}	
			fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_WPERR|FMC_FLAG_BANK0_PGERR);//��� FMC ��־
			fmc_lock();//����				 
		}		
		
			

/****************************End*****************************/

