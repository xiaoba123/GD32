	
/************************************************************
 * 文件：.c

************************************************************/

/************************* 头文件 *************************/

#include "Hex_trans_bin.h"

/************************* 宏定义 *************************/
#define M 400
/************************ 变量定义 ************************/
//#include<stdio.h>
//#include<string.h>
//#include<ctype.h>
//#include<math.h>

	void fun10_2(long Sum)//十进制转二进制 
		{
			int i=0,arr[M];
			while(Sum)
			{
				arr[i++]=Sum%2;
				Sum=Sum/2;
			}
			for( ;i>0;i--)
			printf("%d",arr[i-1]);
		}

	void fun16_10(char str[])//十六进制转十进制 
		{
			long Sum=0,cet=0;
			int len=strlen(str);
			for(  ;len>0;len--)
			{
				if(isdigit(str[len-1])){
					Sum+=(str[len-1]-'0')*pow(16,cet);
				}else{
					Sum+=(str[len-1]-'A'+10)*pow(16,cet);
				}
				cet++;
			}
			
			fun10_2(Sum);//函数嵌套调用 
		}

	//int main()
	//{
	//	char str[100];
	//	gets(str);
	//	fun16_10(str);
	//	return 0;
	//}

/************************ 函数定义 ************************/

/***********************************************************************
                       十六进制转浮点数
***********************************************************************/			
//	float hextofloat (unsigned int number) 
//		{
//				//符号位
//				unsigned int sign = number >>31;
//				//幂数
//				int exponent = ((number >> 23) & 0xff) - 0x7F;
//				//尾数
//				unsigned int mantissa = number << 9; 
//				float value = 0; 
//				float mantissa2;
//				value = 0.5f;
//				mantissa2 = 0.0f; 
//				while (mantissa) 
//				{
//						if (mantissa & 0x80000000) 
//								mantissa2 += value; 
//						mantissa <<= 1; 
//						value *= 0.5f; 
//				}
//				value = (1.0f + mantissa2) * (pow (2, exponent)); 
//				if (sign) value = -value;
//				return value; 
//		}
/************************************************************ 

************************************************************/




/****************************End*****************************/


