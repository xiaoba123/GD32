#ifndef _DATAPROCESS_H
#define _DATAPROCESS_H


#include <stdint.h>

typedef union                                        
{
   float ul_Temp;
   uint8_t  uc_Buf[4];//小数转换成IEEE754格式的数组
   uint16_t us_Buf[2];//用于modbus协议小数转换的数组
}un_DtformConver;


// 将两字节数数组转为16位整型
uint16_t BEBufToUint16(uint8_t *_pBuf);
uint16_t LEBufToUint16(uint8_t *_pBuf);
// 将两字节数数组转为32位整型
uint32_t BEBufToUint32(uint8_t *_pBuf);
uint32_t LEBufToUint32(uint8_t *_pBuf);
		

void DEC_to_BIN(long Sum);
void HEX_to_DEC(char str[]);
float HEX_to_FLOAT (unsigned int number);

// IEE754数据转浮点值
float  DataConvertFloat1(uint16_t *HoldReg,uint16_t HoldAddr);
float  DataConvertFloat2(uint16_t *HoldReg,uint16_t HoldAddr);
float  DataConvertFloat3(uint16_t *HoldReg,uint16_t HoldAddr);
float  DataConvertFloat4(uint16_t *HoldReg,uint16_t HoldAddr);

// 浮点值转IEE754整型
void FloatConvertuint32_t1(float value, uint16_t Holdaddr);
void FloatConvertuint32_t2(float value, uint16_t Holdaddr);		

#endif


