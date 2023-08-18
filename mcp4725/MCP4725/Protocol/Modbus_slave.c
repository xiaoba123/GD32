/*
*********************************************************************************************************
*
*	ģ������ : MODSͨ��ģ��. ��վģʽ
*	�ļ����� : modbus_slave.c
*
*********************************************************************************************************
*/

#include "Modbus_slave.h"
#include "FLASH.h"
#include "string.h"
#include "CRC16.h" 
#include "USART1.h"
#include "USART0.h"
#include "DataProcess.h"
#include "Mod_Imple.h"

static void MODS_SendWithCRCUS1(uint8_t *_pBuf, uint8_t _ucLen);
static void MODS_SendWithCRCUS0(uint8_t *_pBuf, uint8_t _ucLen);
static void MODS_SendAckOk(void);
static void MODS_SendAckOk17(void);
static void MODS_SendAckErr(uint8_t _ucErrCode);


static void MODS_AnalyzeApp(void);

static void MODS_01H(void);
static void MODS_02H(void);
static void MODS_03H(void);
static void MODS_04H(void);
static void MODS_05H(void);
static void MODS_06H(void);
static void MODS_10H(void);

static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value);
uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value);

void MODS_ReciveNew(uint8_t _byte);

MODS_T g_tModS;
VAR_T g_tVar;


extern short usartFlag;
extern short RS485Flag;	
extern uint16_t *data;         // ��ȡ485��ַ����



/*
*********************************************************************************************************
*	�� �� ��: MODS_SendWithCRC
*	����˵��: ����һ������, �Զ�׷��2�ֽ�CRC
*	��    ��: _pBuf ���ݣ�
*			  _ucLen ���ݳ��ȣ�����CRC��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// 485����
	static void MODS_SendWithCRCUS1(uint8_t *_pBuf, uint8_t _ucLen)
	{
		uint16_t crc;
		uint8_t buf[S_TX_BUF_SIZE];
    memset(buf,0,sizeof(buf));
		memcpy(buf, _pBuf, _ucLen);
		crc = Modbus_CRC16(_pBuf, _ucLen);
		buf[_ucLen++] = crc >> 8;
		buf[_ucLen++] = crc;

		RS485_SendData(buf, _ucLen);
		

	}

// ���ڷ���	
	static void MODS_SendWithCRCUS0(uint8_t *_pBuf, uint8_t _ucLen)
	{
		uint16_t crc;
		uint8_t buf[S_TX_BUF_SIZE];
    memset(buf,0,sizeof(buf));
		memcpy(buf, _pBuf, _ucLen);
		crc = Modbus_CRC16(_pBuf, _ucLen);
		buf[_ucLen++] = crc >> 8;
		buf[_ucLen++] = crc;

		USART0_Send_Data(buf, _ucLen);
		

	}
/*
*********************************************************************************************************
*	�� �� ��: MODS_SendAckErr
*	����˵��: ���ʹ���Ӧ��
*	��    ��: _ucErrCode : �������
*	�� �� ֵ: ��
*********************************************************************************************************
*/

static void MODS_SendAckErr(uint8_t _ucErrCode)
{
	uint8_t txbuf[3];

	txbuf[0] = g_tModS.RxBuf[0];					/* 485��ַ */
	txbuf[1] = g_tModS.RxBuf[1] | 0x80;	  /* �쳣�Ĺ����� */
	txbuf[2] = _ucErrCode;							  /* �������(01,02,03,04) */

	if(usartFlag==1)
	{
			MODS_SendWithCRCUS0(txbuf,3);
	}else if(RS485Flag==1)
		{
			MODS_SendWithCRCUS1(txbuf,3);
		}	
}



/*
*********************************************************************************************************
*	�� �� ��: MODS_SendAckOk
*	����˵��: ������ȷ��Ӧ��.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

static void MODS_SendAckOk(void)
{
	uint8_t txbuf[6];
	uint8_t i;

	for (i = 0; i < 6; i++)
	{
		txbuf[i] = g_tModS.RxBuf[i];
	}
	if(usartFlag==1)
	{
			MODS_SendWithCRCUS0(txbuf,6);
	}else if(RS485Flag==1)
		{
			MODS_SendWithCRCUS1(txbuf,6);
		}	
}

static void MODS_SendAckOk17(void)
{
	uint8_t txbuf[17];
	uint8_t i;

	for (i = 0; i < 17; i++)
	{
		txbuf[i] = g_tModS.RxBuf[i];
	}
	if(usartFlag==1)
	{
			MODS_SendWithCRCUS0(txbuf,17);
	}else if(RS485Flag==1)
		{
			MODS_SendWithCRCUS1(txbuf,17);
		}	
}


/*
*********************************************************************************************************
*	�� �� ��: MODS_01H
*	����˵��: ��ȡ��Ȧ״̬����ӦԶ�̿���D01/D02/D03��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
/* ˵��:������LED����̵���,���ڹ۲����� */
static void MODS_01H(void)
{
	/*
	 ������
		��������:
			11 �ӻ���ַ
			01 ������
			00 �Ĵ�����ʼ��ַ���ֽ�
			13 �Ĵ�����ʼ��ַ���ֽ�
			00 �Ĵ����������ֽ�
			25 �Ĵ����������ֽ�
			0E CRCУ����ֽ�
			84 CRCУ����ֽ�

		�ӻ�Ӧ��: 	1����ON��0����OFF�������ص���Ȧ����Ϊ8�ı�����������������ֽ�δβʹ��0����. BIT0��Ӧ��1��
			11 �ӻ���ַ
			01 ������
			05 �����ֽ���
			CD ����1(��Ȧ0013H-��Ȧ001AH)
			6B ����2(��Ȧ001BH-��Ȧ0022H)
			B2 ����3(��Ȧ0023H-��Ȧ002AH)
			0E ����4(��Ȧ0032H-��Ȧ002BH)
			1B ����5(��Ȧ0037H-��Ȧ0033H)
			45 CRCУ����ֽ�
			E6 CRCУ����ֽ�

		����:
			01 01 10 01 00 03   29 0B	--- ��ѯD01��ʼ��3���̵���״̬
			01 01 10 03 00 01   09 0A   --- ��ѯD03�̵�����״̬
	*/
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint16_t m;
	uint8_t status[10];
	memset(g_tModS.TxBuf,0,sizeof(g_tModS.TxBuf));
	
	g_tModS.RspCode = RSP_OK;

	/* û���ⲿ�̵�����ֱ��Ӧ����� */
	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;				/* ����ֵ����� */
		return;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 			/* �Ĵ����� */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);				/* �Ĵ������� */

	m = (num + 7) / 8;
	
	if ((reg >= COIL_D01) && (num > 0) && (reg + num <= COIL_DXX + 1))
	{
		for (i = 0; i < m; i++)
		{
			status[i] = 0;
		}
		for (i = 0; i < num; i++)
		{
			if(reg<=COIL_D10)
			{
//				if (Check_pinOn1(i + 1 + reg - COIL_D01))		/* �����ŵ�״̬��д��״̬�Ĵ�����ÿһλ */
//				{  
//					status[i / 8] |= (1 << (i % 8));
//				}
		  }else if(COIL_D10<reg<=COIL_D20)
			{
//				if (Check_pinOn2(i + 1 + reg - COIL_D01))		
//				{  
//					status[i / 8] |= (1 << (i % 8));
//				}			
			}
		}
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* �Ĵ�����ַ���� */
	}

	if (g_tModS.RspCode == RSP_OK)						/* ��ȷӦ�� */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = m;			/* �����ֽ��� */

		for (i = 0; i < m; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i];	/* �̵���״̬ */
		}
			// ��ʼ����Modbus����
		if(usartFlag==1)
			{
				MODS_SendWithCRCUS0(g_tModS.TxBuf, g_tModS.TxCount);							
			}else if(RS485Flag==1)
				{
					MODS_SendWithCRCUS1(g_tModS.TxBuf, g_tModS.TxCount);							
				}	
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);				/* ��������������� */
	}
}


