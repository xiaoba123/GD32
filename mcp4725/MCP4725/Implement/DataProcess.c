
#include "DataProcess.h"
#include "string.h"
#include <math.h>
#include <ctype.h>

extern uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value);


/*
*********************************************************************************************************
*	�� �� ��: BEBufToUint16
*	����˵��: ��2�ֽ�����(���Big Endian���򣬸��ֽ���ǰ)ת��Ϊ16λ����
*	��    ��: _pBuf : ����
*	�� �� ֵ: 16λ����ֵ
*
*   ���(Big Endian)��С��(Little Endian)
*********************************************************************************************************
*/
uint16_t BEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[0] << 8) | _pBuf[1]);
}

/*
*********************************************************************************************************
*	�� �� ��: LEBufToUint16
*	����˵��: ��2�ֽ�����(С��Little Endian�����ֽ���ǰ)ת��Ϊ16λ����
*	��    ��: _pBuf : ����
*	�� �� ֵ: 16λ����ֵ
*********************************************************************************************************
*/
uint16_t LEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[1] << 8) | _pBuf[0]);
}


/*
*********************************************************************************************************
*	�� �� ��: BEBufToUint32
*	����˵��: ��4�ֽ�����(���Big Endian���򣬸��ֽ���ǰ)ת��Ϊ16λ����
*	��    ��: _pBuf : ����
*	�� �� ֵ: 16λ����ֵ
*
*   ���(Big Endian)��С��(Little Endian)
*********************************************************************************************************
*/
uint32_t BEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[0] << 24) | ((uint32_t)_pBuf[1] << 16) | ((uint32_t)_pBuf[2] << 8) | _pBuf[3]);
}

/*
*********************************************************************************************************
*	�� �� ��: LEBufToUint32
*	����˵��: ��4�ֽ�����(С��Little Endian�����ֽ���ǰ)ת��Ϊ16λ����
*	��    ��: _pBuf : ����
*	�� �� ֵ: 16λ����ֵ
*********************************************************************************************************
*/
uint32_t LEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[3] << 24) | ((uint32_t)_pBuf[2] << 16) | ((uint32_t)_pBuf[1] << 8) | _pBuf[0]);
}


/*
*********************************************************************************************************
*	�� �� ��: DEC_to_BIN
*	����˵��: ��ʮ������ת��Ϊ������
*	��    ��: Sum : ����
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
	void DEC_to_BIN(long Sum)  //ʮ����ת������ 
		{
			//int i=0;
			//int arr[400];
			while(Sum)
			{
				//arr[i++]=Sum%2;
				Sum=Sum/2;
			}
			//for( ;i>0;i--)
			//printf("%d",arr[i-1]);
		}

		
/*
*********************************************************************************************************
*	�� �� ��: HEX_to_DEC
*	����˵��: ��ʮ��������ת��Ϊʮ������
*	��    ��: _pBuf : ����
*	�� �� ֵ: 16λ����ֵ
*********************************************************************************************************
*/
	void HEX_to_DEC(char str[])//ʮ������תʮ���� 
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
			
			DEC_to_BIN(Sum);//����Ƕ�׵��� 
		}

	//int main()
	//{
	//	char str[100];
	//	gets(str);
	//	fun16_10(str);
	//	return 0;
	//}

		
/*
*********************************************************************************************************
*	�� �� ��: HEX_to_FLOAT
*	����˵��: ��ʮ��������ת��Ϊ������
*	��    ��: number : 16������
*	�� �� ֵ: ת����ĸ�����
*********************************************************************************************************
*/
	float HEX_to_FLOAT (unsigned int number) 
		{
				//����λ
				unsigned int sign = number >>31;
				//����
				int exponent = ((number >> 23) & 0xff) - 0x7F;
				//β��
				unsigned int mantissa = number << 9; 
				float value = 0; 
				float mantissa2;
				value = 0.5f;
				mantissa2 = 0.0f; 
				while (mantissa) 
				{
					if (mantissa & 0x80000000) mantissa2 += value; 
					mantissa <<= 1; 
					value *= 0.5f; 
				}
				value = (1.0f + mantissa2) * (pow (2, exponent)); 
				if (sign) value = -value;
				return value; 
		}


		
//�ɼ�ֵ(lv)ת4��20ma��Ӧ���㹫ʽ��
//ʵ�ʹ�����=[((ʵ�ʹ�����)�ĸ���-(ʵ�ʹ�����)�ĵ���)*(lv-4)/(20-4)]+(ʵ�ʹ�����)�ĵ��ޡ�


/************************************************************ 
 * ����:        Vol_convert_Elec(float vol)
 * ˵��:        ��ͨ����ѹת��Ϊ4-20ma���� С�����  
************************************************************/
		
	float Vol_convert_Elec(float vol)
		{
			float elec;
			elec=(((vol-1)*16)/4)+4;
			return(elec);
		}
		
		
/***********************************************************************
�������ܣ����ӻ����ּĴ�����ֵת���ɶ�ӦС��  16��������ת������
���������� HoldAddr ----�ӻ����ּĴ������յ���С����ŵ���ʼ��ַ
***********************************************************************/
	
