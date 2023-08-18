/************************************************************ 
 * 文件：FLASH.h
************************************************************/
#ifndef __FLASH_H
#define __FLASH_H

/************************* 头文件 *************************/

#include "gd32f10x.h"



#define FMC_PAGE_SIZE                  ((uint16_t)0x800U)          //FLASH页大小，2048B  2KB


//记录设备类型的起始地址：页32              计算方法：08后面的数/0x800
#define FMC_WRITE_START_1_lxADDR        ((uint32_t)0x08010000U)    
#define FMC_WRITE_START_2_lxADDR        ((uint32_t)0x08010800U)    
#define FMC_WRITE_START_3_lxADDR        ((uint32_t)0x08011000U)    
#define FMC_WRITE_START_4_lxADDR        ((uint32_t)0x08011800U)   
#define FMC_WRITE_START_5_lxADDR        ((uint32_t)0x08012000U)    
#define FMC_WRITE_START_6_lxADDR        ((uint32_t)0x08012800U)    
#define FMC_WRITE_START_7_lxADDR        ((uint32_t)0x08013000U)    
#define FMC_WRITE_START_8_lxADDR        ((uint32_t)0x08013800U)    
#define FMC_WRITE_START_9_lxADDR        ((uint32_t)0x08014000U)    
#define FMC_WRITE_START_10_lxADDR       ((uint32_t)0x08014800U)   
#define FMC_WRITE_START_11_lxADDR       ((uint32_t)0x08015000U)   
#define FMC_WRITE_START_12_lxADDR       ((uint32_t)0x08015800U)   
#define FMC_WRITE_START_13_lxADDR       ((uint32_t)0x08016000U)    
#define FMC_WRITE_START_14_lxADDR       ((uint32_t)0x08016800U)    
#define FMC_WRITE_START_15_lxADDR       ((uint32_t)0x08017000U)    
#define FMC_WRITE_START_16_lxADDR       ((uint32_t)0x08017800U)   

//设备类型结束地址               
#define FMC_WRITE_END_1_lxADDR          ((uint32_t)0x080107FFU)      
#define FMC_WRITE_END_2_lxADDR          ((uint32_t)0x08010FFFU)      
#define FMC_WRITE_END_3_lxADDR          ((uint32_t)0x080117FFU)     
#define FMC_WRITE_END_4_lxADDR          ((uint32_t)0x08011FFFU)      
#define FMC_WRITE_END_5_lxADDR          ((uint32_t)0x080127FFU)      
#define FMC_WRITE_END_6_lxADDR          ((uint32_t)0x08012FFFU)      
#define FMC_WRITE_END_7_lxADDR          ((uint32_t)0x080137FFU)      
#define FMC_WRITE_END_8_lxADDR          ((uint32_t)0x08013FFFU)     
#define FMC_WRITE_END_9_lxADDR          ((uint32_t)0x080147FFU)      
#define FMC_WRITE_END_10_lxADDR         ((uint32_t)0x08014FFFU) 
#define FMC_WRITE_END_11_lxADDR         ((uint32_t)0x080157FFU)     
#define FMC_WRITE_END_12_lxADDR         ((uint32_t)0x08015FFFU)      
#define FMC_WRITE_END_13_lxADDR         ((uint32_t)0x080167FFU)      
#define FMC_WRITE_END_14_lxADDR         ((uint32_t)0x08016FFFU)      
#define FMC_WRITE_END_15_lxADDR         ((uint32_t)0x080177FFU)      
#define FMC_WRITE_END_16_lxADDR         ((uint32_t)0x08017FFFU)      

 //记录量程上限的起始地址：页48