/*********************************************************************************************************
*	�� �� ��: MODS_02H
*	����˵��: ��ȡ����״̬����ӦK01��K03��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************/
static void MODS_02H(void)
{
	/*
		��������:
			11 �ӻ���ַ
			02 ������
			00 �Ĵ�����ַ���ֽ�
			C4 �Ĵ�����ַ���ֽ�
			00 �Ĵ����������ֽ�
			16 �Ĵ����������ֽ�
			BA CRCУ����ֽ�
			A9 CRCУ����ֽ�

		�ӻ�Ӧ��:  ��Ӧ����ɢ����Ĵ���״̬���ֱ��Ӧ�������е�ÿλֵ��1 ����ON��0 ����OFF��
		           ��һ�������ֽڵ�LSB(����ֽ�)Ϊ��ѯ��Ѱַ��ַ����������ڰ�˳���ڸ��ֽ����ɵ��ֽ�
		           ����ֽ����У�ֱ�������8λ����һ���ֽ��е�8������λҲ�Ǵӵ��ֽڵ����ֽ����С�
		           �����ص�����λ������8�ı������������������ֽ��е�ʣ��λ�����ֽڵ����λʹ��0��䡣
			11 �ӻ���ַ
			02 ������
			03 �����ֽ���
			AC ����1(00C4H-00CBH)
			DB ����2(00CCH-00D3H)
			35 ����3(00D4H-00D9H)
			20 CRCУ����ֽ�
			18 CRCУ����ֽ�

		����:
		01 02 20 01 00 08  23CC  ---- ��ȡT01-08��״̬
		01 02 20 04 00 02  B3CA  ---- ��ȡT04-05��״̬
		01 02 20 01 00 12  A207   ---- �� T01-18
	*/

	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint16_t m;
	uint8_t status[10];

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;				/* ����ֵ����� */
		return;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 			/* �Ĵ����� */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);				/* �Ĵ������� */

	m = (num + 7) / 8;
	if ((reg >= REG_T01) && (num > 0) && (reg + num <= REG_TXX + 1))
	{
		for (i = 0; i < m; i++)
		{
			status[i] = 0;
		}
		for (i = 0; i < num; i++)
		{
//			if (bsp_GetKeyState((KEY_ID_E)(KID_K1 + reg - REG_T01 + i)))
//			{
				status[i / 8] |= (1 << (i % 8));
//			}
		}
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* �Ĵ�����ַ���� */
	}

	if (g_tModS.RspCode == RSP_OK)						/* ��ȷӦ�� */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = m;			/* �����ֽ��� */

		for (i = 0; i < m; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i];	/* T01-02״̬ */
		}
			// ��ʼ����Modbus����
		if(usartFlag==1)
			{
				MODS_SendWithCRCUS0(g_tModS.TxBuf, g_tModS.TxCount);							
			}else if(RS485Flag==1)
				{
					MODS_SendWithCRCUS1(g_tModS.TxBuf, g_tModS.TxCount);							
				}	
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);				/* ��������������� */
	}
}




