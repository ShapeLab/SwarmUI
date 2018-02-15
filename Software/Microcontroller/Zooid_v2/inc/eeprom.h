/**
  ******************************************************************************
  * @file    EEPROM_Emulation/inc/eeprom.h 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    29-January-2016
  * @brief   This file contains all the functions prototypes for the EEPROM 
  *          emulation firmware library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Exported constants --------------------------------------------------------*/

/* Base address of the Flash sectors */
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 2 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000800) /* Base @ of Page 1, 2 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08001000) /* Base @ of Page 2, 2 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08001800) /* Base @ of Page 3, 2 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08002000) /* Base @ of Page 4, 2 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x08002800) /* Base @ of Page 5, 2 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x08003000) /* Base @ of Page 6, 2 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x08003800) /* Base @ of Page 7, 2 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x08004000) /* Base @ of Page 8, 2 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x08004800) /* Base @ of Page 9, 2 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x08005000) /* Base @ of Page 10, 2 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x08005800) /* Base @ of Page 11, 2 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x08006000) /* Base @ of Page 12, 2 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x08006800) /* Base @ of Page 13, 2 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x08007000) /* Base @ of Page 14, 2 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x08007800) /* Base @ of Page 15, 2 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x08008000) /* Base @ of Page 16, 2 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x08008800) /* Base @ of Page 17, 2 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x08009000) /* Base @ of Page 18, 2 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x08009800) /* Base @ of Page 19, 2 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x0800A000) /* Base @ of Page 20, 2 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x0800A800) /* Base @ of Page 21, 2 Kbytes */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x0800B000) /* Base @ of Page 22, 2 Kbytes */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x0800B800) /* Base @ of Page 23, 2 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x0800C000) /* Base @ of Page 24, 2 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x0800C800) /* Base @ of Page 25, 2 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x0800D000) /* Base @ of Page 26, 2 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x0800D800) /* Base @ of Page 27, 2 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x0800E000) /* Base @ of Page 28, 2 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x0800E800) /* Base @ of Page 29, 2 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x0800F000) /* Base @ of Page 30, 2 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x0800F800) /* Base @ of Page 31, 2 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x08010000) /* Base @ of Page 32, 2 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x08010800) /* Base @ of Page 33, 2 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x08011000) /* Base @ of Page 34, 2 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x08011800) /* Base @ of Page 35, 2 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x08012000) /* Base @ of Page 36, 2 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x08012800) /* Base @ of Page 37, 2 Kbytes */
#define ADDR_FLASH_PAGE_38    ((uint32_t)0x08013000) /* Base @ of Page 38, 2 Kbytes */
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x08013800) /* Base @ of Page 39, 2 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x08014000) /* Base @ of Page 40, 2 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x08014800) /* Base @ of Page 41, 2 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x08015000) /* Base @ of Page 42, 2 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x08015800) /* Base @ of Page 43, 2 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x08016000) /* Base @ of Page 44, 2 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x08016800) /* Base @ of Page 45, 2 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x08017000) /* Base @ of Page 46, 2 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x08017800) /* Base @ of Page 47, 2 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x08018000) /* Base @ of Page 48, 2 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x08018800) /* Base @ of Page 49, 2 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x08019000) /* Base @ of Page 50, 2 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x08019800) /* Base @ of Page 51, 2 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x0801A000) /* Base @ of Page 52, 2 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x0801A800) /* Base @ of Page 53, 2 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x0801B000) /* Base @ of Page 54, 2 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x0801B800) /* Base @ of Page 55, 2 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x0801C000) /* Base @ of Page 56, 2 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x0801C800) /* Base @ of Page 57, 2 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0801D000) /* Base @ of Page 58, 2 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0801D800) /* Base @ of Page 59, 2 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0801E000) /* Base @ of Page 60, 2 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0801E800) /* Base @ of Page 61, 2 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0801F000) /* Base @ of Page 62, 2 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0801F800) /* Base @ of Page 63, 2 Kbytes */
#define ADDR_FLASH_PAGE_64    ((uint32_t)0x08020000) /* Base @ of Page 64, 2 Kbytes */
#define ADDR_FLASH_PAGE_65    ((uint32_t)0x08020800) /* Base @ of Page 65, 2 Kbytes */
#define ADDR_FLASH_PAGE_66    ((uint32_t)0x08021000) /* Base @ of Page 66, 2 Kbytes */
#define ADDR_FLASH_PAGE_67    ((uint32_t)0x08021800) /* Base @ of Page 67, 2 Kbytes */
#define ADDR_FLASH_PAGE_68    ((uint32_t)0x08022000) /* Base @ of Page 68, 2 Kbytes */
#define ADDR_FLASH_PAGE_69    ((uint32_t)0x08022800) /* Base @ of Page 69, 2 Kbytes */
#define ADDR_FLASH_PAGE_70    ((uint32_t)0x08023000) /* Base @ of Page 70, 2 Kbytes */
#define ADDR_FLASH_PAGE_71    ((uint32_t)0x08023800) /* Base @ of Page 71, 2 Kbytes */
#define ADDR_FLASH_PAGE_72    ((uint32_t)0x08024000) /* Base @ of Page 72, 2 Kbytes */
#define ADDR_FLASH_PAGE_73    ((uint32_t)0x08024800) /* Base @ of Page 73, 2 Kbytes */
#define ADDR_FLASH_PAGE_74    ((uint32_t)0x08025000) /* Base @ of Page 74, 2 Kbytes */
#define ADDR_FLASH_PAGE_75    ((uint32_t)0x08025800) /* Base @ of Page 75, 2 Kbytes */
#define ADDR_FLASH_PAGE_76    ((uint32_t)0x08026000) /* Base @ of Page 76, 2 Kbytes */
#define ADDR_FLASH_PAGE_77    ((uint32_t)0x08026800) /* Base @ of Page 77, 2 Kbytes */
#define ADDR_FLASH_PAGE_78    ((uint32_t)0x08027000) /* Base @ of Page 78, 2 Kbytes */
#define ADDR_FLASH_PAGE_79    ((uint32_t)0x08027800) /* Base @ of Page 79, 2 Kbytes */
#define ADDR_FLASH_PAGE_80    ((uint32_t)0x08028000) /* Base @ of Page 80, 2 Kbytes */
#define ADDR_FLASH_PAGE_81    ((uint32_t)0x08028800) /* Base @ of Page 81, 2 Kbytes */
#define ADDR_FLASH_PAGE_82    ((uint32_t)0x08029000) /* Base @ of Page 82, 2 Kbytes */
#define ADDR_FLASH_PAGE_83    ((uint32_t)0x08029800) /* Base @ of Page 83, 2 Kbytes */
#define ADDR_FLASH_PAGE_84    ((uint32_t)0x0802A000) /* Base @ of Page 84, 2 Kbytes */
#define ADDR_FLASH_PAGE_85    ((uint32_t)0x0802A800) /* Base @ of Page 85, 2 Kbytes */
#define ADDR_FLASH_PAGE_86    ((uint32_t)0x0802B000) /* Base @ of Page 86, 2 Kbytes */
#define ADDR_FLASH_PAGE_87    ((uint32_t)0x0802B800) /* Base @ of Page 87, 2 Kbytes */
#define ADDR_FLASH_PAGE_88    ((uint32_t)0x0802C000) /* Base @ of Page 88, 2 Kbytes */
#define ADDR_FLASH_PAGE_89    ((uint32_t)0x0802C800) /* Base @ of Page 89, 2 Kbytes */
#define ADDR_FLASH_PAGE_90    ((uint32_t)0x0802D000) /* Base @ of Page 90, 2 Kbytes */
#define ADDR_FLASH_PAGE_91    ((uint32_t)0x0802D800) /* Base @ of Page 91, 2 Kbytes */
#define ADDR_FLASH_PAGE_92    ((uint32_t)0x0802E000) /* Base @ of Page 92, 2 Kbytes */
#define ADDR_FLASH_PAGE_93    ((uint32_t)0x0802E800) /* Base @ of Page 93, 2 Kbytes */
#define ADDR_FLASH_PAGE_94    ((uint32_t)0x0802F000) /* Base @ of Page 94, 2 Kbytes */
#define ADDR_FLASH_PAGE_95    ((uint32_t)0x0802F800) /* Base @ of Page 95, 2 Kbytes */
#define ADDR_FLASH_PAGE_96    ((uint32_t)0x08030000) /* Base @ of Page 96, 2 Kbytes */
#define ADDR_FLASH_PAGE_97    ((uint32_t)0x08030800) /* Base @ of Page 97, 2 Kbytes */
#define ADDR_FLASH_PAGE_98    ((uint32_t)0x08031000) /* Base @ of Page 98, 2 Kbytes */
#define ADDR_FLASH_PAGE_99    ((uint32_t)0x08031800) /* Base @ of Page 99, 2 Kbytes */
#define ADDR_FLASH_PAGE_100   ((uint32_t)0x08032000) /* Base @ of Page 100, 2 Kbytes */
#define ADDR_FLASH_PAGE_101   ((uint32_t)0x08032800) /* Base @ of Page 101, 2 Kbytes */
#define ADDR_FLASH_PAGE_102   ((uint32_t)0x08033000) /* Base @ of Page 102, 2 Kbytes */
#define ADDR_FLASH_PAGE_103   ((uint32_t)0x08033800) /* Base @ of Page 103, 2 Kbytes */
#define ADDR_FLASH_PAGE_104   ((uint32_t)0x08034000) /* Base @ of Page 104, 2 Kbytes */
#define ADDR_FLASH_PAGE_105   ((uint32_t)0x08034800) /* Base @ of Page 105, 2 Kbytes */
#define ADDR_FLASH_PAGE_106   ((uint32_t)0x08035000) /* Base @ of Page 106, 2 Kbytes */
#define ADDR_FLASH_PAGE_107   ((uint32_t)0x08035800) /* Base @ of Page 107, 2 Kbytes */
#define ADDR_FLASH_PAGE_108   ((uint32_t)0x08036000) /* Base @ of Page 108, 2 Kbytes */
#define ADDR_FLASH_PAGE_109   ((uint32_t)0x08036800) /* Base @ of Page 109, 2 Kbytes */
#define ADDR_FLASH_PAGE_110   ((uint32_t)0x08037000) /* Base @ of Page 110, 2 Kbytes */
#define ADDR_FLASH_PAGE_111   ((uint32_t)0x08037800) /* Base @ of Page 111, 2 Kbytes */
#define ADDR_FLASH_PAGE_112   ((uint32_t)0x08038000) /* Base @ of Page 112, 2 Kbytes */
#define ADDR_FLASH_PAGE_113   ((uint32_t)0x08038800) /* Base @ of Page 113, 2 Kbytes */
#define ADDR_FLASH_PAGE_114   ((uint32_t)0x08039000) /* Base @ of Page 114, 2 Kbytes */
#define ADDR_FLASH_PAGE_115   ((uint32_t)0x08039800) /* Base @ of Page 115, 2 Kbytes */
#define ADDR_FLASH_PAGE_116   ((uint32_t)0x0803A000) /* Base @ of Page 116, 2 Kbytes */
#define ADDR_FLASH_PAGE_117   ((uint32_t)0x0803A800) /* Base @ of Page 117, 2 Kbytes */
#define ADDR_FLASH_PAGE_118   ((uint32_t)0x0803B000) /* Base @ of Page 118, 2 Kbytes */
#define ADDR_FLASH_PAGE_119   ((uint32_t)0x0803B800) /* Base @ of Page 119, 2 Kbytes */
#define ADDR_FLASH_PAGE_120   ((uint32_t)0x0803C000) /* Base @ of Page 120, 2 Kbytes */
#define ADDR_FLASH_PAGE_121   ((uint32_t)0x0803C800) /* Base @ of Page 121, 2 Kbytes */
#define ADDR_FLASH_PAGE_122   ((uint32_t)0x0803D000) /* Base @ of Page 122, 2 Kbytes */
#define ADDR_FLASH_PAGE_123   ((uint32_t)0x0803D800) /* Base @ of Page 123, 2 Kbytes */
#define ADDR_FLASH_PAGE_124   ((uint32_t)0x0803E000) /* Base @ of Page 124, 2 Kbytes */
#define ADDR_FLASH_PAGE_125   ((uint32_t)0x0803E800) /* Base @ of Page 125, 2 Kbytes */
#define ADDR_FLASH_PAGE_126   ((uint32_t)0x0803F000) /* Base @ of Page 126, 2 Kbytes */
#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0803F800) /* Base @ of Page 127, 2 Kbytes */