#define FMC_WRITE_START_1_lcsxADDR      ((uint32_t)0x08018000U)   
#define FMC_WRITE_START_2_lcsxADDR      ((uint32_t)0x08018800U)    
#define FMC_WRITE_START_3_lcsxADDR      ((uint32_t)0x08019000U)    
#define FMC_WRITE_START_4_lcsxADDR      ((uint32_t)0x08019800U)    
#define FMC_WRITE_START_5_lcsxADDR      ((uint32_t)0x0801A000U)    
#define FMC_WRITE_START_6_lcsxADDR      ((uint32_t)0x0801A800U)    
#define FMC_WRITE_START_7_lcsxADDR      ((uint32_t)0x0801B000U)    
#define FMC_WRITE_START_8_lcsxADDR      ((uint32_t)0x0801B800U)   
#define FMC_WRITE_START_9_lcsxADDR      ((uint32_t)0x0801C000U)    
#define FMC_WRITE_START_10_lcsxADDR     ((uint32_t)0x0801C800U)    
#define FMC_WRITE_START_11_lcsxADDR     ((uint32_t)0x0801D000U)    
#define FMC_WRITE_START_12_lcsxADDR     ((uint32_t)0x0801D800U)    
#define FMC_WRITE_START_13_lcsxADDR     ((uint32_t)0x0801E000U)    
#define FMC_WRITE_START_14_lcsxADDR     ((uint32_t)0x0801E800U)    
#define FMC_WRITE_START_15_lcsxADDR     ((uint32_t)0x0801F000U)    
#define FMC_WRITE_START_16_lcsxADDR     ((uint32_t)0x0801F800U)    

#define FMC_WRITE_END_1_lcsxADDR        ((uint32_t)0x080187FFU)      
#define FMC_WRITE_END_2_lcsxADDR        ((uint32_t)0x08018FFFU)      
#define FMC_WRITE_END_3_lcsxADDR        ((uint32_t)0x080197FFU)      
#define FMC_WRITE_END_4_lcsxADDR        ((uint32_t)0x08019FFFU)      
#define FMC_WRITE_END_5_lcsxADDR        ((uint32_t)0x0801A7FFU)      
#define FMC_WRITE_END_6_lcsxADDR        ((uint32_t)0x0801AFFFU)      
#define FMC_WRITE_END_7_lcsxADDR        ((uint32_t)0x0801B7FFU)      
#define FMC_WRITE_END_8_lcsxADDR        ((uint32_t)0x0801BFFFU)      
#define FMC_WRITE_END_9_lcsxADDR        ((uint32_t)0x0801C7FFU)      
#define FMC_WRITE_END_10_lcsxADDR       ((uint32_t)0x0801CFFFU)      
#define FMC_WRITE_END_11_lcsxADDR       ((uint32_t)0x0801D7FFU)      
#define FMC_WRITE_END_12_lcsxADDR       ((uint32_t)0x0801DFFFU)      
#define FMC_WRITE_END_13_lcsxADDR       ((uint32_t)0x0801E7FFU)      
#define FMC_WRITE_END_14_lcsxADDR       ((uint32_t)0x0801EFFFU)      
#define FMC_WRITE_END_15_lcsxADDR       ((uint32_t)0x0801F7FFU)      
#define FMC_WRITE_END_16_lcsxADDR       ((uint32_t)0x0801FFFFU)      



 //记录量程下限的起始地址：页64
#define FMC_WRITE_START_1_lcxxADDR      ((uint32_t)0x08020000U)    
#define FMC_WRITE_START_2_lcxxADDR      ((uint32_t)0x08020800U)    
#define FMC_WRITE_START_3_lcxxADDR      ((uint32_t)0x08021000U)    
#define FMC_WRITE_START_4_lcxxADDR      ((uint32_t)0x08021800U)    
#define FMC_WRITE_START_5_lcxxADDR      ((uint32_t)0x08022000U)    
#define FMC_WRITE_START_6_lcxxADDR      ((uint32_t)0x08022800U)    
#define FMC_WRITE_START_7_lcxxADDR      ((uint32_t)0x08023000U)    
#define FMC_WRITE_START_8_lcxxADDR      ((uint32_t)0x08023800U)    
#define FMC_WRITE_START_9_lcxxADDR      ((uint32_t)0x08024000U)    
#define FMC_WRITE_START_10_lcxxADDR     ((uint32_t)0x08024800U)    
#define FMC_WRITE_START_11_lcxxADDR     ((uint32_t)0x08025000U)    
#define FMC_WRITE_START_12_lcxxADDR     ((uint32_t)0x08025800U)    
#define FMC_WRITE_START_13_lcxxADDR     ((uint32_t)0x08026000U)    
#define FMC_WRITE_START_14_lcxxADDR     ((uint32_t)0x08026800U)    
#define FMC_WRITE_START_15_lcxxADDR     ((uint32_t)0x08027000U)    
#define FMC_WRITE_START_16_lcxxADDR     ((uint32_t)0x08027800U)    

