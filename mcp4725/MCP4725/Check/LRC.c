/* ----------------------- Platform includes --------------------------------*/
#include "LRC.h" 

/******************************************************************
功能: LRC校验
输入:
输出:
******************************************************************/
unsigned char Check_LRC(unsigned char* data, int data_len)
	{

		int i = 0;
		unsigned char lrc = 0;
		
		for (i = 0; i < data_len; i++)
			{
			lrc ^=  data[i];
			}

		return lrc;
	}




