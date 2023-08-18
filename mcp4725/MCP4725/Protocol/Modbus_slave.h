/*********************************************************************************************************
*
*	模块名称 : MODEBUS 通信模块 (从站）
*	文件名称 : modbus_slave.h
*
*********************************************************************************************************/

#ifndef __MODBUY_SLAVE_H
#define __MODBUY_SLAVE_H


#include <stdint.h>



#define SADDR485	1
#define SBAUD485	115200

/* 01H 读强制单线圈 */
/* 05H 写强制单线圈 */
#define COIL_D01		0x0001// DI状态
#define COIL_D02		0x0002
#define COIL_D03		0x0003
#define COIL_D04		0x0004
#define COIL_D05		0x0005
#define COIL_D06		0x0006
#define COIL_D07		0x0007
#define COIL_D08		0x0008
#define COIL_D09		0x0009
#define COIL_D0A		0x000A
#define COIL_D0B		0x000B
#define COIL_D0C		0x000C
#define COIL_D0D		0x000D
#define COIL_D0E		0x000E
#define COIL_D0F		0x000F
#define COIL_D10		0x0010

#define COIL_D11		0x0011// DO状态
#define COIL_D12		0x0012
#define COIL_D13		0x0013
#define COIL_D14		0x0014
#define COIL_D15		0x0015
#define COIL_D16		0x0016
#define COIL_D17		0x0017
#define COIL_D18		0x0018
#define COIL_D19		0x0019
#define COIL_D1A		0x001A
#define COIL_D1B		0x001B
#define COIL_D1C		0x001C
#define COIL_D1D		0x001D
#define COIL_D1E		0x001E
#define COIL_D1F		0x001F
#define COIL_D20		0x0020
#define COIL_DXX 	COIL_D20

/* 02H 读取输入状态 */
#define REG_T01		0x0201
#define REG_T02		0x0202
#define REG_T03		0x0203
#define REG_TXX		REG_T03

/* 03H 读保持寄存器 */
/* 06H 写保持寄存器 */
/* 10H 写多个保存寄存器 */
#define SLAVE_REG_P00	  0x0000     // 设备重启
#define SLAVE_REG_P01		0x0001     // 485地址
#define SLAVE_REG_P02		0x0002     // 产片型号
#define SLAVE_REG_P03		0x0003
#define SLAVE_REG_P04		0x0004
#define SLAVE_REG_P05	  0x0005
#define SLAVE_REG_P06		0x0006
#define SLAVE_REG_P07		0x0007
#define SLAVE_REG_P08 	0x0008
#define SLAVE_REG_P09		0x0009
#define SLAVE_REG_P0A	  0x000A
#define SLAVE_REG_P0B		0x000B
#define SLAVE_REG_P0C		0x000C
#define SLAVE_REG_P0D		0x000D    // 产品型号占用12个寄存器
#define SLAVE_REG_P0E		0x000E    // 恢复出厂设置
#define SLAVE_REG_P0F	  0x000F    // 工作模式：自动 手动

#define SLAVE_REG_P10	  0x0010    // 通道配置信息
#define SLAVE_REG_P11		0x0011
#define SLAVE_REG_P12		0x0012
#define SLAVE_REG_P13		0x0013
#define SLAVE_REG_P14		0x0014

#define SLAVE_REG_P15	  0x0015
#define SLAVE_REG_P16		0x0016
#define SLAVE_REG_P17		0x0017
#define SLAVE_REG_P18 	0x0018
#define SLAVE_REG_P19		0x0019

#define SLAVE_REG_P1A	  0x001A
#define SLAVE_REG_P1B		0x001B
#define SLAVE_REG_P1C		0x001C
#define SLAVE_REG_P1D		0x001D
#define SLAVE_REG_P1E		0x001E

#define SLAVE_REG_P1F	  0x001F
#define SLAVE_REG_P20		0x0020
#define SLAVE_REG_P21		0x0021
#define SLAVE_REG_P22		0x0022
#define SLAVE_REG_P23 	0x0023

#define SLAVE_REG_P24	  0x0024
#define SLAVE_REG_P25		0x0025
#define SLAVE_REG_P26		0x0026
#define SLAVE_REG_P27		0x0027
#define SLAVE_REG_P28 	0x0028