/* Define the size of the sectors to be used */
#define PAGE_SIZE               (uint32_t)FLASH_PAGE_SIZE  /* Page size */

///* EEPROM start address in Flash */
//#define EEPROM_START_ADDRESS  ((uint32_t)ADDR_FLASH_PAGE_16) /* EEPROM emulation start address */
//
///* Pages 0 and 1 base and end addresses */
//#define PAGE0_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + 0x0000))
//#define PAGE0_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))
//
//#define PAGE1_BASE_ADDRESS    ((uint32_t)(ADDR_FLASH_PAGE_48))
//#define PAGE1_END_ADDRESS     ((uint32_t)(ADDR_FLASH_PAGE_48 + PAGE_SIZE - 1))
//
///* Used Flash pages for EEPROM emulation */
//#define PAGE0                 ((uint16_t)0x0000)
//#define PAGE1                 ((uint16_t)32) /* Page nb between PAGE0_BASE_ADDRESS & PAGE1_BASE_ADDRESS*/


/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS  ((uint32_t)ADDR_FLASH_PAGE_20) /* EEPROM emulation start address */

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + 0x0000))
#define PAGE0_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS    ((uint32_t)(ADDR_FLASH_PAGE_22))
#define PAGE1_END_ADDRESS     ((uint32_t)(ADDR_FLASH_PAGE_22 + PAGE_SIZE - 1))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                 ((uint16_t)0x0000)
#define PAGE1                 ((uint16_t)2) /* Page nb between PAGE0_BASE_ADDRESS & PAGE1_BASE_ADDRESS*/



/* No valid page define */
#define NO_VALID_PAGE         ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                ((uint16_t)0xFFFF)     /* Page is empty */
#define RECEIVE_DATA          ((uint16_t)0xEEEE)     /* Page is marked to receive data */
#define VALID_PAGE            ((uint16_t)0x0000)     /* Page containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE  ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE   ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL             ((uint8_t)0x80)

/* Variables' number */
#define NB_OF_VAR             ((uint8_t)0x03)

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint16_t EE_Init(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

#endif /* __EEPROM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