#define FMC_WRITE_END_1_lcxxADDR        ((uint32_t)0x080207FFU)     
#define FMC_WRITE_END_2_lcxxADDR        ((uint32_t)0x08020FFFU)     
#define FMC_WRITE_END_3_lcxxADDR        ((uint32_t)0x080217FFU)     
#define FMC_WRITE_END_4_lcxxADDR        ((uint32_t)0x08021FFFU)     
#define FMC_WRITE_END_5_lcxxADDR        ((uint32_t)0x080227FFU)     
#define FMC_WRITE_END_6_lcxxADDR        ((uint32_t)0x08022FFFU)     
#define FMC_WRITE_END_7_lcxxADDR        ((uint32_t)0x080237FFU)     
#define FMC_WRITE_END_8_lcxxADDR        ((uint32_t)0x08023FFFU)     
#define FMC_WRITE_END_9_lcxxADDR        ((uint32_t)0x080247FFU)     
#define FMC_WRITE_END_10_lcxxADDR       ((uint32_t)0x08024FFFU)     
#define FMC_WRITE_END_11_lcxxADDR       ((uint32_t)0x080257FFU)     
#define FMC_WRITE_END_12_lcxxADDR       ((uint32_t)0x08025FFFU)     
#define FMC_WRITE_END_13_lcxxADDR       ((uint32_t)0x080267FFU)     
#define FMC_WRITE_END_14_lcxxADDR       ((uint32_t)0x08026FFFU)     
#define FMC_WRITE_END_15_lcxxADDR       ((uint32_t)0x080277FFU)     
#define FMC_WRITE_END_16_lcxxADDR       ((uint32_t)0x08027FFFU)     



 //记录阈值上限的起始地址：页80
#define FMC_WRITE_START_1_yzsxADDR      ((uint32_t)0x08028000U)    
#define FMC_WRITE_START_2_yzsxADDR      ((uint32_t)0x08028800U)    
#define FMC_WRITE_START_3_yzsxADDR      ((uint32_t)0x08029000U)    
#define FMC_WRITE_START_4_yzsxADDR      ((uint32_t)0x08029800U)    
#define FMC_WRITE_START_5_yzsxADDR      ((uint32_t)0x0802A000U)    
#define FMC_WRITE_START_6_yzsxADDR      ((uint32_t)0x0802A800U)    
#define FMC_WRITE_START_7_yzsxADDR      ((uint32_t)0x0802B000U)    
#define FMC_WRITE_START_8_yzsxADDR      ((uint32_t)0x0802B800U)    
#define FMC_WRITE_START_9_yzsxADDR      ((uint32_t)0x0802C000U)    
#define FMC_WRITE_START_10_yzsxADDR     ((uint32_t)0x0802C800U)    
#define FMC_WRITE_START_11_yzsxADDR     ((uint32_t)0x0802D000U)    
#define FMC_WRITE_START_12_yzsxADDR     ((uint32_t)0x0802D800U)    
#define FMC_WRITE_START_13_yzsxADDR     ((uint32_t)0x0802E000U)    
#define FMC_WRITE_START_14_yzsxADDR     ((uint32_t)0x0802E800U)    
#define FMC_WRITE_START_15_yzsxADDR     ((uint32_t)0x0802F000U)    
#define FMC_WRITE_START_16_yzsxADDR     ((uint32_t)0x0802F800U)    