#define SLAVE_REG_P29	  0x0029
#define SLAVE_REG_P2A		0x002A
#define SLAVE_REG_P2B		0x002B
#define SLAVE_REG_P2C		0x002C
#define SLAVE_REG_P2D 	0x002D

#define SLAVE_REG_P2E	  0x002E
#define SLAVE_REG_P2F		0x002F
#define SLAVE_REG_P30		0x0030
#define SLAVE_REG_P31		0x0031
#define SLAVE_REG_P32	  0x0032

#define SLAVE_REG_P33   0x0033
#define SLAVE_REG_P34		0x0034
#define SLAVE_REG_P35	  0x0035
#define SLAVE_REG_P36		0x0036
#define SLAVE_REG_P37	  0x0037

#define SLAVE_REG_P38   0x0038
#define SLAVE_REG_P39		0x0039
#define SLAVE_REG_P3A	  0x003A
#define SLAVE_REG_P3B		0x003B
#define SLAVE_REG_P3C	  0x003C

#define SLAVE_REG_P3D   0x003D
#define SLAVE_REG_P3E		0x003E
#define SLAVE_REG_P3F	  0x003F
#define SLAVE_REG_P40		0x0040
#define SLAVE_REG_P41	  0x0041

#define SLAVE_REG_P42		0x0042
#define SLAVE_REG_P43	  0x0043
#define SLAVE_REG_P44		0x0044
#define SLAVE_REG_P45	  0x0045
#define SLAVE_REG_P46		0x0046

#define SLAVE_REG_P47		0x0047
#define SLAVE_REG_P48	  0x0048
#define SLAVE_REG_P49		0x0049
#define SLAVE_REG_P4A	  0x004A
#define SLAVE_REG_P4B		0x004B

#define SLAVE_REG_P4C	  0x004C
#define SLAVE_REG_P4D		0x004D
#define SLAVE_REG_P4E	  0x004E
#define SLAVE_REG_P4F		0x004F
#define SLAVE_REG_P50		0x0050

#define SLAVE_REG_P51		0x0051
#define SLAVE_REG_P52		0x0052
#define SLAVE_REG_P53		0x0053
#define SLAVE_REG_P54		0x0054
#define SLAVE_REG_P55		0x0055

#define SLAVE_REG_P56		0x0056
#define SLAVE_REG_P57		0x0057
#define SLAVE_REG_P58		0x0058
#define SLAVE_REG_P59		0x0059
#define SLAVE_REG_P5A		0x005A

#define SLAVE_REG_P5B		0x005B
#define SLAVE_REG_P5C		0x005C
#define SLAVE_REG_P5D		0x005D
#define SLAVE_REG_P5E		0x005E
#define SLAVE_REG_P5F		0x005F


#define SLAVE_REG_P60		0x0060
#define SLAVE_REG_P61		0x0061
#define SLAVE_REG_P62		0x0062
#define SLAVE_REG_P63		0x0063
#define SLAVE_REG_P64		0x0064
#define SLAVE_REG_P65		0x0065
#define SLAVE_REG_P66		0x0066
#define SLAVE_REG_P67		0x0067
#define SLAVE_REG_P68		0x0068
#define SLAVE_REG_P69		0x0069
#define SLAVE_REG_P6A		0x006A
#define SLAVE_REG_P6B		0x006B
#define SLAVE_REG_P6C		0x006C
#define SLAVE_REG_P6D		0x006D
#define SLAVE_REG_P6E		0x006E
#define SLAVE_REG_P6F		0x006F

#define SLAVE_REG_P70		0x0070
#define SLAVE_REG_P71		0x0071
#define SLAVE_REG_P72		0x0072
#define SLAVE_REG_P73		0x0073
#define SLAVE_REG_P74		0x0074
#define SLAVE_REG_P75		0x0075
#define SLAVE_REG_P76		0x0076
#define SLAVE_REG_P77		0x0077
#define SLAVE_REG_P78		0x0078
#define SLAVE_REG_P79		0x0079
#define SLAVE_REG_P7A		0x007A
#define SLAVE_REG_P7B		0x007B
#define SLAVE_REG_P7C		0x007C
#define SLAVE_REG_P7D		0x007D
#define SLAVE_REG_P7E		0x007E
#define SLAVE_REG_P7F		0x007F

