	
/************************************************************
 * �ļ���.c

************************************************************/

/************************* ͷ�ļ� *************************/

#include "Hex_trans_bin.h"

/************************* �궨�� *************************/
#define M 400
/************************ �������� ************************/
//#include<stdio.h>
//#include<string.h>
//#include<ctype.h>
//#include<math.h>

	void fun10_2(long Sum)//ʮ����ת������ 
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

	void fun16_10(char str[])//ʮ������תʮ���� 
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
			
			fun10_2(Sum);//����Ƕ�׵��� 
		}

	//int main()
	//{
	//	char str[100];
	//	gets(str);
	//	fun16_10(str);
	//	return 0;
	//}

/************************ �������� ************************/

/***********************************************************************
                       ʮ������ת������
***********************************************************************/			
//	float hextofloat (unsigned int number) 
//		{
//				//����λ
//				unsigned int sign = number >>31;
//				//����
//				int exponent = ((number >> 23) & 0xff) - 0x7F;
//				//β��
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