#define FMC_WRITE_END_1_yzsxADDR        ((uint32_t)0x080287FFU)      
#define FMC_WRITE_END_2_yzsxADDR        ((uint32_t)0x08028FFFU)      
#define FMC_WRITE_END_3_yzsxADDR        ((uint32_t)0x080297FFU)      
#define FMC_WRITE_END_4_yzsxADDR        ((uint32_t)0x08029FFFU)      
#define FMC_WRITE_END_5_yzsxADDR        ((uint32_t)0x0802A7FFU)      
#define FMC_WRITE_END_6_yzsxADDR        ((uint32_t)0x0802AFFFU)      
#define FMC_WRITE_END_7_yzsxADDR        ((uint32_t)0x0802B7FFU)      
#define FMC_WRITE_END_8_yzsxADDR        ((uint32_t)0x0802BFFFU)      
#define FMC_WRITE_END_9_yzsxADDR        ((uint32_t)0x0802C7FFU)      
#define FMC_WRITE_END_10_yzsxADDR       ((uint32_t)0x0802CFFFU)      
#define FMC_WRITE_END_11_yzsxADDR       ((uint32_t)0x0802D7FFU)      
#define FMC_WRITE_END_12_yzsxADDR       ((uint32_t)0x0802DFFFU)      
#define FMC_WRITE_END_13_yzsxADDR       ((uint32_t)0x0802E7FFU)      
#define FMC_WRITE_END_14_yzsxADDR       ((uint32_t)0x0802EFFFU)      
#define FMC_WRITE_END_15_yzsxADDR       ((uint32_t)0x0802F7FFU)      
#define FMC_WRITE_END_16_yzsxADDR       ((uint32_t)0x0802FFFFU)      



 //记录阈值下限的起始地址：页96
#define FMC_WRITE_START_1_yzxxADDR      ((uint32_t)0x08030000U)    
#define FMC_WRITE_START_2_yzxxADDR      ((uint32_t)0x08030800U)    
#define FMC_WRITE_START_3_yzxxADDR      ((uint32_t)0x08031000U)    
#define FMC_WRITE_START_4_yzxxADDR      ((uint32_t)0x08031800U)    
#define FMC_WRITE_START_5_yzxxADDR      ((uint32_t)0x08032000U)    
#define FMC_WRITE_START_6_yzxxADDR      ((uint32_t)0x08032800U)    
#define FMC_WRITE_START_7_yzxxADDR      ((uint32_t)0x08033000U)    
#define FMC_WRITE_START_8_yzxxADDR      ((uint32_t)0x08033800U)    
#define FMC_WRITE_START_9_yzxxADDR      ((uint32_t)0x08034000U)    
#define FMC_WRITE_START_10_yzxxADDR     ((uint32_t)0x08034800U)    
#define FMC_WRITE_START_11_yzxxADDR     ((uint32_t)0x08035000U)    
#define FMC_WRITE_START_12_yzxxADDR     ((uint32_t)0x08035800U)    
#define FMC_WRITE_START_13_yzxxADDR     ((uint32_t)0x08036000U)    
#define FMC_WRITE_START_14_yzxxADDR     ((uint32_t)0x08036800U)    
#define FMC_WRITE_START_15_yzxxADDR     ((uint32_t)0x08037000U)    
#define FMC_WRITE_START_16_yzxxADDR     ((uint32_t)0x08037800U)    