#define SLAVE_REG_P80		0x0080
#define SLAVE_REG_P81		0x0081
#define SLAVE_REG_P82		0x0082
#define SLAVE_REG_P83		0x0083
#define SLAVE_REG_P84		0x0084

#define SLAVE_REG_P85		0x0085
#define SLAVE_REG_P86		0x0086
#define SLAVE_REG_P87		0x0087
#define SLAVE_REG_P88		0x0088
#define SLAVE_REG_P89		0x0089

#define SLAVE_REG_P8A		0x008A
#define SLAVE_REG_P8B		0x008B
#define SLAVE_REG_P8C		0x008C
#define SLAVE_REG_P8D		0x008D
#define SLAVE_REG_P8E		0x008E

#define SLAVE_REG_P8F		0x008F
#define SLAVE_REG_P90		0x0090
#define SLAVE_REG_P91		0x0091
#define SLAVE_REG_P92		0x0092
#define SLAVE_REG_P93		0x0093

#define SLAVE_REG_P94		0x0094
#define SLAVE_REG_P95		0x0095
#define SLAVE_REG_P96		0x0096
#define SLAVE_REG_P97		0x0097
#define SLAVE_REG_P98		0x0098

#define SLAVE_REG_P99		0x0099
#define SLAVE_REG_P9A		0x009A
#define SLAVE_REG_P9B		0x009B
#define SLAVE_REG_P9C		0x009C
#define SLAVE_REG_P9D		0x009D

#define SLAVE_REG_P9E		0x009E
#define SLAVE_REG_P9F		0x009F
#define SLAVE_REG_PA0		0x00A0
#define SLAVE_REG_PA1		0x00A1
#define SLAVE_REG_PA2		0x00A2

#define SLAVE_REG_PA3		0x00A3
#define SLAVE_REG_PA4		0x00A4
#define SLAVE_REG_PA5		0x00A5
#define SLAVE_REG_PA6		0x00A6
#define SLAVE_REG_PA7		0x00A7
#define SLAVE_REG_PA8		0x00A8
#define SLAVE_REG_PA9		0x00A9
#define SLAVE_REG_PAA		0x00AA
#define SLAVE_REG_PAB		0x00AB
#define SLAVE_REG_PAC		0x00AC
#define SLAVE_REG_PAD		0x00AD
#define SLAVE_REG_PAE		0x00AE
#define SLAVE_REG_PAF		0x00AF

#define SLAVE_REG_PB0		0x00B0
#define SLAVE_REG_PB1		0x00B1
#define SLAVE_REG_PB2		0x00B2
#define SLAVE_REG_PB3		0x00B3
#define SLAVE_REG_PB4		0x00B4
#define SLAVE_REG_PB5		0x00B5
#define SLAVE_REG_PB6		0x00B6
#define SLAVE_REG_PB7		0x00B7
#define SLAVE_REG_PB8		0x00B8
#define SLAVE_REG_PB9		0x00B9
#define SLAVE_REG_PBA		0x00BA
#define SLAVE_REG_PBB		0x00BB
#define SLAVE_REG_PBC		0x00BC
#define SLAVE_REG_PBD		0x00BD
#define SLAVE_REG_PBE		0x00BE
#define SLAVE_REG_PBF		0x00BF

#define SLAVE_REG_PC0		0x00C0
#define SLAVE_REG_PC1		0x00C1
#define SLAVE_REG_PC2		0x00C2
#define SLAVE_REG_PC3		0x00C3
#define SLAVE_REG_PC4		0x00C4
#define SLAVE_REG_PC5		0x00C5
#define SLAVE_REG_PC6		0x00C6
#define SLAVE_REG_PC7		0x00C7
#define SLAVE_REG_PC8		0x00C8
#define SLAVE_REG_PC9		0x00C9
#define SLAVE_REG_PCA		0x00CA
#define SLAVE_REG_PCB		0x00CB
#define SLAVE_REG_PCC		0x00CC
#define SLAVE_REG_PCD		0x00CD
#define SLAVE_REG_PCE		0x00CE
#define SLAVE_REG_PCF		0x00CF