//float  DataConvertFloat1(uint16_t *HoldReg,uint16_t HoldAddr)
//	{
//		uint8_t str[4];
//		float temp;
//		str[0] = (uint8_t )(HoldReg[HoldAddr]&0xff);//�õ����ּĴ���HoldAddr�ĵ�8λ��Ҳ����С����0~7λ
//		str[1] = (uint8_t )(HoldReg[HoldAddr]>>8);	//�õ����ּĴ���HoldAddr�ĸ�8λ��Ҳ����С����8~15λ
//		str[2] = (uint8_t )(HoldReg[HoldAddr+1]&0xff);//�õ����ּĴ���(HoldAddr+1)�ĵ�8λ��Ҳ����С����16~23λ
//		str[3] = (uint8_t )(HoldReg[HoldAddr+1]>>8);//�õ����ּĴ�(HoldAddr+1)�ĸ�8λ��Ҳ����С����24`31λ
//		temp = *((float *)str);//�õ�С����	
//		return temp;
//	}

//�˷�ʽ�������壩���ϱߵķ�ʽ��࣬���������ʽ�������õ��ı��С��ת����IEEE754�ĳ���		
float  DataConvertFloat2(uint16_t *HoldReg,uint16_t HoldAddr)
	{
		un_DtformConver  Data;
		float temp;
		Data.uc_Buf[0] = (uint8_t )(HoldReg[HoldAddr]&0xff);  //�õ����ּĴ���HoldAddr�ĵ�8λ��Ҳ����С����0~7λ
		Data.uc_Buf[1] = (uint8_t )(HoldReg[HoldAddr]>>8);	   //�õ����ּĴ���HoldAddr�ĸ�8λ��Ҳ����С����8~15λ
		Data.uc_Buf[2] = (uint8_t )(HoldReg[HoldAddr+1]&0xff);//�õ����ּĴ���(HoldAddr+1)�ĵ�8λ��Ҳ����С����16~23λ
		Data.uc_Buf[3] = (uint8_t )(HoldReg[HoldAddr+1]>>8);  //�õ����ּĴ���(HoldAddr+1)�ĸ�8λ��Ҳ����С����24~31λ
		temp = Data.ul_Temp;//�õ�С��
		return temp;
		
	}		

//  //ͬ���������巽ʽ  �����˽⼴��	
//float DataConvertFloat3(uint16_t *HoldReg,uint16_t HoldAddr)
//	{
//		un_DtformConver Data;
//		float temp;
//		Data.us_Buf[0] = HoldReg[HoldAddr];//��ȡ�ӻ����յ���С���ĵ�16λ
//		Data.us_Buf[1] = HoldReg[HoldAddr+1];//��ȡ�ӻ����յ���С���ĸ�16λ
//		temp = Data.ul_Temp;//�õ�С��
//		return temp;
//	}

// //ֱ�ӻ�ȡ��ַת���Ϳ����ˣ���ΪHoldReg[HoldAddr+1]��ŵ�������С���ĸ�16λ������stm32��С��ģʽ
//float DataConvertFloat4(uint16_t *HoldReg,uint16_t HoldAddr)
//	{
//		float temp;
//		temp = *((float *)&HoldReg[HoldAddr]);
//		return temp;
//	}
		
		
/***********************************************************************
�������ܣ���С��ת����IEEE754�������ͣ�����modbusЭ�鴫������
����������value С��ֵ    Holdaddr �ӻ����ּĴ�����ַ
***********************************************************************/		
	
//void FloatConvertuint32_t1(float value, uint16_t Holdaddr)
//	{
//		un_DtformConver Data;
//		uint16_t Reg[2];//���ּĴ��� 
//		Data.ul_Temp = value;//��ȡС��ֵ
//		Reg[Holdaddr] = Data.us_Buf[0];//��ȡ��16λ
//		Reg[Holdaddr+1] = Data.us_Buf[1];//��ȡ��16λ
//	}


	
void FloatConvertuint32_t2(float value, uint16_t Holdaddr)
	{
			un_DtformConver Data;
			uint16_t Reg[2];//���ּĴ��� 
			uint16_t us_temp = 0;
			Data.ul_Temp = value;    //��ȡС��ֵ
		
			//Э�鶨���λ��ǰ
			Reg[0] = Data.uc_Buf[0];//��ȡ16~23λ 
			us_temp = Data.uc_Buf[1];        //��ȡ24~31λ
			Reg[0] |= us_temp <<8;  //��16λ���浽���ּĴ�����
			MODS_WriteRegValue(Holdaddr+1,Reg[0]);
		
			Reg[1] = Data.uc_Buf[2];  //��ȡ���8λ
			us_temp = Data.uc_Buf[3];        //��ȡ8~15λ
			Reg[1] |= us_temp <<8;    //��16λ���浽���ּĴ�����  
			MODS_WriteRegValue(Holdaddr,Reg[1]);
	}