#define FMC_WRITE_END_1_yzxxADDR        ((uint32_t)0x080307FFU)     
#define FMC_WRITE_END_2_yzxxADDR        ((uint32_t)0x08030FFFU)      
#define FMC_WRITE_END_3_yzxxADDR        ((uint32_t)0x080317FFU)     
#define FMC_WRITE_END_4_yzxxADDR        ((uint32_t)0x08031FFFU)     
#define FMC_WRITE_END_5_yzxxADDR        ((uint32_t)0x080327FFU)     
#define FMC_WRITE_END_6_yzxxADDR        ((uint32_t)0x08032FFFU)     
#define FMC_WRITE_END_7_yzxxADDR        ((uint32_t)0x080337FFU)     
#define FMC_WRITE_END_8_yzxxADDR        ((uint32_t)0x08033FFFU)     
#define FMC_WRITE_END_9_yzxxADDR        ((uint32_t)0x080347FFU)     
#define FMC_WRITE_END_10_yzxxADDR       ((uint32_t)0x08034FFFU)     
#define FMC_WRITE_END_11_yzxxADDR       ((uint32_t)0x080357FFU)     
#define FMC_WRITE_END_12_yzxxADDR       ((uint32_t)0x08035FFFU)     
#define FMC_WRITE_END_13_yzxxADDR       ((uint32_t)0x080367FFU)     
#define FMC_WRITE_END_14_yzxxADDR       ((uint32_t)0x08036FFFU)     
#define FMC_WRITE_END_15_yzxxADDR       ((uint32_t)0x080377FFU)     
#define FMC_WRITE_END_16_yzxxADDR       ((uint32_t)0x08037FFFU)     



 //记录DO逻辑的起始地址：页96
#define FMC_WRITE_START_1_DOADDR       ((uint32_t)0x08038000U)
#define FMC_WRITE_START_2_DOADDR       ((uint32_t)0x08038800U)    
#define FMC_WRITE_START_3_DOADDR       ((uint32_t)0x08039000U)    
#define FMC_WRITE_START_4_DOADDR       ((uint32_t)0x08039800U)    
#define FMC_WRITE_START_5_DOADDR       ((uint32_t)0x0803A000U)    
#define FMC_WRITE_START_6_DOADDR       ((uint32_t)0x0803A800U)    
#define FMC_WRITE_START_7_DOADDR       ((uint32_t)0x0803B000U)   
#define FMC_WRITE_START_8_DOADDR       ((uint32_t)0x0803B800U)    
#define FMC_WRITE_START_9_DOADDR       ((uint32_t)0x0803C000U)   
#define FMC_WRITE_START_10_DOADDR      ((uint32_t)0x0803C800U)    
#define FMC_WRITE_START_11_DOADDR      ((uint32_t)0x0803D000U)   
#define FMC_WRITE_START_12_DOADDR      ((uint32_t)0x0803D800U)    
#define FMC_WRITE_START_13_DOADDR      ((uint32_t)0x0803E000U)    
#define FMC_WRITE_START_14_DOADDR      ((uint32_t)0x0803E800U)   
#define FMC_WRITE_START_15_DOADDR      ((uint32_t)0x0803F000U)   
#define FMC_WRITE_START_16_DOADDR      ((uint32_t)0x0803F800U)  

#define FMC_WRITE_END_1_DOADDR        ((uint32_t)0x080387FFU)   
#define FMC_WRITE_END_2_DOADDR        ((uint32_t)0x08038FFFU)    
#define FMC_WRITE_END_3_DOADDR        ((uint32_t)0x080397FFU)    
#define FMC_WRITE_END_4_DOADDR        ((uint32_t)0x08039FFFU)    
#define FMC_WRITE_END_5_DOADDR        ((uint32_t)0x0803A7FFU)    
#define FMC_WRITE_END_6_DOADDR        ((uint32_t)0x0803AFFFU)    
#define FMC_WRITE_END_7_DOADDR        ((uint32_t)0x0803B7FFU)   
#define FMC_WRITE_END_8_DOADDR        ((uint32_t)0x0803BFFFU)    
#define FMC_WRITE_END_9_DOADDR        ((uint32_t)0x0803C7FFU)   
#define FMC_WRITE_END_10_DOADDR       ((uint32_t)0x0803CFFFU)    
#define FMC_WRITE_END_11_DOADDR       ((uint32_t)0x0803D7FFU)   
#define FMC_WRITE_END_12_DOADDR       ((uint32_t)0x0803DFFFU)    
#define FMC_WRITE_END_13_DOADDR       ((uint32_t)0x0803E7FFU)    
#define FMC_WRITE_END_14_DOADDR       ((uint32_t)0x0803EFFFU)   
#define FMC_WRITE_END_15_DOADDR       ((uint32_t)0x0803F7FFU)   
#define FMC_WRITE_END_16_DOADDR       ((uint32_t)0x0803FFFFU)    