#define SLAVE_REG_PD0		0x00D0
#define SLAVE_REG_PD1		0x00D1
#define SLAVE_REG_PD2		0x00D2
#define SLAVE_REG_PD3		0x00D3
#define SLAVE_REG_PD4		0x00D4
#define SLAVE_REG_PD5		0x00D5
#define SLAVE_REG_PD6		0x00D6
#define SLAVE_REG_PD7		0x00D7
#define SLAVE_REG_PD8		0x00D8
#define SLAVE_REG_PD9		0x00D9
#define SLAVE_REG_PDA		0x00DA
#define SLAVE_REG_PDB		0x00DB
#define SLAVE_REG_PDC		0x00DC
#define SLAVE_REG_PDD		0x00DD
#define SLAVE_REG_PDE		0x00DE
#define SLAVE_REG_PDF		0x00DF

#define SLAVE_REG_PE0		0x00E0
#define SLAVE_REG_PE1		0x00E1
/* 04H 读取输入寄存器(模拟信号) */
#define REG_A01		0x0401
#define REG_AXX		REG_A01


/* RTU 应答代码 */
#define RSP_OK				    0		  /* 成功 */
#define RSP_ERR_CMD			  0x01	/* 不支持的功能码 */
#define RSP_ERR_REG_ADDR  0x02	/* 寄存器地址错误 */
#define RSP_ERR_VALUE		  0x03	/* 数据值域错误 */
#define RSP_ERR_WRITE		  0x04	/* 写入失败 */

#define S_RX_BUF_SIZE		40
#define S_TX_BUF_SIZE		256

typedef struct
{
	uint8_t RxBuf[S_RX_BUF_SIZE]; //modbus接受缓冲区
	uint8_t RxCount;              //modbus端口接收到的数据个数
	uint8_t RxStatus;             //modbus一帧数据接受完成标志位
	
  uint8_t TimeRun;              //modbus定时器计时标志 0空闲 1计时
	uint8_t TimeOut;              //modbus数据接收持续时间
	
	uint8_t RspCode;              //功能码应答状态

	uint8_t TxBuf[S_TX_BUF_SIZE]; //modbus接发送缓冲区
	uint8_t TxCount;              //modbus端口接收到的数据个数
}MODS_T;

typedef struct
{
	/* 03H 06H 读写保持寄存器 */
	uint16_t P00, P01, P02, P03, P04, P05, P06, P07, P08, P09, P0A, P0B, P0C, P0D,P0E, P0F;
	
	uint16_t P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P1A, P1B, P1C, P1D,P1E, P1F;
	uint16_t P20, P21, P22, P23, P24, P25, P26, P27, P28, P29, P2A, P2B, P2C, P2D,P2E, P2F;
	uint16_t P30, P31, P32, P33, P34, P35, P36, P37, P38, P39, P3A, P3B, P3C, P3D,P3E, P3F;
	uint16_t P40, P41, P42, P43, P44, P45, P46, P47, P48, P49, P4A, P4B, P4C, P4D,P4E, P4F;
	uint16_t P50, P51, P52, P53, P54, P55, P56, P57, P58, P59, P5A, P5B, P5C, P5D,P5E, P5F;
	uint16_t P60, P61, P62, P63, P64, P65, P66, P67, P68, P69, P6A, P6B, P6C, P6D,P6E, P6F;
	uint16_t P70, P71, P72, P73, P74, P75, P76, P77, P78, P79, P7A, P7B, P7C, P7D,P7E, P7F;
	uint16_t P80, P81, P82, P83, P84, P85, P86, P87, P88, P89, P8A, P8B, P8C, P8D,P8E, P8F;
	uint16_t P90, P91, P92, P93, P94, P95, P96, P97, P98, P99, P9A, P9B, P9C, P9D,P9E, P9F;
	uint16_t PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PAA, PAB, PAC, PAD,PAE, PAF;
	uint16_t PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PBA, PBB, PBC, PBD,PBE, PBF;
	uint16_t PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PCA, PCB, PCC, PCD,PCE, PCF;
	uint16_t PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PDA, PDB, PDC, PDD,PDE, PDF;
	uint16_t PE0, PE1;

	/* 04H 读取模拟量寄存器 */
	uint16_t A01;

	/* 01H 05H 读写单个强制线圈 */
	uint16_t D01, D02, D03, D04, D05, D06, D07, D08, D09, D0A, D0B, D0C, D0D, D0E, D0F, D10;	
}VAR_T;

void MODS_Poll(void);
void MODS_ReciveNew(uint8_t _byte);


extern MODS_T g_tModS;
extern VAR_T g_tVar;
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
