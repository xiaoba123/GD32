/*
*********************************************************************************************************
*
*	模块名称 : MODS通信模块. 从站模式
*	文件名称 : modbus_slave.c
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
extern uint16_t *data;         // 读取485地址数据



/*
*********************************************************************************************************
*	函 数 名: MODS_SendWithCRC
*	功能说明: 发送一串数据, 自动追加2字节CRC
*	形    参: _pBuf 数据；
*			  _ucLen 数据长度（不带CRC）
*	返 回 值: 无
*********************************************************************************************************
*/
// 485发送
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

// 网口发送	
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
*	函 数 名: MODS_SendAckErr
*	功能说明: 发送错误应答
*	形    参: _ucErrCode : 错误代码
*	返 回 值: 无
*********************************************************************************************************
*/

static void MODS_SendAckErr(uint8_t _ucErrCode)
{
	uint8_t txbuf[3];

	txbuf[0] = g_tModS.RxBuf[0];					/* 485地址 */
	txbuf[1] = g_tModS.RxBuf[1] | 0x80;	  /* 异常的功能码 */
	txbuf[2] = _ucErrCode;							  /* 错误代码(01,02,03,04) */

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
*	函 数 名: MODS_SendAckOk
*	功能说明: 发送正确的应答.
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: MODS_01H
*	功能说明: 读取线圈状态（对应远程开关D01/D02/D03）
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
/* 说明:这里用LED代替继电器,便于观察现象 */
static void MODS_01H(void)
{
	/*
	 举例：
		主机发送:
			11 从机地址
			01 功能码
			00 寄存器起始地址高字节
			13 寄存器起始地址低字节
			00 寄存器数量高字节
			25 寄存器数量低字节
			0E CRC校验高字节
			84 CRC校验低字节

		从机应答: 	1代表ON，0代表OFF。若返回的线圈数不为8的倍数，则在最后数据字节未尾使用0代替. BIT0对应第1个
			11 从机地址
			01 功能码
			05 返回字节数
			CD 数据1(线圈0013H-线圈001AH)
			6B 数据2(线圈001BH-线圈0022H)
			B2 数据3(线圈0023H-线圈002AH)
			0E 数据4(线圈0032H-线圈002BH)
			1B 数据5(线圈0037H-线圈0033H)
			45 CRC校验高字节
			E6 CRC校验低字节

		例子:
			01 01 10 01 00 03   29 0B	--- 查询D01开始的3个继电器状态
			01 01 10 03 00 01   09 0A   --- 查询D03继电器的状态
	*/
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint16_t m;
	uint8_t status[10];
	memset(g_tModS.TxBuf,0,sizeof(g_tModS.TxBuf));
	
	g_tModS.RspCode = RSP_OK;

	/* 没有外部继电器，直接应答错误 */
	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;				/* 数据值域错误 */
		return;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 			/* 寄存器号 */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);				/* 寄存器个数 */

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
//				if (Check_pinOn1(i + 1 + reg - COIL_D01))		/* 读引脚的状态，写入状态寄存器的每一位 */
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
		g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* 寄存器地址错误 */
	}

	if (g_tModS.RspCode == RSP_OK)						/* 正确应答 */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = m;			/* 返回字节数 */

		for (i = 0; i < m; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i];	/* 继电器状态 */
		}
			// 开始返回Modbus数据
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
		MODS_SendAckErr(g_tModS.RspCode);				/* 告诉主机命令错误 */
	}
}


/*********************************************************************************************************
*	函 数 名: MODS_02H
*	功能说明: 读取输入状态（对应K01～K03）
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************/
static void MODS_02H(void)
{
	/*
		主机发送:
			11 从机地址
			02 功能码
			00 寄存器地址高字节
			C4 寄存器地址低字节
			00 寄存器数量高字节
			16 寄存器数量低字节
			BA CRC校验高字节
			A9 CRC校验低字节

		从机应答:  响应各离散输入寄存器状态，分别对应数据区中的每位值，1 代表ON；0 代表OFF。
		           第一个数据字节的LSB(最低字节)为查询的寻址地址，其他输入口按顺序在该字节中由低字节
		           向高字节排列，直到填充满8位。下一个字节中的8个输入位也是从低字节到高字节排列。
		           若返回的输入位数不是8的倍数，则在最后的数据字节中的剩余位至该字节的最高位使用0填充。
			11 从机地址
			02 功能码
			03 返回字节数
			AC 数据1(00C4H-00CBH)
			DB 数据2(00CCH-00D3H)
			35 数据3(00D4H-00D9H)
			20 CRC校验高字节
			18 CRC校验低字节

		例子:
		01 02 20 01 00 08  23CC  ---- 读取T01-08的状态
		01 02 20 04 00 02  B3CA  ---- 读取T04-05的状态
		01 02 20 01 00 12  A207   ---- 读 T01-18
	*/

	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint16_t m;
	uint8_t status[10];

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;				/* 数据值域错误 */
		return;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 			/* 寄存器号 */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);				/* 寄存器个数 */

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
		g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* 寄存器地址错误 */
	}

	if (g_tModS.RspCode == RSP_OK)						/* 正确应答 */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = m;			/* 返回字节数 */

		for (i = 0; i < m; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i];	/* T01-02状态 */
		}
			// 开始返回Modbus数据
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
		MODS_SendAckErr(g_tModS.RspCode);				/* 告诉主机命令错误 */
	}
}