////与或优先级关系
#define FMC_WRITE_START_BAUD1_ADDR       ((uint32_t)0x08040000U)    //208页
#define FMC_WRITE_START_BAUD2_ADDR       ((uint32_t)0x08040800U)    
//#define FMC_WRITE_START_YHY3_ADDR       ((uint32_t)0x08041000U)    
//#define FMC_WRITE_START_YHY4_ADDR       ((uint32_t)0x08041800U)    
//#define FMC_WRITE_START_YHY5_ADDR       ((uint32_t)0x08042000U)    
//#define FMC_WRITE_START_YHY6_ADDR       ((uint32_t)0x08042800U)    
//#define FMC_WRITE_START_YHY7_ADDR       ((uint32_t)0x08043000U)   
//#define FMC_WRITE_START_YHY8_ADDR       ((uint32_t)0x08043800U)    
//#define FMC_WRITE_START_YHY9_ADDR       ((uint32_t)0x08044000U)   
//#define FMC_WRITE_START_YHY10_ADDR      ((uint32_t)0x08044800U)    
//#define FMC_WRITE_START_YHY11_ADDR      ((uint32_t)0x08045000U)   
//#define FMC_WRITE_START_YHY12_ADDR      ((uint32_t)0x08045800U)    
//#define FMC_WRITE_START_YHY13_ADDR      ((uint32_t)0x08046000U)    
//#define FMC_WRITE_START_YHY14_ADDR      ((uint32_t)0x08046800U)   
//#define FMC_WRITE_START_YHY15_ADDR      ((uint32_t)0x08047000U)   
//#define FMC_WRITE_START_YHY16_ADDR      ((uint32_t)0x08047800U)    


#define FMC_WRITE_END_BAUD1_ADDR       ((uint32_t)0x080407FFU)    //208页
#define FMC_WRITE_END_BAUD2_ADDR       ((uint32_t)0x08040FFFU)    
//#define FMC_WRITE_END_YHY3_ADDR       ((uint32_t)0x080417FFU)    
//#define FMC_WRITE_END_YHY4_ADDR       ((uint32_t)0x08041FFFU)    
//#define FMC_WRITE_END_YHY5_ADDR       ((uint32_t)0x080427FFU)    
//#define FMC_WRITE_END_YHY6_ADDR       ((uint32_t)0x08042FFFU)    
//#define FMC_WRITE_END_YHY7_ADDR       ((uint32_t)0x080437FFU)   
//#define FMC_WRITE_END_YHY8_ADDR       ((uint32_t)0x08043FFFU)    
//#define FMC_WRITE_END_YHY9_ADDR       ((uint32_t)0x080447FFU)   
//#define FMC_WRITE_END_YHY10_ADDR      ((uint32_t)0x08044FFFU)    
//#define FMC_WRITE_END_YHY11_ADDR      ((uint32_t)0x080457FFU)   
//#define FMC_WRITE_END_YHY12_ADDR      ((uint32_t)0x08045FFFU)    
//#define FMC_WRITE_END_YHY13_ADDR      ((uint32_t)0x080467FFU)    
//#define FMC_WRITE_END_YHY14_ADDR      ((uint32_t)0x08046FFFU)   
//#define FMC_WRITE_END_YHY15_ADDR      ((uint32_t)0x080477FFU)   
//#define FMC_WRITE_END_YHY16_ADDR      ((uint32_t)0x08047FFFU)   


