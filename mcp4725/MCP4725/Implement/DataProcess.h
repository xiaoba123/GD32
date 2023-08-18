#ifndef _DATAPROCESS_H
#define _DATAPROCESS_H


#include <stdint.h>

typedef union                                        
{
   float ul_Temp;
   uint8_t  uc_Buf[4];//С��ת����IEEE754��ʽ������
   uint16_t us_Buf[2];//����modbusЭ��С��ת��������
}un_DtformConver;


// �����ֽ�������תΪ16λ����
uint16_t BEBufToUint16(uint8_t *_pBuf);
uint16_t LEBufToUint16(uint8_t *_pBuf);
// �����ֽ�������תΪ32λ����
uint32_t BEBufToUint32(uint8_t *_pBuf);
uint32_t LEBufToUint32(uint8_t *_pBuf);
		

void DEC_to_BIN(long Sum);
void HEX_to_DEC(char str[]);
float HEX_to_FLOAT (unsigned int number);

// IEE754����ת����ֵ
float  DataConvertFloat1(uint16_t *HoldReg,uint16_t HoldAddr);
float  DataConvertFloat2(uint16_t *HoldReg,uint16_t HoldAddr);
float  DataConvertFloat3(uint16_t *HoldReg,uint16_t HoldAddr);
float  DataConvertFloat4(uint16_t *HoldReg,uint16_t HoldAddr);

// ����ֵתIEE754����
void FloatConvertuint32_t1(float value, uint16_t Holdaddr);
void FloatConvertuint32_t2(float value, uint16_t Holdaddr);		

#endif


