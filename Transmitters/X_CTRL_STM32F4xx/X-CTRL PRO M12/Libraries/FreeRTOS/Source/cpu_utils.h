/**
  ******************************************************************************
  * @file    cpu_utils.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Header for cpu_utils module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CPU_UTILS_H__
#define _CPU_UTILS_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define CALCULATION_PERIOD    1000
/* Exported functions ------------------------------------------------------- */
uint16_t FreeRTOS_GetCPUUsage (void);
#ifdef __cplusplus
}
#endif

#endif /* _CPU_UTILS_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