////DO通道逻辑配置开始地址                计算方法：31*2048-1
//#define FMC_WRITE_START_1_DOADDR        ((uint32_t)0x08048000U)   
//#define FMC_WRITE_START_2_DOADDR        ((uint32_t)0x08048800U) 
//#define FMC_WRITE_START_3_DOADDR        ((uint32_t)0x08049000U) 
//#define FMC_WRITE_START_4_DOADDR        ((uint32_t)0x08049800U) 
//#define FMC_WRITE_START_5_DOADDR        ((uint32_t)0x0804A000U) 
//#define FMC_WRITE_START_6_DOADDR        ((uint32_t)0x0804A800U) 
//#define FMC_WRITE_START_7_DOADDR        ((uint32_t)0x0804B000U) 
//#define FMC_WRITE_START_8_DOADDR        ((uint32_t)0x0804B800U) 
//#define FMC_WRITE_START_9_DOADDR        ((uint32_t)0x0804C000U) 
//#define FMC_WRITE_START_10_DOADDR       ((uint32_t)0x0804C800U) 
//#define FMC_WRITE_START_11_DOADDR       ((uint32_t)0x0804D000U) 
//#define FMC_WRITE_START_12_DOADDR       ((uint32_t)0x0804D800U) 
//#define FMC_WRITE_START_13_DOADDR       ((uint32_t)0x0802E000U) 
//#define FMC_WRITE_START_14_DOADDR       ((uint32_t)0x0802E800U) 
//#define FMC_WRITE_START_15_DOADDR       ((uint32_t)0x0802F000U) 
//#define FMC_WRITE_START_16_DOADDR       ((uint32_t)0x0802F800U) 



////DO通道逻辑配置结束地址                计算方法：31*2048-1
//#define FMC_WRITE_END_1_DOADDR        ((uint32_t)0x080487FFU)    //开关继电器
//#define FMC_WRITE_END_2_DOADDR        ((uint32_t)0x08048FFFU) 
//#define FMC_WRITE_END_3_DOADDR        ((uint32_t)0x080497FFU) 
//#define FMC_WRITE_END_4_DOADDR        ((uint32_t)0x08049FFFU) 
//#define FMC_WRITE_END_5_DOADDR        ((uint32_t)0x0804A7FFU) 
//#define FMC_WRITE_END_6_DOADDR        ((uint32_t)0x0804AFFFU) 
//#define FMC_WRITE_END_7_DOADDR        ((uint32_t)0x0804B7FFU) 
//#define FMC_WRITE_END_8_DOADDR        ((uint32_t)0x0804BFFFU) 
//#define FMC_WRITE_END_9_DOADDR        ((uint32_t)0x0804C7FFU) 
//#define FMC_WRITE_END_10_DOADDR       ((uint32_t)0x0804CFFFU) 
//#define FMC_WRITE_END_11_DOADDR       ((uint32_t)0x0804D7FFU) 
//#define FMC_WRITE_END_12_DOADDR       ((uint32_t)0x0804DFFFU) 
//#define FMC_WRITE_END_13_DOADDR       ((uint32_t)0x0804E7FFU) 
//#define FMC_WRITE_END_14_DOADDR       ((uint32_t)0x0804EFFFU) 
//#define FMC_WRITE_END_15_DOADDR       ((uint32_t)0x0804F7FFU) 
//#define FMC_WRITE_END_16_DOADDR       ((uint32_t)0x0804FFFFU) 


#define FMC_WRITE_START_485ADDR          ((uint32_t)0x08050000U)  
#define FMC_WRITE_END_485ADDR            ((uint32_t)0x080507FFU) 

// 工作模式
#define FMC_WRITE_START_WORKMODE     ((uint32_t)0x08050800U) 
#define FMC_WRITE_END_WORKMODE       ((uint32_t)0x08050FFFU) 
/************************ 变量定义 ************************/
extern uint16_t data1;
/************************ 函数定义 ************************/

void fmc_erase_pages(uint32_t start_addr);                                     // 擦除页
void fmc_program(uint32_t start_addr,uint32_t end_addr,uint32_t DATA);         // 写数据

void fmc_write_dataaddr(uint32_t staraddr,uint32_t endaddr,uint32_t dat);    // 写入设备各种信息 485地址 设备配置信息等
void fmc_write_DOdataAddr(uint32_t staraddr,uint32_t *data);                 // 写入DO配置信息的数组

#endif


/****************************End*****************************/