/*********************************************************************************************************
*	�� �� ��: MODS_ReadRegValue
*	����˵��: ��ȡ���ּĴ�����ֵ
*	��    ��: reg_addr �Ĵ�����ַ
*			      reg_value ��żĴ������
*	�� �� ֵ: 1��ʾOK 0��ʾ����
*********************************************************************************************************/
static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value)
{
	uint16_t value;
	uint16_t *temp;
	switch (reg_addr)							/* �жϼĴ�����ַ */
	{
		case SLAVE_REG_P00:
			value =	g_tVar.P00;	      /* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P01:
			temp = (uint16_t *)FMC_WRITE_START_485ADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P02:
			value =	g_tVar.P02;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P03:
			value =	g_tVar.P03;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P04:
			value =	g_tVar.P04;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P05:
			value =	g_tVar.P05;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P06:
			value =	g_tVar.P06;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P07:
			value =	g_tVar.P07;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P08:
			value =	g_tVar.P08;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P09:
			value =	g_tVar.P09;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P0A:
			value =	g_tVar.P0A;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P0B:
			value =	g_tVar.P0B;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P0C:
			value =	g_tVar.P0C;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P0D:
			value =	g_tVar.P0D;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P0E:
			value =	g_tVar.P0E;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P0F:
			temp = (uint16_t *)FMC_WRITE_START_WORKMODE;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;			
			
		
		case SLAVE_REG_P10:
			temp = (uint16_t *)FMC_WRITE_START_1_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P11:
			temp = (uint16_t *)FMC_WRITE_START_1_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P12:
			temp = (uint16_t *)FMC_WRITE_START_1_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P13:
			temp = (uint16_t *)FMC_WRITE_START_1_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P14:
			temp = (uint16_t *)FMC_WRITE_START_1_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P15:
			temp = (uint16_t *)FMC_WRITE_START_2_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P16:
			temp = (uint16_t *)FMC_WRITE_START_2_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P17:
			temp = (uint16_t *)FMC_WRITE_START_2_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P18:
			temp = (uint16_t *)FMC_WRITE_START_2_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P19:
			temp = (uint16_t *)FMC_WRITE_START_2_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;		
		
		case SLAVE_REG_P1A:
			temp = (uint16_t *)FMC_WRITE_START_3_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P1B:
			temp = (uint16_t *)FMC_WRITE_START_3_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P1C:
			temp = (uint16_t *)FMC_WRITE_START_3_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P1D:
			temp = (uint16_t *)FMC_WRITE_START_3_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P1E:
			temp = (uint16_t *)FMC_WRITE_START_3_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;	

		case SLAVE_REG_P1F:
			temp = (uint16_t *)FMC_WRITE_START_4_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;	
		
		case SLAVE_REG_P20:
			temp = (uint16_t *)FMC_WRITE_START_4_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P21:
			temp = (uint16_t *)FMC_WRITE_START_4_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P22:
			temp = (uint16_t *)FMC_WRITE_START_4_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P23:
			temp = (uint16_t *)FMC_WRITE_START_4_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P24:
			temp = (uint16_t *)FMC_WRITE_START_5_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P25:
			temp = (uint16_t *)FMC_WRITE_START_5_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P26:
			temp = (uint16_t *)FMC_WRITE_START_5_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P27:
			temp = (uint16_t *)FMC_WRITE_START_5_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P28:
			temp = (uint16_t *)FMC_WRITE_START_5_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P29:
			temp = (uint16_t *)FMC_WRITE_START_6_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;		
		
		case SLAVE_REG_P2A:
			temp = (uint16_t *)FMC_WRITE_START_6_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P2B:
			temp = (uint16_t *)FMC_WRITE_START_6_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P2C:
			temp = (uint16_t *)FMC_WRITE_START_6_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P2D:
			temp = (uint16_t *)FMC_WRITE_START_6_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P2E:
			temp = (uint16_t *)FMC_WRITE_START_7_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;	

		case SLAVE_REG_P2F:
			temp = (uint16_t *)FMC_WRITE_START_7_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;				
		
		case SLAVE_REG_P30:
			temp = (uint16_t *)FMC_WRITE_START_7_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P31:
			temp = (uint16_t *)FMC_WRITE_START_7_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P32:
			temp = (uint16_t *)FMC_WRITE_START_7_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P33:
			temp = (uint16_t *)FMC_WRITE_START_8_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P34:
			temp = (uint16_t *)FMC_WRITE_START_8_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P35:
			temp = (uint16_t *)FMC_WRITE_START_8_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P36:
			temp = (uint16_t *)FMC_WRITE_START_8_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P37:
			temp = (uint16_t *)FMC_WRITE_START_8_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P38:
			temp = (uint16_t *)FMC_WRITE_START_9_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P39:
			temp = (uint16_t *)FMC_WRITE_START_9_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;		
		
		case SLAVE_REG_P3A:
			temp = (uint16_t *)FMC_WRITE_START_9_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P3B:
			temp = (uint16_t *)FMC_WRITE_START_9_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P3C:
			temp = (uint16_t *)FMC_WRITE_START_9_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P3D:
			temp = (uint16_t *)FMC_WRITE_START_10_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P3E:
			temp = (uint16_t *)FMC_WRITE_START_10_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;	

		case SLAVE_REG_P3F:
			temp = (uint16_t *)FMC_WRITE_START_10_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;	

		case SLAVE_REG_P40:
			temp = (uint16_t *)FMC_WRITE_START_10_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P41:
			temp = (uint16_t *)FMC_WRITE_START_10_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P42:
			temp = (uint16_t *)FMC_WRITE_START_11_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P43:
			temp = (uint16_t *)FMC_WRITE_START_11_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P44:
			temp = (uint16_t *)FMC_WRITE_START_11_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P45:
			temp = (uint16_t *)FMC_WRITE_START_11_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P46:
			temp = (uint16_t *)FMC_WRITE_START_11_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P47:
			temp = (uint16_t *)FMC_WRITE_START_12_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P48:
			temp = (uint16_t *)FMC_WRITE_START_12_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P49:
			temp = (uint16_t *)FMC_WRITE_START_12_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;		
		
		case SLAVE_REG_P4A:
			temp = (uint16_t *)FMC_WRITE_START_12_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P4B:
			temp = (uint16_t *)FMC_WRITE_START_12_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P4C:
			temp = (uint16_t *)FMC_WRITE_START_13_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P4D:
			temp = (uint16_t *)FMC_WRITE_START_13_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P4E:
			temp = (uint16_t *)FMC_WRITE_START_13_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;	

		case SLAVE_REG_P4F:
			temp = (uint16_t *)FMC_WRITE_START_13_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;	
		case SLAVE_REG_P50:
			temp = (uint16_t *)FMC_WRITE_START_13_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P51:
			temp = (uint16_t *)FMC_WRITE_START_14_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P52:
			temp = (uint16_t *)FMC_WRITE_START_14_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P53:
			temp = (uint16_t *)FMC_WRITE_START_14_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P54:
			temp = (uint16_t *)FMC_WRITE_START_14_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P55:
			temp = (uint16_t *)FMC_WRITE_START_14_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P56:
			temp = (uint16_t *)FMC_WRITE_START_15_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P57:
			temp = (uint16_t *)FMC_WRITE_START_15_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P58:
			temp = (uint16_t *)FMC_WRITE_START_15_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P59:
			temp = (uint16_t *)FMC_WRITE_START_15_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;		
		
		case SLAVE_REG_P5A:
			temp = (uint16_t *)FMC_WRITE_START_15_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P5B:
			temp = (uint16_t *)FMC_WRITE_START_16_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P5C:
			temp = (uint16_t *)FMC_WRITE_START_16_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_P5D:
			temp = (uint16_t *)FMC_WRITE_START_16_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;
		
		case SLAVE_REG_P5E:
			temp = (uint16_t *)FMC_WRITE_START_16_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;	

		case SLAVE_REG_P5F:
			temp = (uint16_t *)FMC_WRITE_START_16_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;	

		case SLAVE_REG_P60:
			value =	g_tVar.P60;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P61:
			value =	g_tVar.P61;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P62:
			value =	g_tVar.P62;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P63:
			value =	g_tVar.P63;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P64:
			value =	g_tVar.P64;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P65:
			value =	g_tVar.P65;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P66:
			value =	g_tVar.P66;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P67:
			value =	g_tVar.P67;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P68:
			value =	g_tVar.P68;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P69:
			value =	g_tVar.P69;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P6A:
			value =	g_tVar.P6A;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P6B:
			value =	g_tVar.P6B;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P6C:
			value =	g_tVar.P6C;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P6D:
			value =	g_tVar.P6D;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P6E:
			value =	g_tVar.P6E;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P6F:
			value =	g_tVar.P6F;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P70:
			value =	g_tVar.P70;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P71:
			value =	g_tVar.P71;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P72:
			value =	g_tVar.P72;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P73:
			value =	g_tVar.P73;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P74:
			value =	g_tVar.P74;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P75:
			value =	g_tVar.P75;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P76:
			value =	g_tVar.P76;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P77:
			value =	g_tVar.P77;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P78:
			value =	g_tVar.P78;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P79:
			value =	g_tVar.P79;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P7A:
			value =	g_tVar.P7A;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P7B:
			value =	g_tVar.P7B;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P7C:
			value =	g_tVar.P7C;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P7D:
			value =	g_tVar.P7D;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P7E:
			value =	g_tVar.P7E;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P7F:
			value =	g_tVar.P7F;	      /* ���Ĵ���ֵ���� */
			break;


		case SLAVE_REG_P80:
			temp = (uint16_t *)FMC_WRITE_START_1_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P81:
			temp = (uint16_t *)(FMC_WRITE_START_1_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P82:
			temp = (uint16_t *)(FMC_WRITE_START_1_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P83:
			temp = (uint16_t *)(FMC_WRITE_START_1_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P84:
			temp = (uint16_t *)(FMC_WRITE_START_1_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P85:
			temp = (uint16_t *)FMC_WRITE_START_2_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P86:
			temp = (uint16_t *)(FMC_WRITE_START_2_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P87:
			temp = (uint16_t *)(FMC_WRITE_START_2_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P88:
			temp = (uint16_t *)(FMC_WRITE_START_2_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P89:
			temp = (uint16_t *)(FMC_WRITE_START_2_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;		
		
		case SLAVE_REG_P8A:
			temp = (uint16_t *)FMC_WRITE_START_3_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P8B:
			temp = (uint16_t *)(FMC_WRITE_START_3_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P8C:
			temp = (uint16_t *)(FMC_WRITE_START_3_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P8D:
			temp = (uint16_t *)(FMC_WRITE_START_3_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P8E:
			temp = (uint16_t *)(FMC_WRITE_START_3_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
			
		case SLAVE_REG_P8F:
			temp = (uint16_t *)FMC_WRITE_START_4_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P90:
			temp = (uint16_t *)(FMC_WRITE_START_4_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P91:
			temp = (uint16_t *)(FMC_WRITE_START_4_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P92:
			temp = (uint16_t *)(FMC_WRITE_START_4_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P93:
			temp = (uint16_t *)(FMC_WRITE_START_4_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P94:
			temp = (uint16_t *)FMC_WRITE_START_5_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P95:
			temp = (uint16_t *)(FMC_WRITE_START_5_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P96:
			temp = (uint16_t *)(FMC_WRITE_START_5_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P97:
			temp = (uint16_t *)(FMC_WRITE_START_5_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P98:
			temp = (uint16_t *)(FMC_WRITE_START_5_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P99:
			temp = (uint16_t *)FMC_WRITE_START_6_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P9A:
			temp = (uint16_t *)(FMC_WRITE_START_6_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P9B:
			temp = (uint16_t *)(FMC_WRITE_START_6_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P9C:
			temp = (uint16_t *)(FMC_WRITE_START_6_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P9D:
			temp = (uint16_t *)(FMC_WRITE_START_6_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_P9E:
			temp = (uint16_t *)FMC_WRITE_START_7_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_P9F:
			temp = (uint16_t *)(FMC_WRITE_START_7_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PA0:
			temp = (uint16_t *)(FMC_WRITE_START_7_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PA1:
			temp = (uint16_t *)(FMC_WRITE_START_7_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PA2:
			temp = (uint16_t *)(FMC_WRITE_START_7_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PA3:
			temp = (uint16_t *)FMC_WRITE_START_8_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PA4:
			temp = (uint16_t *)(FMC_WRITE_START_8_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PA5:
			temp = (uint16_t *)(FMC_WRITE_START_8_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PA6:
			temp = (uint16_t *)(FMC_WRITE_START_8_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PA7:
			temp = (uint16_t *)(FMC_WRITE_START_8_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PA8:
			temp = (uint16_t *)FMC_WRITE_START_9_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PA9:
			temp = (uint16_t *)(FMC_WRITE_START_9_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PAA:
			temp = (uint16_t *)(FMC_WRITE_START_9_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PAB:
			temp = (uint16_t *)(FMC_WRITE_START_9_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PAC:
			temp = (uint16_t *)(FMC_WRITE_START_9_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PAD:
			temp = (uint16_t *)FMC_WRITE_START_10_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PAE:
			temp = (uint16_t *)(FMC_WRITE_START_10_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PAF:
			temp = (uint16_t *)(FMC_WRITE_START_10_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PB0:
			temp = (uint16_t *)(FMC_WRITE_START_10_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PB1:
			temp = (uint16_t *)(FMC_WRITE_START_10_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PB2:
			temp = (uint16_t *)FMC_WRITE_START_11_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PB3:
			temp = (uint16_t *)(FMC_WRITE_START_11_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PB4:
			temp = (uint16_t *)(FMC_WRITE_START_11_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PB5:
			temp = (uint16_t *)(FMC_WRITE_START_11_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PB6:
			temp = (uint16_t *)(FMC_WRITE_START_11_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PB7:
			temp = (uint16_t *)FMC_WRITE_START_12_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PB8:
			temp = (uint16_t *)(FMC_WRITE_START_12_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PB9:
			temp = (uint16_t *)(FMC_WRITE_START_12_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PBA:
			temp = (uint16_t *)(FMC_WRITE_START_12_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PBB:
			temp = (uint16_t *)(FMC_WRITE_START_12_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PBC:
			temp = (uint16_t *)FMC_WRITE_START_13_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PBD:
			temp = (uint16_t *)(FMC_WRITE_START_13_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PBE:
			temp = (uint16_t *)(FMC_WRITE_START_13_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PBF:
			temp = (uint16_t *)(FMC_WRITE_START_13_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PC0:
			temp = (uint16_t *)(FMC_WRITE_START_13_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PC1:
			temp = (uint16_t *)FMC_WRITE_START_14_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PC2:
			temp = (uint16_t *)(FMC_WRITE_START_14_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PC3:
			temp = (uint16_t *)(FMC_WRITE_START_14_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PC4:
			temp = (uint16_t *)(FMC_WRITE_START_14_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PC5:
			temp = (uint16_t *)(FMC_WRITE_START_14_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PC6:
			temp = (uint16_t *)FMC_WRITE_START_15_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PC7:
			temp = (uint16_t *)(FMC_WRITE_START_15_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PC8:
			temp = (uint16_t *)(FMC_WRITE_START_15_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PC9:
			temp = (uint16_t *)(FMC_WRITE_START_15_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PCA:
			temp = (uint16_t *)(FMC_WRITE_START_15_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PCB:
			temp = (uint16_t *)FMC_WRITE_START_16_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PCC:
			temp = (uint16_t *)(FMC_WRITE_START_16_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PCD:
			temp = (uint16_t *)(FMC_WRITE_START_16_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;

		case SLAVE_REG_PCE:
			temp = (uint16_t *)(FMC_WRITE_START_16_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
		
		case SLAVE_REG_PCF:
			temp = (uint16_t *)(FMC_WRITE_START_16_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */   
			break;
	

		case SLAVE_REG_PD0:
			value =	g_tVar.PD0;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PD1:
			value =	g_tVar.PD1;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PD2:
			value =	g_tVar.PD2;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PD3:
			value =	g_tVar.PD3;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PD4:
			value =	g_tVar.PD4;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PD5:
			value =	g_tVar.PD5;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PD6:
			value =	g_tVar.PD6;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PD7:
			value =	g_tVar.PD7;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PD8:
			value =	g_tVar.PD8;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PD9:
			value =	g_tVar.PD9;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_PDA:
			value =	g_tVar.PDA;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PDB:
			value =	g_tVar.PDB;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PDC:
			value =	g_tVar.PDC;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PDD:
			value =	g_tVar.PDD;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PDE:
			value =	g_tVar.PDE;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_PDF:
			value =	g_tVar.PDF;	      /* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PE0:
			temp = (uint16_t *)FMC_WRITE_START_BAUD1_ADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;

		case SLAVE_REG_PE1:
			temp = (uint16_t *)FMC_WRITE_START_BAUD2_ADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* ���Ĵ���ֵ���� */ 
			break;		

		default:
			return 0;									/* �����쳣������ 0 */
	}

	reg_value[0] = value >> 8;
	reg_value[1] = value;

	return 1;											/* ��ȡ�ɹ� */
}




/*********************************************************************************************************
*	�� �� ��: MODS_WriteRegValue
*	����˵��: д�뱣�ּĴ�����ֵ
*	��    ��: reg_addr �Ĵ�����ַ
*			      reg_value �Ĵ���ֵ
*	�� �� ֵ: 1��ʾOK 0��ʾ����
*********************************************************************************************************/
uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value)
{
	switch (reg_addr)							/* �жϼĴ�����ַ */
	{	
		case SLAVE_REG_P00:
			g_tVar.P00 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P01:
			g_tVar.P01 = reg_value;				/* ��ֵд�뱣��Ĵ��� */
			break;
		
		case SLAVE_REG_P02:
			g_tVar.P02 = reg_value;				/* ��ֵд�뱣��Ĵ��� */
			break;
		
		case SLAVE_REG_P03:
			g_tVar.P03 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P04:
			g_tVar.P04 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P05:
			g_tVar.P05 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P06:
			g_tVar.P06 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P07:
			g_tVar.P07 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P08:
			g_tVar.P08 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P09:
			g_tVar.P09 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P0A:
			g_tVar.P0A = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P0B:
			g_tVar.P0B = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P0C:
			g_tVar.P0C = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P0D:
			g_tVar.P0D = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P0E:
			g_tVar.P0E = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P0F:
			g_tVar.P0F = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	
				
		case SLAVE_REG_P10:
			g_tVar.P10 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P11:
			g_tVar.P11 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P12:
			g_tVar.P12 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P13:
			g_tVar.P13 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P14:
			g_tVar.P14 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P15:
			g_tVar.P15 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P16:
			g_tVar.P16 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P17:
			g_tVar.P17 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P18:
			g_tVar.P18 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P19:
			g_tVar.P19 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P1A:
			g_tVar.P1A = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P1B:
			g_tVar.P1B = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P1C:
			g_tVar.P1C = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P1D:
			g_tVar.P1D = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P1E:
			g_tVar.P1E = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P1F:
			g_tVar.P1F = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	
		
		case SLAVE_REG_P20:
			g_tVar.P20 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P21:
			g_tVar.P21 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P22:
			g_tVar.P22 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P23:
			g_tVar.P23 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P24:
			g_tVar.P24 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P25:
			g_tVar.P25 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P26:
			g_tVar.P26 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P27:
			g_tVar.P27 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P28:
			g_tVar.P28 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P29:
			g_tVar.P29 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P2A:
			g_tVar.P2A = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P2B:
			g_tVar.P2B = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P2C:
			g_tVar.P2C = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P2D:
			g_tVar.P2D = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P2E:
			g_tVar.P2E = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P2F:
			g_tVar.P2F = reg_value;	      /* ���Ĵ���ֵ���� */
			break;			
		
		case SLAVE_REG_P30:
			g_tVar.P30 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P31:
			g_tVar.P31 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P32:
			g_tVar.P32 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P33:
			g_tVar.P33 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P34:
			g_tVar.P34 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P35:
			g_tVar.P35 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P36:
			g_tVar.P36 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P37:
			g_tVar.P37 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P38:
			g_tVar.P38 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P39:
			g_tVar.P39 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P3A:
			g_tVar.P3A = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P3B:
			g_tVar.P3B = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P3C:
			g_tVar.P3C = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P3D:
			g_tVar.P3D = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P3E:
			g_tVar.P3E = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P3F:
			g_tVar.P3F = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P40:
			g_tVar.P40 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P41:
			g_tVar.P41 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P42:
			g_tVar.P42 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P43:
			g_tVar.P43 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P44:
			g_tVar.P44 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P45:
			g_tVar.P45 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P46:
			g_tVar.P46 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P47:
			g_tVar.P47 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P48:
			g_tVar.P48 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P49:
			g_tVar.P49 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P4A:
			g_tVar.P4A = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P4B:
			g_tVar.P4B = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P4C:
			g_tVar.P4C = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P4D:
			g_tVar.P4D = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P4E:
			g_tVar.P4E = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P4F:
			g_tVar.P4F = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P50:
			g_tVar.P50 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P51:
			g_tVar.P51 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P52:
			g_tVar.P52 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P53:
			g_tVar.P53 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P54:
			g_tVar.P54 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P55:
			g_tVar.P55 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P56:
			g_tVar.P56 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P57:
			g_tVar.P57 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P58:
			g_tVar.P58 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P59:
			g_tVar.P59 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P5A:
			g_tVar.P5A = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P5B:
			g_tVar.P5B = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P5C:
			g_tVar.P5C = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P5D:
			g_tVar.P5D = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P5E:
			g_tVar.P5E = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P5F:
			g_tVar.P5F = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P60:
			g_tVar.P60 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P61:
			g_tVar.P61 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P62:
			g_tVar.P62 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P63:
			g_tVar.P63 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P64:
			g_tVar.P64 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P65:
			g_tVar.P65 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P66:
			g_tVar.P66 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P67:
			g_tVar.P67 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P68:
			g_tVar.P68 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P69:
			g_tVar.P69 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P6A:
			g_tVar.P6A = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P6B:
			g_tVar.P6B = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P6C:
			g_tVar.P6C = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P6D:
			g_tVar.P6D = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P6E:
			g_tVar.P6E = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P6F:
			g_tVar.P6F = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P70:
			g_tVar.P70 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P71:
			g_tVar.P71 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P72:
			g_tVar.P72 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P73:
			g_tVar.P73 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P74:
			g_tVar.P74 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P75:
			g_tVar.P75 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P76:
			g_tVar.P76 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P77:
			g_tVar.P77 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P78:
			g_tVar.P78 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P79:
			g_tVar.P79 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P7A:
			g_tVar.P7A = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P7B:
			g_tVar.P7B = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P7C:
			g_tVar.P7C = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P7D:
			g_tVar.P7D = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P7E:
			g_tVar.P7E = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P7F:
			g_tVar.P7F = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P80:
			g_tVar.P80 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P81:
			g_tVar.P81 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P82:
			g_tVar.P82 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P83:
			g_tVar.P83 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P84:
			g_tVar.P84 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P85:
			g_tVar.P85 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P86:
			g_tVar.P86 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P87:
			g_tVar.P87 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P88:
			g_tVar.P88 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P89:
			g_tVar.P89 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P8A:
			g_tVar.P8A = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P8B:
			g_tVar.P8B = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P8C:
			g_tVar.P8C = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P8D:
			g_tVar.P8D = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P8E:
			g_tVar.P8E = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P8F:
			g_tVar.P8F = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P90:
			g_tVar.P90 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P91:
			g_tVar.P91 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P92:
			g_tVar.P92 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P93:
			g_tVar.P93 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P94:
			g_tVar.P94 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P95:
			g_tVar.P95 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P96:
			g_tVar.P96 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P97:
			g_tVar.P97 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P98:
			g_tVar.P98 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P99:
			g_tVar.P99 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_P9A:
			g_tVar.P9A = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P9B:
			g_tVar.P9B = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P9C:
			g_tVar.P9C = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_P9D:
			g_tVar.P9D = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_P9E:
			g_tVar.P9E = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_P9F:
			g_tVar.P9F = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PA0:
			g_tVar.PA0 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PA1:
			g_tVar.PA1 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PA2:
			g_tVar.PA2 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PA3:
			g_tVar.PA3 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PA4:
			g_tVar.PA4 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PA5:
			g_tVar.PA5 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PA6:
			g_tVar.PA6 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PA7:
			g_tVar.PA7 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PA8:
			g_tVar.PA8 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PA9:
			g_tVar.PA9 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_PAA:
			g_tVar.PAA = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PAB:
			g_tVar.PAB = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PAC:
			g_tVar.PAC = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PAD:
			g_tVar.PAD = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PAE:
			g_tVar.PAE = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_PAF:
			g_tVar.PAF = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PB0:
			g_tVar.PB0 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PB1:
			g_tVar.PB1 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PB2:
			g_tVar.PB2 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PB3:
			g_tVar.PB3 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PB4:
			g_tVar.PB4 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PB5:
			g_tVar.PB5 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PB6:
			g_tVar.PB6 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PB7:
			g_tVar.PB7 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PB8:
			g_tVar.PB8 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PB9:
			g_tVar.PB9 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_PBA:
			g_tVar.PBA = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PBB:
			g_tVar.PBB = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PBC:
			g_tVar.PBC = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PBD:
			g_tVar.PBD = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PBE:
			g_tVar.PBE = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_PBF:
			g_tVar.PBF = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PC0:
			g_tVar.PC0 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PC1:
			g_tVar.PC1 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PC2:
			g_tVar.PC2 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PC3:
			g_tVar.PC3 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PC4:
			g_tVar.PC4 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PC5:
			g_tVar.PC5 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PC6:
			g_tVar.PC6 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PC7:
			g_tVar.PC7 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PC8:
			g_tVar.PC8 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PC9:
			g_tVar.PC9 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_PCA:
			g_tVar.PCA = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PCB:
			g_tVar.PCB = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PCC:
			g_tVar.PCC = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PCD:
			g_tVar.PCD = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PCE:
			g_tVar.PCE = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_PCF:
			g_tVar.PCF = reg_value;	      /* ���Ĵ���ֵ���� */
			break;
			
	
		case SLAVE_REG_PD0:
			g_tVar.PD0 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PD1:
			g_tVar.PD1 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PD2:
			g_tVar.PD2 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PD3:
			g_tVar.PD3 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PD4:
			g_tVar.PD4 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PD5:
			g_tVar.PD5 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PD6:
			g_tVar.PD6 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PD7:
			g_tVar.PD7 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PD8:
			g_tVar.PD8 = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PD9:
			g_tVar.PD9 = reg_value;	      /* ���Ĵ���ֵ���� */
			break;		
		
		case SLAVE_REG_PDA:
			g_tVar.PDA = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PDB:
			g_tVar.PDB = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PDC:
			g_tVar.PDC = reg_value;	      /* ���Ĵ���ֵ���� */
			break;

		case SLAVE_REG_PDD:
			g_tVar.PDD = reg_value;				/* ���Ĵ���ֵ���� */
			break;
		
		case SLAVE_REG_PDE:
			g_tVar.PDE = reg_value;	      /* ���Ĵ���ֵ���� */
			break;	

		case SLAVE_REG_PDF:
			g_tVar.PDF = reg_value;	      /* ���Ĵ���ֵ���� */
			break;
	
		default:
			return 0;		/* �����쳣������ 0 */
	}

	return 1;		/* ��ȡ�ɹ� */
}




/*********************************************************************************************************
*	�� �� ��: MODS_03H
*	����˵��: ��ȡ���ּĴ��� ��һ���������ּĴ�����ȡ�õ�ǰ�Ķ�����ֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************/
static void MODS_03H(void)
{
	/*
		�ӻ���ַΪ11H�����ּĴ�������ʼ��ַΪ006BH��������ַΪ006DH���ôβ�ѯ�ܹ�����3�����ּĴ�����

		��������:
			11 �ӻ���ַ
			03 ������
			00 �Ĵ�����ַ���ֽ�
			6B �Ĵ�����ַ���ֽ�
			00 �Ĵ����������ֽ�
			03 �Ĵ����������ֽ�
			76 CRC���ֽ�
			87 CRC���ֽ�

		�ӻ�Ӧ��: 	���ּĴ����ĳ���Ϊ2���ֽڡ����ڵ������ּĴ������ԣ��Ĵ������ֽ������ȱ����䣬
					���ֽ����ݺ󱻴��䡣���ּĴ���֮�䣬�͵�ַ�Ĵ����ȱ����䣬�ߵ�ַ�Ĵ����󱻴��䡣
			11 �ӻ���ַ
			03 ������
			06 �ֽ���
			00 ����1���ֽ�(006BH)
			6B ����1���ֽ�(006BH)
			00 ����2���ֽ�(006CH)
			13 ����2 ���ֽ�(006CH)
			00 ����3���ֽ�(006DH)
			00 ����3���ֽ�(006DH)
			38 CRC���ֽ�
			B9 CRC���ֽ�

		����:
			01 03 30 06 00 01  6B0B      ---- �� 3006H, ��������
			01 03 4000 0010 51C6         ---- �� 4000H ������1����ӿ��¼ 32�ֽ�
			01 03 4001 0010 0006         ---- �� 4001H ������1����ӿ��¼ 32�ֽ�

			01 03 F000 0008 770C         ---- �� F000H ������1���澯��¼ 16�ֽ�
			01 03 F001 0008 26CC         ---- �� F001H ������2���澯��¼ 16�ֽ�

			01 03 7000 0020 5ED2         ---- �� 7000H ������1�����μ�¼��1�� 64�ֽ�
			01 03 7001 0020 0F12         ---- �� 7001H ������1�����μ�¼��2�� 64�ֽ�

			01 03 7040 0020 5F06         ---- �� 7040H ������2�����μ�¼��1�� 64�ֽ�
	*/
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint8_t reg_value[160];
	memset(g_tModS.TxBuf,0,sizeof(g_tModS.TxBuf));

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)								/* 03H���������8���ֽ� */
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 				/* �Ĵ����� */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);					/* �Ĵ������� */
	if (num > sizeof(reg_value) / 2)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* ����ֵ����� */
		goto err_ret;
	}

	for (i = 0; i < num; i++)
	{
		if (MODS_ReadRegValue(reg, &reg_value[2 * i]) == 0)	/* �����Ĵ���ֵ����reg_value */
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* �Ĵ�����ַ���� */
			break;
		}
		reg++;
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)							/* ��ȷӦ�� */
	{
		g_tModS.TxCount = 0;
		if(g_tModS.RxBuf[0]==0xFF)                          // ȷ�Ϸ���485��ַ��ȡ����
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		}else
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = *data%256;
		}
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			/* �����ֽ��� */

		for (i = 0; i < num; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i];
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i+1];
		}
			// ��ʼ����Modbus����
		if(usartFlag==1)
			{
				MODS_SendWithCRCUS0(g_tModS.TxBuf, g_tModS.TxCount);							
			}else if(RS485Flag==1)
				{
					MODS_SendWithCRCUS1(g_tModS.TxBuf, g_tModS.TxCount);							
				}		/* ������ȷӦ�� */
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);					/* ���ʹ���Ӧ�� */
	}
}



/*********************************************************************************************************
*	�� �� ��: MODS_04H
*	����˵��: ��ȡ����Ĵ�������ӦA01/A02�� SMA
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************/
static void MODS_04H(void)
{
	/*
		��������:
			11 �ӻ���ַ
			04 ������
			00 �Ĵ�����ʼ��ַ���ֽ�
			08 �Ĵ�����ʼ��ַ���ֽ�
			00 �Ĵ����������ֽ�
			02 �Ĵ����������ֽ�
			F2 CRC���ֽ�
			99 CRC���ֽ�

		�ӻ�Ӧ��:  ����Ĵ�������Ϊ2���ֽڡ����ڵ�������Ĵ������ԣ��Ĵ������ֽ������ȱ����䣬
				���ֽ����ݺ󱻴��䡣����Ĵ���֮�䣬�͵�ַ�Ĵ����ȱ����䣬�ߵ�ַ�Ĵ����󱻴��䡣
			11 �ӻ���ַ
			04 ������
			04 �ֽ���
			00 ����1���ֽ�(0008H)
			0A ����1���ֽ�(0008H)
			00 ����2���ֽ�(0009H)
			0B ����2���ֽ�(0009H)
			8B CRC���ֽ�
			80 CRC���ֽ�

		����:

			01 04 2201 0006 2BB0  --- �� 2201H A01ͨ��ģ���� ��ʼ��6������
			01 04 2201 0001 6A72  --- �� 2201H

	*/
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint16_t status[10];

	memset(status, 0, 10);

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;	/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);	/* �Ĵ������� */
	
	if ((reg >= REG_A01) && (num > 0) && (reg + num <= REG_AXX + 1))
	{	
		for (i = 0; i < num; i++)
		{
			switch (reg)
			{
				/* ���Բ��� */
				case REG_A01:
					status[i] = g_tVar.A01;
					break;
					
				default:
					status[i] = 0;
					break;
			}
			reg++;
		}
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)		/* ��ȷӦ�� */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			/* �����ֽ��� */

		for (i = 0; i < num; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i] >> 8;
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i] & 0xFF;
		}
			// ��ʼ����Modbus����
		if(usartFlag==1)
			{
				MODS_SendWithCRCUS0(g_tModS.TxBuf, g_tModS.TxCount);							
			}else if(RS485Flag==1)
				{
					MODS_SendWithCRCUS1(g_tModS.TxBuf, g_tModS.TxCount);							
				}	
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
	}
}



/*********************************************************************************************************
*	�� �� ��: MODS_05H
*	����˵��: ǿ�Ƶ���Ȧ����ӦD01/D02/D03��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************/
static void MODS_05H(void)
{
	/*
		��������: д������Ȧ�Ĵ�����FF00Hֵ������Ȧ����ON״̬��0000Hֵ������Ȧ����OFF״̬
		��05Hָ�����õ�����Ȧ��״̬��15Hָ��������ö����Ȧ��״̬��
			11 �ӻ���ַ
			05 ������
			00 �Ĵ�����ַ���ֽ�
			AC �Ĵ�����ַ���ֽ�
			FF ����1���ֽ�
			00 ����2���ֽ�
			4E CRCУ����ֽ�
			8B CRCУ����ֽ�

		�ӻ�Ӧ��:
			11 �ӻ���ַ
			05 ������
			00 �Ĵ�����ַ���ֽ�
			AC �Ĵ�����ַ���ֽ�
			FF �Ĵ���1���ֽ�
			00 �Ĵ���1���ֽ�
			4E CRCУ����ֽ�
			8B CRCУ����ֽ�

		����:
		01 05 10 01 FF 00   D93A   -- D01��
		01 05 10 01 00 00   98CA   -- D01�ر�

		01 05 10 02 FF 00   293A   -- D02��
		01 05 10 02 00 00   68CA   -- D02�ر�

		01 05 10 03 FF 00   78FA   -- D03��
		01 05 10 03 00 00   390A   -- D03�ر�
	*/
	uint16_t reg;
	uint16_t value;
	memset(g_tModS.TxBuf,0,sizeof(g_tModS.TxBuf));

	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* ���� */
	
	if (value != 0x0000 && value != 0xFF00)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}
	
	switch(reg)
	{
		case COIL_D11:g_tVar.D01 = value; break;
		case COIL_D12:g_tVar.D02 = value; break;
		case COIL_D13:g_tVar.D03 = value; break;
		case COIL_D14:g_tVar.D04 = value; break;
		case COIL_D15:g_tVar.D05 = value; break;
		case COIL_D16:g_tVar.D06 = value; break;
		case COIL_D17:g_tVar.D07 = value; break;
		case COIL_D18:g_tVar.D08 = value; break;
		
		case COIL_D19:g_tVar.D09 = value; break;
		case COIL_D1A:g_tVar.D0A = value; break;
		case COIL_D1B:g_tVar.D0B = value; break;
		case COIL_D1C:g_tVar.D0C = value; break;
		case COIL_D1D:g_tVar.D0D = value; break;
		case COIL_D1E:g_tVar.D0E = value; break;
		case COIL_D1F:g_tVar.D0F = value; break;
		case COIL_D20:g_tVar.D10 = value; break;
		
		default:g_tModS.RspCode = RSP_ERR_REG_ADDR;		break;/* �Ĵ�����ַ���� */
		
	}
err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* ��ȷӦ�� */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* ��������������� */
	}
}



/*********************************************************************************************************
*	�� �� ��: MODS_06H
*	����˵��: д�����Ĵ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************/
static void MODS_06H(void)
{

	/*
		д���ּĴ�����ע��06ָ��ֻ�ܲ����������ּĴ�����16ָ��������õ����������ּĴ���

		��������:
			11 �ӻ���ַ
			06 ������
			00 �Ĵ�����ַ���ֽ�
			01 �Ĵ�����ַ���ֽ�
			00 ����1���ֽ�
			01 ����1���ֽ�
			9A CRCУ����ֽ�
			9B CRCУ����ֽ�

		�ӻ���Ӧ:
			11 �ӻ���ַ
			06 ������
			00 �Ĵ�����ַ���ֽ�
			01 �Ĵ�����ַ���ֽ�
			00 ����1���ֽ�
			01 ����1���ֽ�
			1B CRCУ����ֽ�
			5A	CRCУ����ֽ�

		����:
			01 06 30 06 00 25  A710    ---- ������������Ϊ 2.5
			01 06 30 06 00 10  6707    ---- ������������Ϊ 1.0


			01 06 30 1B 00 00  F6CD    ---- SMA �˲�ϵ�� = 0 �ر��˲�
			01 06 30 1B 00 01  370D    ---- SMA �˲�ϵ�� = 1
			01 06 30 1B 00 02  770C    ---- SMA �˲�ϵ�� = 2
			01 06 30 1B 00 05  36CE    ---- SMA �˲�ϵ�� = 5

			01 06 30 07 00 01  F6CB    ---- ����ģʽ�޸�Ϊ T1
			01 06 30 07 00 02  B6CA    ---- ����ģʽ�޸�Ϊ T2

			01 06 31 00 00 00  8736    ---- ������ӿ��¼��
			01 06 31 01 00 00  D6F6    ---- �����澯��¼��

*/

	uint16_t reg;
	uint16_t value;
	memset(g_tModS.TxBuf,0,sizeof(g_tModS.TxBuf));

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* �Ĵ���ֵ */

	if (MODS_WriteRegValue(reg, value) == 1)	/* �ú������д���ֵ����Ĵ��� */
	{
		;
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* ��ȷӦ�� */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* ��������������� */
	}
}



/*********************************************************************************************************
*	�� �� ��: MODS_10H
*	����˵��: ����д����Ĵ���.  �����ڸ�дʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************/
static void MODS_10H(void)
{
	/*
		�ӻ���ַΪ11H�����ּĴ�������ʵ��ַΪ0001H���Ĵ����Ľ�����ַΪ0002H���ܹ�����2���Ĵ�����
		���ּĴ���0001H������Ϊ000AH�����ּĴ���0002H������Ϊ0102H��

		��������:
			11 �ӻ���ַ
			10 ������
			00 �Ĵ�����ʼ��ַ���ֽ�
			01 �Ĵ�����ʼ��ַ���ֽ�
			00 �Ĵ����������ֽ�
			02 �Ĵ����������ֽ�
			04 �ֽ���
			00 ����1���ֽ�
			0A ����1���ֽ�
			01 ����2���ֽ�
			02 ����2���ֽ�
			C6 CRCУ����ֽ�
			F0 CRCУ����ֽ�

		�ӻ���Ӧ:
			11 �ӻ���ַ
			06 ������
			00 �Ĵ�����ַ���ֽ�
			01 �Ĵ�����ַ���ֽ�
			00 ����1���ֽ�
			01 ����1���ֽ�
			1B CRCУ����ֽ�
			5A	CRCУ����ֽ�

		����:
			01 10 30 00 00 06 0C  07 DE  00 0A  00 01  00 08  00 0C  00 00     389A    ---- дʱ�� 2014-10-01 08:12:00
			01 10 30 00 00 06 0C  07 DF  00 01  00 1F  00 17  00 3B  00 39     5549    ---- дʱ�� 2015-01-31 23:59:57

	*/
	uint16_t reg_addr;
	uint16_t reg_num;
	uint8_t byte_num;
	uint8_t i;
	uint16_t value;
	memset(g_tModS.TxBuf,0,sizeof(g_tModS.TxBuf));
	
	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount < 11)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;			/* ����ֵ����� */
		goto err_ret;
	}

	reg_addr = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	reg_num = BEBufToUint16(&g_tModS.RxBuf[4]);		/* �Ĵ������� */
	byte_num = g_tModS.RxBuf[6];					/* ������������ֽ��� */

	if (byte_num != 2 * reg_num)
	{
		;
	}
	
	for (i = 0; i < reg_num; i++)
	{
		value = BEBufToUint16(&g_tModS.RxBuf[7 + 2 * i]);	/* �Ĵ���ֵ */

		if (MODS_WriteRegValue(reg_addr + i, value) == 1)
		{
			;
		}
		else
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
			break;
		}
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)					/* ��ȷӦ�� */
	{
		MODS_SendAckOk17();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);			/* ��������������� */
	}
}


/*
*********************************************************************************************************
*	�� �� ��: MODS_AnalyzeApp
*	����˵��: ����Ӧ�ò�Э��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
		switch (g_tModS.RxBuf[1])				/* ��2���ֽ� ������ */
		{
			case 0x01:							/* ��ȡ��Ȧ״̬����һЩλ״̬��*/
				MODS_01H();
				break;

			case 0x02:							/* ��ȡ����״̬������״̬��*/
				MODS_02H();		
				break;
			
			case 0x03:							/* ��ȡ���ּĴ����������̴���g_tVar�У�*/
				MODS_03H();				
				break;
			
			case 0x04:							/* ��ȡ����Ĵ�����ADC��ֵ��*/
				MODS_04H();				
				break;
			
			case 0x05:							/* ǿ�Ƶ���Ȧ������DO��*/
				MODS_05H();				
				break;
			
			case 0x06:							/* д��������Ĵ����������̸�дg_tVar�еĲ�����*/
				MODS_06H();					
				break;
				
			case 0x10:							/* д�������Ĵ����������̴���g_tVar�еĲ�����*/
				MODS_10H();				
				break;
			
			default:
				g_tModS.RspCode = RSP_ERR_CMD;
				MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
				break;
		}	
}


/*
*********************************************************************************************************
*	�� �� ��: MODS_Poll
*	����˵��: �������ݰ�. �����������������á�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
	void MODS_Poll(void)
		{
			uint8_t addr;
			/* �յ����ݰ���1�ֽ� վ��ַ */ 
			addr = g_tModS.RxBuf[0];						
			/* û���յ����ݰ� �������� */
			if(g_tModS.RxStatus == 0)  
			{
				 return;
			}
			/* ���յ�������С��4���ֽھ���Ϊ���� */
			if(g_tModS.RxCount < 4)				
			{
				// �˴������ڴ�ӡ�����ֽ������� ����
			}
			/* Length and CRC check */
			if(( g_tModS.RxCount >= 4 ) && ( Modbus_CRC16((unsigned char*) g_tModS.RxBuf, g_tModS.RxCount ) == 0 ) ) 
				// �жϽ��յ������ݰ��ĳ����ǲ��Ǵ���modbus���ݰ�����С����4	
			  // ��һ��˼·����֮ǰ������ǰѳ��������λУ��λ֮������ݽ��м���crcУ���ֵ�ͽ��յ�����λУ��ֵ���бȽϣ�
			  // ʵ���Ͽ���ֱ�Ӱѽ��յ������������У��ֵ����������0�Ļ�˵��У��ͨ�� 
			 {	
				if(addr == 0xFF|| addr == *data%256)  // ����ַ�Ƿ����Լ��ĵ�ַ �� �Ϳ�ʼ����������
				 {	
					/* ����Ӧ�ò�Э��  Modbus�¼������� */

					Modbus_05_func();   // 05 д��Ȧ
					Modbus_06_func();   // 06 д�Ĵ���
					Modbus_10_func();   // 10 д����Ĵ���
					 				 
					MODS_AnalyzeApp();	// 01 03 ��ѯ����ؼ����б�����Ӧ

				 }else if(g_tModS.RxBuf[0] == 0) 
				 {
						// �㲥��ַ ���ڶ�Ӧ�Ĺ����봦�������� û�е����ó���
				 }
			 }
			g_tModS.RxCount = 0;					// ��������������������´�֡ͬ�� 
			g_tModS.RxStatus = 0;         // �ָ�����״̬
			usartFlag=0;
			RS485Flag=0;
		}

/*****************************  *********************************/
