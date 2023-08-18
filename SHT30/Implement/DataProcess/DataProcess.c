
#include "DataProcess.h"
#include "string.h"
#include <math.h>
#include <ctype.h>

extern uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value);


/*
*********************************************************************************************************
*	函 数 名: BEBufToUint16
*	功能说明: 将2字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint16_t BEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[0] << 8) | _pBuf[1]);
}

/*
*********************************************************************************************************
*	函 数 名: LEBufToUint16
*	功能说明: 将2字节数组(小端Little Endian，低字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*********************************************************************************************************
*/
uint16_t LEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[1] << 8) | _pBuf[0]);
}


/*
*********************************************************************************************************
*	函 数 名: BEBufToUint32
*	功能说明: 将4字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint32_t BEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[0] << 24) | ((uint32_t)_pBuf[1] << 16) | ((uint32_t)_pBuf[2] << 8) | _pBuf[3]);
}

/*
*********************************************************************************************************
*	函 数 名: LEBufToUint32
*	功能说明: 将4字节数组(小端Little Endian，低字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*********************************************************************************************************
*/
uint32_t LEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[3] << 24) | ((uint32_t)_pBuf[2] << 16) | ((uint32_t)_pBuf[1] << 8) | _pBuf[0]);
}


/*
*********************************************************************************************************
*	函 数 名: DEC_to_BIN
*	功能说明: 将十进制数转换为二进制
*	形    参: Sum : 整数
*	返 回 值: 二进制数
*********************************************************************************************************
*/
	void DEC_to_BIN(long Sum)  //十进制转二进制 
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
*	函 数 名: HEX_to_DEC
*	功能说明: 将十六进制数转换为十进制数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*********************************************************************************************************
*/
	void HEX_to_DEC(char str[])//十六进制转十进制 
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
			
			DEC_to_BIN(Sum);//函数嵌套调用 
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
*	函 数 名: HEX_to_FLOAT
*	功能说明: 将十六进制数转换为浮点数
*	形    参: number : 16进制数
*	返 回 值: 转换后的浮点数
*********************************************************************************************************
*/
	float HEX_to_FLOAT (unsigned int number) 
		{
				//符号位
				unsigned int sign = number >>31;
				//幂数
				int exponent = ((number >> 23) & 0xff) - 0x7F;
				//尾数
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


		
//采集值(lv)转4到20ma对应计算公式：
//实际工程量=[((实际工程量)的高限-(实际工程量)的低限)*(lv-4)/(20-4)]+(实际工程量)的低限。


/************************************************************ 
 * 函数:        Vol_convert_Elec(float vol)
 * 说明:        将通道电压转换为4-20ma电流 小数输出  
************************************************************/
		
	float Vol_convert_Elec(float vol)
		{
			float elec;
			elec=(((vol-1)*16)/4)+4;
			return(elec);
		}
		
		
/***********************************************************************
函数功能：将从机保持寄存器的值转换成对应小数  16进制数据转浮点数
函数参数： HoldAddr ----从机保持寄存器接收到的小数存放的起始地址
***********************************************************************/
	
//float  DataConvertFloat1(uint16_t *HoldReg,uint16_t HoldAddr)
//	{
//		uint8_t str[4];
//		float temp;
//		str[0] = (uint8_t )(HoldReg[HoldAddr]&0xff);//得到保持寄存器HoldAddr的低8位，也就是小数的0~7位
//		str[1] = (uint8_t )(HoldReg[HoldAddr]>>8);	//得到保持寄存器HoldAddr的高8位，也就是小数的8~15位
//		str[2] = (uint8_t )(HoldReg[HoldAddr+1]&0xff);//得到保持寄存器(HoldAddr+1)的低8位，也就是小数的16~23位
//		str[3] = (uint8_t )(HoldReg[HoldAddr+1]>>8);//得到保持寄存(HoldAddr+1)的高8位，也就是小数的24`31位
//		temp = *((float *)str);//得到小数，	
//		return temp;
//	}

//此方式（联合体）和上边的方式差不多，不过这个方式可以运用到的别的小数转换成IEEE754的场合		
float  DataConvertFloat2(uint16_t *HoldReg,uint16_t HoldAddr)
	{
		un_DtformConver  Data;
		float temp;
		Data.uc_Buf[0] = (uint8_t )(HoldReg[HoldAddr]&0xff);  //得到保持寄存器HoldAddr的低8位，也就是小数的0~7位
		Data.uc_Buf[1] = (uint8_t )(HoldReg[HoldAddr]>>8);	   //得到保持寄存器HoldAddr的高8位，也就是小数的8~15位
		Data.uc_Buf[2] = (uint8_t )(HoldReg[HoldAddr+1]&0xff);//得到保持寄存器(HoldAddr+1)的低8位，也就是小数的16~23位
		Data.uc_Buf[3] = (uint8_t )(HoldReg[HoldAddr+1]>>8);  //得到保持寄存器(HoldAddr+1)的高8位，也就是小数的24~31位
		temp = Data.ul_Temp;//得到小数
		return temp;
		
	}		

//  //同样是联合体方式  辅助了解即可	
//float DataConvertFloat3(uint16_t *HoldReg,uint16_t HoldAddr)
//	{
//		un_DtformConver Data;
//		float temp;
//		Data.us_Buf[0] = HoldReg[HoldAddr];//获取从机接收到的小数的低16位
//		Data.us_Buf[1] = HoldReg[HoldAddr+1];//获取从机接收到的小数的高16位
//		temp = Data.ul_Temp;//得到小数
//		return temp;
//	}

// //直接获取地址转换就可以了，因为HoldReg[HoldAddr+1]存放的正好是小数的高16位，满足stm32的小端模式
//float DataConvertFloat4(uint16_t *HoldReg,uint16_t HoldAddr)
//	{
//		float temp;
//		temp = *((float *)&HoldReg[HoldAddr]);
//		return temp;
//	}
		
		
/***********************************************************************
函数功能：将小数转换成IEEE754数据类型，用于modbus协议传输数据
函数参数：value 小数值    Holdaddr 从机保持寄存器地址
***********************************************************************/		
	
//void FloatConvertuint32_t1(float value, uint16_t Holdaddr)
//	{
//		un_DtformConver Data;
//		uint16_t Reg[2];//保持寄存器 
//		Data.ul_Temp = value;//获取小数值
//		Reg[Holdaddr] = Data.us_Buf[0];//获取低16位
//		Reg[Holdaddr+1] = Data.us_Buf[1];//获取高16位
//	}


	
void FloatConvertuint32_t2(float value, uint16_t Holdaddr)
	{
			un_DtformConver Data;
			uint16_t Reg[2];//保持寄存器 
			uint16_t us_temp = 0;
			Data.ul_Temp = value;    //获取小数值
		
			//协议定义高位在前
			Reg[0] = Data.uc_Buf[0];//获取16~23位 
			us_temp = Data.uc_Buf[1];        //获取24~31位
			Reg[0] |= us_temp <<8;  //高16位保存到保持寄存器中
			MODS_WriteRegValue(Holdaddr+1,Reg[0]);
		
			Reg[1] = Data.uc_Buf[2];  //获取最低8位
			us_temp = Data.uc_Buf[3];        //获取8~15位
			Reg[1] |= us_temp <<8;    //低16位保存到保持寄存器中  
			MODS_WriteRegValue(Holdaddr,Reg[1]);
	}