/*********************************************************************************************************
*	函 数 名: MODS_ReadRegValue
*	功能说明: 读取保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			      reg_value 存放寄存器结果
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************/
static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value)
{
	uint16_t value;
	uint16_t *temp;
	switch (reg_addr)							/* 判断寄存器地址 */
	{
		case SLAVE_REG_P00:
			value =	g_tVar.P00;	      /* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P01:
			temp = (uint16_t *)FMC_WRITE_START_485ADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P02:
			value =	g_tVar.P02;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P03:
			value =	g_tVar.P03;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P04:
			value =	g_tVar.P04;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P05:
			value =	g_tVar.P05;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P06:
			value =	g_tVar.P06;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P07:
			value =	g_tVar.P07;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P08:
			value =	g_tVar.P08;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P09:
			value =	g_tVar.P09;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P0A:
			value =	g_tVar.P0A;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P0B:
			value =	g_tVar.P0B;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P0C:
			value =	g_tVar.P0C;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P0D:
			value =	g_tVar.P0D;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P0E:
			value =	g_tVar.P0E;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P0F:
			temp = (uint16_t *)FMC_WRITE_START_WORKMODE;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;			
			
		
		case SLAVE_REG_P10:
			temp = (uint16_t *)FMC_WRITE_START_1_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P11:
			temp = (uint16_t *)FMC_WRITE_START_1_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P12:
			temp = (uint16_t *)FMC_WRITE_START_1_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P13:
			temp = (uint16_t *)FMC_WRITE_START_1_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P14:
			temp = (uint16_t *)FMC_WRITE_START_1_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P15:
			temp = (uint16_t *)FMC_WRITE_START_2_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P16:
			temp = (uint16_t *)FMC_WRITE_START_2_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P17:
			temp = (uint16_t *)FMC_WRITE_START_2_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P18:
			temp = (uint16_t *)FMC_WRITE_START_2_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P19:
			temp = (uint16_t *)FMC_WRITE_START_2_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;		
		
		case SLAVE_REG_P1A:
			temp = (uint16_t *)FMC_WRITE_START_3_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P1B:
			temp = (uint16_t *)FMC_WRITE_START_3_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P1C:
			temp = (uint16_t *)FMC_WRITE_START_3_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P1D:
			temp = (uint16_t *)FMC_WRITE_START_3_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P1E:
			temp = (uint16_t *)FMC_WRITE_START_3_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;	

		case SLAVE_REG_P1F:
			temp = (uint16_t *)FMC_WRITE_START_4_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;	
		
		case SLAVE_REG_P20:
			temp = (uint16_t *)FMC_WRITE_START_4_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P21:
			temp = (uint16_t *)FMC_WRITE_START_4_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P22:
			temp = (uint16_t *)FMC_WRITE_START_4_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P23:
			temp = (uint16_t *)FMC_WRITE_START_4_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P24:
			temp = (uint16_t *)FMC_WRITE_START_5_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P25:
			temp = (uint16_t *)FMC_WRITE_START_5_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P26:
			temp = (uint16_t *)FMC_WRITE_START_5_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P27:
			temp = (uint16_t *)FMC_WRITE_START_5_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P28:
			temp = (uint16_t *)FMC_WRITE_START_5_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P29:
			temp = (uint16_t *)FMC_WRITE_START_6_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;		
		
		case SLAVE_REG_P2A:
			temp = (uint16_t *)FMC_WRITE_START_6_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P2B:
			temp = (uint16_t *)FMC_WRITE_START_6_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P2C:
			temp = (uint16_t *)FMC_WRITE_START_6_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P2D:
			temp = (uint16_t *)FMC_WRITE_START_6_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P2E:
			temp = (uint16_t *)FMC_WRITE_START_7_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;	

		case SLAVE_REG_P2F:
			temp = (uint16_t *)FMC_WRITE_START_7_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;				
		
		case SLAVE_REG_P30:
			temp = (uint16_t *)FMC_WRITE_START_7_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P31:
			temp = (uint16_t *)FMC_WRITE_START_7_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P32:
			temp = (uint16_t *)FMC_WRITE_START_7_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P33:
			temp = (uint16_t *)FMC_WRITE_START_8_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P34:
			temp = (uint16_t *)FMC_WRITE_START_8_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P35:
			temp = (uint16_t *)FMC_WRITE_START_8_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P36:
			temp = (uint16_t *)FMC_WRITE_START_8_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P37:
			temp = (uint16_t *)FMC_WRITE_START_8_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P38:
			temp = (uint16_t *)FMC_WRITE_START_9_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P39:
			temp = (uint16_t *)FMC_WRITE_START_9_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;		
		
		case SLAVE_REG_P3A:
			temp = (uint16_t *)FMC_WRITE_START_9_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P3B:
			temp = (uint16_t *)FMC_WRITE_START_9_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P3C:
			temp = (uint16_t *)FMC_WRITE_START_9_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P3D:
			temp = (uint16_t *)FMC_WRITE_START_10_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P3E:
			temp = (uint16_t *)FMC_WRITE_START_10_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;	

		case SLAVE_REG_P3F:
			temp = (uint16_t *)FMC_WRITE_START_10_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;	

		case SLAVE_REG_P40:
			temp = (uint16_t *)FMC_WRITE_START_10_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P41:
			temp = (uint16_t *)FMC_WRITE_START_10_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P42:
			temp = (uint16_t *)FMC_WRITE_START_11_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P43:
			temp = (uint16_t *)FMC_WRITE_START_11_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P44:
			temp = (uint16_t *)FMC_WRITE_START_11_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P45:
			temp = (uint16_t *)FMC_WRITE_START_11_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P46:
			temp = (uint16_t *)FMC_WRITE_START_11_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P47:
			temp = (uint16_t *)FMC_WRITE_START_12_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P48:
			temp = (uint16_t *)FMC_WRITE_START_12_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P49:
			temp = (uint16_t *)FMC_WRITE_START_12_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;		
		
		case SLAVE_REG_P4A:
			temp = (uint16_t *)FMC_WRITE_START_12_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P4B:
			temp = (uint16_t *)FMC_WRITE_START_12_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P4C:
			temp = (uint16_t *)FMC_WRITE_START_13_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P4D:
			temp = (uint16_t *)FMC_WRITE_START_13_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P4E:
			temp = (uint16_t *)FMC_WRITE_START_13_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;	

		case SLAVE_REG_P4F:
			temp = (uint16_t *)FMC_WRITE_START_13_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;	
		case SLAVE_REG_P50:
			temp = (uint16_t *)FMC_WRITE_START_13_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P51:
			temp = (uint16_t *)FMC_WRITE_START_14_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P52:
			temp = (uint16_t *)FMC_WRITE_START_14_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P53:
			temp = (uint16_t *)FMC_WRITE_START_14_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P54:
			temp = (uint16_t *)FMC_WRITE_START_14_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P55:
			temp = (uint16_t *)FMC_WRITE_START_14_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P56:
			temp = (uint16_t *)FMC_WRITE_START_15_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P57:
			temp = (uint16_t *)FMC_WRITE_START_15_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P58:
			temp = (uint16_t *)FMC_WRITE_START_15_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P59:
			temp = (uint16_t *)FMC_WRITE_START_15_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;		
		
		case SLAVE_REG_P5A:
			temp = (uint16_t *)FMC_WRITE_START_15_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P5B:
			temp = (uint16_t *)FMC_WRITE_START_16_lxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P5C:
			temp = (uint16_t *)FMC_WRITE_START_16_lcsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_P5D:
			temp = (uint16_t *)FMC_WRITE_START_16_lcxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;
		
		case SLAVE_REG_P5E:
			temp = (uint16_t *)FMC_WRITE_START_16_yzsxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;	

		case SLAVE_REG_P5F:
			temp = (uint16_t *)FMC_WRITE_START_16_yzxxADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;	

		case SLAVE_REG_P60:
			value =	g_tVar.P60;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P61:
			value =	g_tVar.P61;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P62:
			value =	g_tVar.P62;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P63:
			value =	g_tVar.P63;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P64:
			value =	g_tVar.P64;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P65:
			value =	g_tVar.P65;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P66:
			value =	g_tVar.P66;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P67:
			value =	g_tVar.P67;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P68:
			value =	g_tVar.P68;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P69:
			value =	g_tVar.P69;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P6A:
			value =	g_tVar.P6A;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P6B:
			value =	g_tVar.P6B;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P6C:
			value =	g_tVar.P6C;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P6D:
			value =	g_tVar.P6D;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P6E:
			value =	g_tVar.P6E;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P6F:
			value =	g_tVar.P6F;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P70:
			value =	g_tVar.P70;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P71:
			value =	g_tVar.P71;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P72:
			value =	g_tVar.P72;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P73:
			value =	g_tVar.P73;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P74:
			value =	g_tVar.P74;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P75:
			value =	g_tVar.P75;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P76:
			value =	g_tVar.P76;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P77:
			value =	g_tVar.P77;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P78:
			value =	g_tVar.P78;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P79:
			value =	g_tVar.P79;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P7A:
			value =	g_tVar.P7A;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P7B:
			value =	g_tVar.P7B;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P7C:
			value =	g_tVar.P7C;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P7D:
			value =	g_tVar.P7D;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P7E:
			value =	g_tVar.P7E;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P7F:
			value =	g_tVar.P7F;	      /* 将寄存器值读出 */
			break;


		case SLAVE_REG_P80:
			temp = (uint16_t *)FMC_WRITE_START_1_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P81:
			temp = (uint16_t *)(FMC_WRITE_START_1_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P82:
			temp = (uint16_t *)(FMC_WRITE_START_1_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P83:
			temp = (uint16_t *)(FMC_WRITE_START_1_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P84:
			temp = (uint16_t *)(FMC_WRITE_START_1_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P85:
			temp = (uint16_t *)FMC_WRITE_START_2_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P86:
			temp = (uint16_t *)(FMC_WRITE_START_2_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P87:
			temp = (uint16_t *)(FMC_WRITE_START_2_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P88:
			temp = (uint16_t *)(FMC_WRITE_START_2_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P89:
			temp = (uint16_t *)(FMC_WRITE_START_2_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;		
		
		case SLAVE_REG_P8A:
			temp = (uint16_t *)FMC_WRITE_START_3_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P8B:
			temp = (uint16_t *)(FMC_WRITE_START_3_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P8C:
			temp = (uint16_t *)(FMC_WRITE_START_3_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P8D:
			temp = (uint16_t *)(FMC_WRITE_START_3_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P8E:
			temp = (uint16_t *)(FMC_WRITE_START_3_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
			
		case SLAVE_REG_P8F:
			temp = (uint16_t *)FMC_WRITE_START_4_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P90:
			temp = (uint16_t *)(FMC_WRITE_START_4_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P91:
			temp = (uint16_t *)(FMC_WRITE_START_4_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P92:
			temp = (uint16_t *)(FMC_WRITE_START_4_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P93:
			temp = (uint16_t *)(FMC_WRITE_START_4_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P94:
			temp = (uint16_t *)FMC_WRITE_START_5_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P95:
			temp = (uint16_t *)(FMC_WRITE_START_5_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P96:
			temp = (uint16_t *)(FMC_WRITE_START_5_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P97:
			temp = (uint16_t *)(FMC_WRITE_START_5_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P98:
			temp = (uint16_t *)(FMC_WRITE_START_5_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P99:
			temp = (uint16_t *)FMC_WRITE_START_6_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P9A:
			temp = (uint16_t *)(FMC_WRITE_START_6_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P9B:
			temp = (uint16_t *)(FMC_WRITE_START_6_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P9C:
			temp = (uint16_t *)(FMC_WRITE_START_6_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P9D:
			temp = (uint16_t *)(FMC_WRITE_START_6_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_P9E:
			temp = (uint16_t *)FMC_WRITE_START_7_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_P9F:
			temp = (uint16_t *)(FMC_WRITE_START_7_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PA0:
			temp = (uint16_t *)(FMC_WRITE_START_7_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PA1:
			temp = (uint16_t *)(FMC_WRITE_START_7_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PA2:
			temp = (uint16_t *)(FMC_WRITE_START_7_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PA3:
			temp = (uint16_t *)FMC_WRITE_START_8_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PA4:
			temp = (uint16_t *)(FMC_WRITE_START_8_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PA5:
			temp = (uint16_t *)(FMC_WRITE_START_8_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PA6:
			temp = (uint16_t *)(FMC_WRITE_START_8_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PA7:
			temp = (uint16_t *)(FMC_WRITE_START_8_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PA8:
			temp = (uint16_t *)FMC_WRITE_START_9_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PA9:
			temp = (uint16_t *)(FMC_WRITE_START_9_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PAA:
			temp = (uint16_t *)(FMC_WRITE_START_9_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PAB:
			temp = (uint16_t *)(FMC_WRITE_START_9_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PAC:
			temp = (uint16_t *)(FMC_WRITE_START_9_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PAD:
			temp = (uint16_t *)FMC_WRITE_START_10_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PAE:
			temp = (uint16_t *)(FMC_WRITE_START_10_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PAF:
			temp = (uint16_t *)(FMC_WRITE_START_10_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PB0:
			temp = (uint16_t *)(FMC_WRITE_START_10_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PB1:
			temp = (uint16_t *)(FMC_WRITE_START_10_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PB2:
			temp = (uint16_t *)FMC_WRITE_START_11_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PB3:
			temp = (uint16_t *)(FMC_WRITE_START_11_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PB4:
			temp = (uint16_t *)(FMC_WRITE_START_11_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PB5:
			temp = (uint16_t *)(FMC_WRITE_START_11_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PB6:
			temp = (uint16_t *)(FMC_WRITE_START_11_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PB7:
			temp = (uint16_t *)FMC_WRITE_START_12_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PB8:
			temp = (uint16_t *)(FMC_WRITE_START_12_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PB9:
			temp = (uint16_t *)(FMC_WRITE_START_12_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PBA:
			temp = (uint16_t *)(FMC_WRITE_START_12_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PBB:
			temp = (uint16_t *)(FMC_WRITE_START_12_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PBC:
			temp = (uint16_t *)FMC_WRITE_START_13_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PBD:
			temp = (uint16_t *)(FMC_WRITE_START_13_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PBE:
			temp = (uint16_t *)(FMC_WRITE_START_13_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PBF:
			temp = (uint16_t *)(FMC_WRITE_START_13_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PC0:
			temp = (uint16_t *)(FMC_WRITE_START_13_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PC1:
			temp = (uint16_t *)FMC_WRITE_START_14_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PC2:
			temp = (uint16_t *)(FMC_WRITE_START_14_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PC3:
			temp = (uint16_t *)(FMC_WRITE_START_14_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PC4:
			temp = (uint16_t *)(FMC_WRITE_START_14_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PC5:
			temp = (uint16_t *)(FMC_WRITE_START_14_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PC6:
			temp = (uint16_t *)FMC_WRITE_START_15_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PC7:
			temp = (uint16_t *)(FMC_WRITE_START_15_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PC8:
			temp = (uint16_t *)(FMC_WRITE_START_15_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PC9:
			temp = (uint16_t *)(FMC_WRITE_START_15_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PCA:
			temp = (uint16_t *)(FMC_WRITE_START_15_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PCB:
			temp = (uint16_t *)FMC_WRITE_START_16_DOADDR;
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PCC:
			temp = (uint16_t *)(FMC_WRITE_START_16_DOADDR+4);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PCD:
			temp = (uint16_t *)(FMC_WRITE_START_16_DOADDR+8);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;

		case SLAVE_REG_PCE:
			temp = (uint16_t *)(FMC_WRITE_START_16_DOADDR+12);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
		
		case SLAVE_REG_PCF:
			temp = (uint16_t *)(FMC_WRITE_START_16_DOADDR+16);
			if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */   
			break;
	

		case SLAVE_REG_PD0:
			value =	g_tVar.PD0;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PD1:
			value =	g_tVar.PD1;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PD2:
			value =	g_tVar.PD2;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PD3:
			value =	g_tVar.PD3;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PD4:
			value =	g_tVar.PD4;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PD5:
			value =	g_tVar.PD5;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PD6:
			value =	g_tVar.PD6;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PD7:
			value =	g_tVar.PD7;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PD8:
			value =	g_tVar.PD8;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PD9:
			value =	g_tVar.PD9;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_PDA:
			value =	g_tVar.PDA;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PDB:
			value =	g_tVar.PDB;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PDC:
			value =	g_tVar.PDC;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PDD:
			value =	g_tVar.PDD;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PDE:
			value =	g_tVar.PDE;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_PDF:
			value =	g_tVar.PDF;	      /* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PE0:
			temp = (uint16_t *)FMC_WRITE_START_BAUD1_ADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;

		case SLAVE_REG_PE1:
			temp = (uint16_t *)FMC_WRITE_START_BAUD2_ADDR;
      if(*temp==0xFFFF){value = 0x0000;}else{value =	*temp;}/* 将寄存器值读出 */ 
			break;		

		default:
			return 0;									/* 参数异常，返回 0 */
	}

	reg_value[0] = value >> 8;
	reg_value[1] = value;

	return 1;											/* 读取成功 */
}




/*********************************************************************************************************
*	函 数 名: MODS_WriteRegValue
*	功能说明: 写入保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			      reg_value 寄存器值
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************/
uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value)
{
	switch (reg_addr)							/* 判断寄存器地址 */
	{	
		case SLAVE_REG_P00:
			g_tVar.P00 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P01:
			g_tVar.P01 = reg_value;				/* 将值写入保存寄存器 */
			break;
		
		case SLAVE_REG_P02:
			g_tVar.P02 = reg_value;				/* 将值写入保存寄存器 */
			break;
		
		case SLAVE_REG_P03:
			g_tVar.P03 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P04:
			g_tVar.P04 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P05:
			g_tVar.P05 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P06:
			g_tVar.P06 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P07:
			g_tVar.P07 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P08:
			g_tVar.P08 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P09:
			g_tVar.P09 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P0A:
			g_tVar.P0A = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P0B:
			g_tVar.P0B = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P0C:
			g_tVar.P0C = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P0D:
			g_tVar.P0D = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P0E:
			g_tVar.P0E = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P0F:
			g_tVar.P0F = reg_value;	      /* 将寄存器值读出 */
			break;	
				
		case SLAVE_REG_P10:
			g_tVar.P10 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P11:
			g_tVar.P11 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P12:
			g_tVar.P12 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P13:
			g_tVar.P13 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P14:
			g_tVar.P14 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P15:
			g_tVar.P15 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P16:
			g_tVar.P16 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P17:
			g_tVar.P17 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P18:
			g_tVar.P18 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P19:
			g_tVar.P19 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P1A:
			g_tVar.P1A = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P1B:
			g_tVar.P1B = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P1C:
			g_tVar.P1C = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P1D:
			g_tVar.P1D = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P1E:
			g_tVar.P1E = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P1F:
			g_tVar.P1F = reg_value;	      /* 将寄存器值读出 */
			break;	
		
		case SLAVE_REG_P20:
			g_tVar.P20 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P21:
			g_tVar.P21 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P22:
			g_tVar.P22 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P23:
			g_tVar.P23 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P24:
			g_tVar.P24 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P25:
			g_tVar.P25 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P26:
			g_tVar.P26 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P27:
			g_tVar.P27 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P28:
			g_tVar.P28 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P29:
			g_tVar.P29 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P2A:
			g_tVar.P2A = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P2B:
			g_tVar.P2B = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P2C:
			g_tVar.P2C = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P2D:
			g_tVar.P2D = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P2E:
			g_tVar.P2E = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P2F:
			g_tVar.P2F = reg_value;	      /* 将寄存器值读出 */
			break;			
		
		case SLAVE_REG_P30:
			g_tVar.P30 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P31:
			g_tVar.P31 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P32:
			g_tVar.P32 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P33:
			g_tVar.P33 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P34:
			g_tVar.P34 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P35:
			g_tVar.P35 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P36:
			g_tVar.P36 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P37:
			g_tVar.P37 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P38:
			g_tVar.P38 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P39:
			g_tVar.P39 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P3A:
			g_tVar.P3A = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P3B:
			g_tVar.P3B = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P3C:
			g_tVar.P3C = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P3D:
			g_tVar.P3D = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P3E:
			g_tVar.P3E = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P3F:
			g_tVar.P3F = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P40:
			g_tVar.P40 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P41:
			g_tVar.P41 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P42:
			g_tVar.P42 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P43:
			g_tVar.P43 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P44:
			g_tVar.P44 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P45:
			g_tVar.P45 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P46:
			g_tVar.P46 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P47:
			g_tVar.P47 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P48:
			g_tVar.P48 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P49:
			g_tVar.P49 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P4A:
			g_tVar.P4A = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P4B:
			g_tVar.P4B = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P4C:
			g_tVar.P4C = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P4D:
			g_tVar.P4D = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P4E:
			g_tVar.P4E = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P4F:
			g_tVar.P4F = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P50:
			g_tVar.P50 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P51:
			g_tVar.P51 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P52:
			g_tVar.P52 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P53:
			g_tVar.P53 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P54:
			g_tVar.P54 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P55:
			g_tVar.P55 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P56:
			g_tVar.P56 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P57:
			g_tVar.P57 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P58:
			g_tVar.P58 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P59:
			g_tVar.P59 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P5A:
			g_tVar.P5A = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P5B:
			g_tVar.P5B = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P5C:
			g_tVar.P5C = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P5D:
			g_tVar.P5D = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P5E:
			g_tVar.P5E = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P5F:
			g_tVar.P5F = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P60:
			g_tVar.P60 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P61:
			g_tVar.P61 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P62:
			g_tVar.P62 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P63:
			g_tVar.P63 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P64:
			g_tVar.P64 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P65:
			g_tVar.P65 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P66:
			g_tVar.P66 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P67:
			g_tVar.P67 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P68:
			g_tVar.P68 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P69:
			g_tVar.P69 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P6A:
			g_tVar.P6A = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P6B:
			g_tVar.P6B = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P6C:
			g_tVar.P6C = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P6D:
			g_tVar.P6D = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P6E:
			g_tVar.P6E = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P6F:
			g_tVar.P6F = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P70:
			g_tVar.P70 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P71:
			g_tVar.P71 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P72:
			g_tVar.P72 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P73:
			g_tVar.P73 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P74:
			g_tVar.P74 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P75:
			g_tVar.P75 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P76:
			g_tVar.P76 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P77:
			g_tVar.P77 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P78:
			g_tVar.P78 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P79:
			g_tVar.P79 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P7A:
			g_tVar.P7A = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P7B:
			g_tVar.P7B = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P7C:
			g_tVar.P7C = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P7D:
			g_tVar.P7D = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P7E:
			g_tVar.P7E = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P7F:
			g_tVar.P7F = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P80:
			g_tVar.P80 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P81:
			g_tVar.P81 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P82:
			g_tVar.P82 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P83:
			g_tVar.P83 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P84:
			g_tVar.P84 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P85:
			g_tVar.P85 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P86:
			g_tVar.P86 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P87:
			g_tVar.P87 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P88:
			g_tVar.P88 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P89:
			g_tVar.P89 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P8A:
			g_tVar.P8A = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P8B:
			g_tVar.P8B = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P8C:
			g_tVar.P8C = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P8D:
			g_tVar.P8D = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P8E:
			g_tVar.P8E = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P8F:
			g_tVar.P8F = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P90:
			g_tVar.P90 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P91:
			g_tVar.P91 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P92:
			g_tVar.P92 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P93:
			g_tVar.P93 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P94:
			g_tVar.P94 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P95:
			g_tVar.P95 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P96:
			g_tVar.P96 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P97:
			g_tVar.P97 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P98:
			g_tVar.P98 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P99:
			g_tVar.P99 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_P9A:
			g_tVar.P9A = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P9B:
			g_tVar.P9B = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P9C:
			g_tVar.P9C = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_P9D:
			g_tVar.P9D = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_P9E:
			g_tVar.P9E = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_P9F:
			g_tVar.P9F = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PA0:
			g_tVar.PA0 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PA1:
			g_tVar.PA1 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PA2:
			g_tVar.PA2 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PA3:
			g_tVar.PA3 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PA4:
			g_tVar.PA4 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PA5:
			g_tVar.PA5 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PA6:
			g_tVar.PA6 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PA7:
			g_tVar.PA7 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PA8:
			g_tVar.PA8 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PA9:
			g_tVar.PA9 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_PAA:
			g_tVar.PAA = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PAB:
			g_tVar.PAB = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PAC:
			g_tVar.PAC = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PAD:
			g_tVar.PAD = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PAE:
			g_tVar.PAE = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_PAF:
			g_tVar.PAF = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PB0:
			g_tVar.PB0 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PB1:
			g_tVar.PB1 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PB2:
			g_tVar.PB2 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PB3:
			g_tVar.PB3 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PB4:
			g_tVar.PB4 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PB5:
			g_tVar.PB5 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PB6:
			g_tVar.PB6 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PB7:
			g_tVar.PB7 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PB8:
			g_tVar.PB8 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PB9:
			g_tVar.PB9 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_PBA:
			g_tVar.PBA = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PBB:
			g_tVar.PBB = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PBC:
			g_tVar.PBC = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PBD:
			g_tVar.PBD = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PBE:
			g_tVar.PBE = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_PBF:
			g_tVar.PBF = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PC0:
			g_tVar.PC0 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PC1:
			g_tVar.PC1 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PC2:
			g_tVar.PC2 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PC3:
			g_tVar.PC3 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PC4:
			g_tVar.PC4 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PC5:
			g_tVar.PC5 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PC6:
			g_tVar.PC6 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PC7:
			g_tVar.PC7 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PC8:
			g_tVar.PC8 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PC9:
			g_tVar.PC9 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_PCA:
			g_tVar.PCA = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PCB:
			g_tVar.PCB = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PCC:
			g_tVar.PCC = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PCD:
			g_tVar.PCD = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PCE:
			g_tVar.PCE = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_PCF:
			g_tVar.PCF = reg_value;	      /* 将寄存器值读出 */
			break;
			
	
		case SLAVE_REG_PD0:
			g_tVar.PD0 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PD1:
			g_tVar.PD1 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PD2:
			g_tVar.PD2 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PD3:
			g_tVar.PD3 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PD4:
			g_tVar.PD4 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PD5:
			g_tVar.PD5 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PD6:
			g_tVar.PD6 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PD7:
			g_tVar.PD7 = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PD8:
			g_tVar.PD8 = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PD9:
			g_tVar.PD9 = reg_value;	      /* 将寄存器值读出 */
			break;		
		
		case SLAVE_REG_PDA:
			g_tVar.PDA = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PDB:
			g_tVar.PDB = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PDC:
			g_tVar.PDC = reg_value;	      /* 将寄存器值读出 */
			break;

		case SLAVE_REG_PDD:
			g_tVar.PDD = reg_value;				/* 将寄存器值读出 */
			break;
		
		case SLAVE_REG_PDE:
			g_tVar.PDE = reg_value;	      /* 将寄存器值读出 */
			break;	

		case SLAVE_REG_PDF:
			g_tVar.PDF = reg_value;	      /* 将寄存器值读出 */
			break;
	
		default:
			return 0;		/* 参数异常，返回 0 */
	}

	return 1;		/* 读取成功 */
}




/*********************************************************************************************************
*	函 数 名: MODS_03H
*	功能说明: 读取保持寄存器 在一个或多个保持寄存器中取得当前的二进制值
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************/
static void MODS_03H(void)
{
	/*
		从机地址为11H。保持寄存器的起始地址为006BH，结束地址为006DH。该次查询总共访问3个保持寄存器。

		主机发送:
			11 从机地址
			03 功能码
			00 寄存器地址高字节
			6B 寄存器地址低字节
			00 寄存器数量高字节
			03 寄存器数量低字节
			76 CRC高字节
			87 CRC低字节

		从机应答: 	保持寄存器的长度为2个字节。对于单个保持寄存器而言，寄存器高字节数据先被传输，
					低字节数据后被传输。保持寄存器之间，低地址寄存器先被传输，高地址寄存器后被传输。
			11 从机地址
			03 功能码
			06 字节数
			00 数据1高字节(006BH)
			6B 数据1低字节(006BH)
			00 数据2高字节(006CH)
			13 数据2 低字节(006CH)
			00 数据3高字节(006DH)
			00 数据3低字节(006DH)
			38 CRC高字节
			B9 CRC低字节

		例子:
			01 03 30 06 00 01  6B0B      ---- 读 3006H, 触发电流
			01 03 4000 0010 51C6         ---- 读 4000H 倒数第1条浪涌记录 32字节
			01 03 4001 0010 0006         ---- 读 4001H 倒数第1条浪涌记录 32字节

			01 03 F000 0008 770C         ---- 读 F000H 倒数第1条告警记录 16字节
			01 03 F001 0008 26CC         ---- 读 F001H 倒数第2条告警记录 16字节

			01 03 7000 0020 5ED2         ---- 读 7000H 倒数第1条波形记录第1段 64字节
			01 03 7001 0020 0F12         ---- 读 7001H 倒数第1条波形记录第2段 64字节

			01 03 7040 0020 5F06         ---- 读 7040H 倒数第2条波形记录第1段 64字节
	*/
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint8_t reg_value[160];
	memset(g_tModS.TxBuf,0,sizeof(g_tModS.TxBuf));

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)								/* 03H命令必须是8个字节 */
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 				/* 寄存器号 */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);					/* 寄存器个数 */
	if (num > sizeof(reg_value) / 2)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* 数据值域错误 */
		goto err_ret;
	}

	for (i = 0; i < num; i++)
	{
		if (MODS_ReadRegValue(reg, &reg_value[2 * i]) == 0)	/* 读出寄存器值放入reg_value */
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* 寄存器地址错误 */
			break;
		}
		reg++;
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)							/* 正确应答 */
	{
		g_tModS.TxCount = 0;
		if(g_tModS.RxBuf[0]==0xFF)                          // 确认返回485地址读取命令
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		}else
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = *data%256;
		}
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			/* 返回字节数 */

		for (i = 0; i < num; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i];
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i+1];
		}
			// 开始返回Modbus数据
		if(usartFlag==1)
			{
				MODS_SendWithCRCUS0(g_tModS.TxBuf, g_tModS.TxCount);							
			}else if(RS485Flag==1)
				{
					MODS_SendWithCRCUS1(g_tModS.TxBuf, g_tModS.TxCount);							
				}		/* 发送正确应答 */
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);					/* 发送错误应答 */
	}
}



/*********************************************************************************************************
*	函 数 名: MODS_04H
*	功能说明: 读取输入寄存器（对应A01/A02） SMA
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************/
static void MODS_04H(void)
{
	/*
		主机发送:
			11 从机地址
			04 功能码
			00 寄存器起始地址高字节
			08 寄存器起始地址低字节
			00 寄存器个数高字节
			02 寄存器个数低字节
			F2 CRC高字节
			99 CRC低字节

		从机应答:  输入寄存器长度为2个字节。对于单个输入寄存器而言，寄存器高字节数据先被传输，
				低字节数据后被传输。输入寄存器之间，低地址寄存器先被传输，高地址寄存器后被传输。
			11 从机地址
			04 功能码
			04 字节数
			00 数据1高字节(0008H)
			0A 数据1低字节(0008H)
			00 数据2高字节(0009H)
			0B 数据2低字节(0009H)
			8B CRC高字节
			80 CRC低字节

		例子:

			01 04 2201 0006 2BB0  --- 读 2201H A01通道模拟量 开始的6个数据
			01 04 2201 0001 6A72  --- 读 2201H

	*/
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint16_t status[10];

	memset(status, 0, 10);

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;	/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);	/* 寄存器个数 */
	
	if ((reg >= REG_A01) && (num > 0) && (reg + num <= REG_AXX + 1))
	{	
		for (i = 0; i < num; i++)
		{
			switch (reg)
			{
				/* 测试参数 */
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
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)		/* 正确应答 */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			/* 返回字节数 */

		for (i = 0; i < num; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i] >> 8;
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i] & 0xFF;
		}
			// 开始返回Modbus数据
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
		MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
	}
}



/*********************************************************************************************************
*	函 数 名: MODS_05H
*	功能说明: 强制单线圈（对应D01/D02/D03）
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************/
static void MODS_05H(void)
{
	/*
		主机发送: 写单个线圈寄存器。FF00H值请求线圈处于ON状态，0000H值请求线圈处于OFF状态
		。05H指令设置单个线圈的状态，15H指令可以设置多个线圈的状态。
			11 从机地址
			05 功能码
			00 寄存器地址高字节
			AC 寄存器地址低字节
			FF 数据1高字节
			00 数据2低字节
			4E CRC校验高字节
			8B CRC校验低字节

		从机应答:
			11 从机地址
			05 功能码
			00 寄存器地址高字节
			AC 寄存器地址低字节
			FF 寄存器1高字节
			00 寄存器1低字节
			4E CRC校验高字节
			8B CRC校验低字节

		例子:
		01 05 10 01 FF 00   D93A   -- D01打开
		01 05 10 01 00 00   98CA   -- D01关闭

		01 05 10 02 FF 00   293A   -- D02打开
		01 05 10 02 00 00   68CA   -- D02关闭

		01 05 10 03 FF 00   78FA   -- D03打开
		01 05 10 03 00 00   390A   -- D03关闭
	*/
	uint16_t reg;
	uint16_t value;
	memset(g_tModS.TxBuf,0,sizeof(g_tModS.TxBuf));

	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* 数据 */
	
	if (value != 0x0000 && value != 0xFF00)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
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
		
		default:g_tModS.RspCode = RSP_ERR_REG_ADDR;		break;/* 寄存器地址错误 */
		
	}
err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* 正确应答 */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* 告诉主机命令错误 */
	}
}



/*********************************************************************************************************
*	函 数 名: MODS_06H
*	功能说明: 写单个寄存器
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************/
static void MODS_06H(void)
{

	/*
		写保持寄存器。注意06指令只能操作单个保持寄存器，16指令可以设置单个或多个保持寄存器

		主机发送:
			11 从机地址
			06 功能码
			00 寄存器地址高字节
			01 寄存器地址低字节
			00 数据1高字节
			01 数据1低字节
			9A CRC校验高字节
			9B CRC校验低字节

		从机响应:
			11 从机地址
			06 功能码
			00 寄存器地址高字节
			01 寄存器地址低字节
			00 数据1高字节
			01 数据1低字节
			1B CRC校验高字节
			5A	CRC校验低字节

		例子:
			01 06 30 06 00 25  A710    ---- 触发电流设置为 2.5
			01 06 30 06 00 10  6707    ---- 触发电流设置为 1.0


			01 06 30 1B 00 00  F6CD    ---- SMA 滤波系数 = 0 关闭滤波
			01 06 30 1B 00 01  370D    ---- SMA 滤波系数 = 1
			01 06 30 1B 00 02  770C    ---- SMA 滤波系数 = 2
			01 06 30 1B 00 05  36CE    ---- SMA 滤波系数 = 5

			01 06 30 07 00 01  F6CB    ---- 测试模式修改为 T1
			01 06 30 07 00 02  B6CA    ---- 测试模式修改为 T2

			01 06 31 00 00 00  8736    ---- 擦除浪涌记录区
			01 06 31 01 00 00  D6F6    ---- 擦除告警记录区

*/

	uint16_t reg;
	uint16_t value;
	memset(g_tModS.TxBuf,0,sizeof(g_tModS.TxBuf));

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* 寄存器值 */

	if (MODS_WriteRegValue(reg, value) == 1)	/* 该函数会把写入的值存入寄存器 */
	{
		;
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* 正确应答 */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* 告诉主机命令错误 */
	}
}



/*********************************************************************************************************
*	函 数 名: MODS_10H
*	功能说明: 连续写多个寄存器.  进用于改写时钟
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************/
static void MODS_10H(void)
{
	/*
		从机地址为11H。保持寄存器的其实地址为0001H，寄存器的结束地址为0002H。总共访问2个寄存器。
		保持寄存器0001H的内容为000AH，保持寄存器0002H的内容为0102H。

		主机发送:
			11 从机地址
			10 功能码
			00 寄存器起始地址高字节
			01 寄存器起始地址低字节
			00 寄存器数量高字节
			02 寄存器数量低字节
			04 字节数
			00 数据1高字节
			0A 数据1低字节
			01 数据2高字节
			02 数据2低字节
			C6 CRC校验高字节
			F0 CRC校验低字节

		从机响应:
			11 从机地址
			06 功能码
			00 寄存器地址高字节
			01 寄存器地址低字节
			00 数据1高字节
			01 数据1低字节
			1B CRC校验高字节
			5A	CRC校验低字节

		例子:
			01 10 30 00 00 06 0C  07 DE  00 0A  00 01  00 08  00 0C  00 00     389A    ---- 写时钟 2014-10-01 08:12:00
			01 10 30 00 00 06 0C  07 DF  00 01  00 1F  00 17  00 3B  00 39     5549    ---- 写时钟 2015-01-31 23:59:57

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
		g_tModS.RspCode = RSP_ERR_VALUE;			/* 数据值域错误 */
		goto err_ret;
	}

	reg_addr = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	reg_num = BEBufToUint16(&g_tModS.RxBuf[4]);		/* 寄存器个数 */
	byte_num = g_tModS.RxBuf[6];					/* 后面的数据体字节数 */

	if (byte_num != 2 * reg_num)
	{
		;
	}
	
	for (i = 0; i < reg_num; i++)
	{
		value = BEBufToUint16(&g_tModS.RxBuf[7 + 2 * i]);	/* 寄存器值 */

		if (MODS_WriteRegValue(reg_addr + i, value) == 1)
		{
			;
		}
		else
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
			break;
		}
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)					/* 正确应答 */
	{
		MODS_SendAckOk17();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);			/* 告诉主机命令错误 */
	}
}


/*
*********************************************************************************************************
*	函 数 名: MODS_AnalyzeApp
*	功能说明: 分析应用层协议
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
		switch (g_tModS.RxBuf[1])				/* 第2个字节 功能码 */
		{
			case 0x01:							/* 读取线圈状态（读一些位状态）*/
				MODS_01H();
				break;

			case 0x02:							/* 读取输入状态（按键状态）*/
				MODS_02H();		
				break;
			
			case 0x03:							/* 读取保持寄存器（此例程存在g_tVar中）*/
				MODS_03H();				
				break;
			
			case 0x04:							/* 读取输入寄存器（ADC的值）*/
				MODS_04H();				
				break;
			
			case 0x05:							/* 强制单线圈（设置DO）*/
				MODS_05H();				
				break;
			
			case 0x06:							/* 写单个保存寄存器（此例程改写g_tVar中的参数）*/
				MODS_06H();					
				break;
				
			case 0x10:							/* 写多个保存寄存器（此例程存在g_tVar中的参数）*/
				MODS_10H();				
				break;
			
			default:
				g_tModS.RspCode = RSP_ERR_CMD;
				MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
				break;
		}	
}


/*
*********************************************************************************************************
*	函 数 名: MODS_Poll
*	功能说明: 解析数据包. 在主程序中轮流调用。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
	void MODS_Poll(void)
		{
			uint8_t addr;
			/* 收到数据包第1字节 站地址 */ 
			addr = g_tModS.RxBuf[0];						
			/* 没有收到数据包 继续接收 */
			if(g_tModS.RxStatus == 0)  
			{
				 return;
			}
			/* 接收到的数据小于4个字节就认为错误 */
			if(g_tModS.RxCount < 4)				
			{
				// 此处可用于打印数据字节数不够 错误
			}
			/* Length and CRC check */
			if(( g_tModS.RxCount >= 4 ) && ( Modbus_CRC16((unsigned char*) g_tModS.RxBuf, g_tModS.RxCount ) == 0 ) ) 
				// 判断接收到的数据包的长度是不是大于modbus数据包的最小长度4	
			  // 换一种思路，我之前的理解是把除了最后两位校验位之外的数据进行计算crc校验的值和接收到的两位校验值进行比较，
			  // 实际上可以直接把接收到整个数组计算校验值如果算出来是0的话说明校验通过 
			 {	
				if(addr == 0xFF|| addr == *data%256)  // 检查地址是否是自己的地址 是 就开始解析功能码
				 {	
					/* 分析应用层协议  Modbus事件处理函数 */

					Modbus_05_func();   // 05 写线圈
					Modbus_06_func();   // 06 写寄存器
					Modbus_10_func();   // 10 写多个寄存器
					 				 
					MODS_AnalyzeApp();	// 01 03 查询命令返回及所有报文响应

				 }else if(g_tModS.RxBuf[0] == 0) 
				 {
						// 广播地址 放在对应的功能码处理函数里了 没有单独拿出来
				 }
			 }
			g_tModS.RxCount = 0;					// 必须清零计数器，方便下次帧同步 
			g_tModS.RxStatus = 0;         // 恢复接收状态
			usartFlag=0;
			RS485Flag=0;
		}

/*****************************  *********************************/
