/**
  ******************************************************************************
  * @file    cpu_utils.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Utilities for CPU management
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

/********************** IMPORTANT **********************************************
To use this module, the following steps should be followed :

1- in the FreeRTOSConfig.h file enable the following macros :
      - #define configUSE_IDLE_HOOK	        1
      - #define configUSE_TICK_HOOK	        1

2- in the FreeRTOSConfig.h define the following macros :
      - #define traceTASK_SWITCHED_IN()  extern void StartIdleMonitor(void); \
                                         StartIdleMonitor()
      - #define traceTASK_SWITCHED_OUT() extern void EndIdleMonitor(void); \
                                         EndIdleMonitor()
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "cpu_utils.h"
#include "delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

xTaskHandle                        xIdleHandle = NULL;
volatile  uint32_t                 FreeRTOS_CPU_Usage = 0;
uint32_t                           FreeRTOS_CPU_IdleStartTime = 0;
uint32_t                           FreeRTOS_CPU_IdleSpentTime = 0;
uint32_t                           FreeRTOS_CPU_TotalIdleTime = 0;

/* Private functions ---------------------------------------------------------*/
/**
* @brief  Application Idle Hook
* @param  None
* @retval None
*/

void vApplicationIdleHook(void)
{
    if( xIdleHandle == NULL )
    {
        /* Store the handle to the idle task. */
        xIdleHandle = xTaskGetCurrentTaskHandle();
    }
}

/**
* @brief  Application Idle Hook
* @param  None
* @retval None
*/
void vApplicationTickHook (void)
{
    extern void SystemMsInc(void);
    SystemMsInc();
    static int tick = 0;

    if(tick ++ > CALCULATION_PERIOD)
    {
        tick = 0;

        if(FreeRTOS_CPU_TotalIdleTime > 1000)
        {
            FreeRTOS_CPU_TotalIdleTime = 1000;
        }
        FreeRTOS_CPU_Usage = (100 - (FreeRTOS_CPU_TotalIdleTime * 100) / CALCULATION_PERIOD);
        FreeRTOS_CPU_TotalIdleTime = 0;
    }
}

/**
* @brief  Start Idle monitor
* @param  None
* @retval None
*/
void StartIdleMonitor (void)
{
    if( xTaskGetCurrentTaskHandle() == xIdleHandle )
    {
        FreeRTOS_CPU_IdleStartTime = xTaskGetTickCount();
    }
}

/**
* @brief  Stop Idle monitor
* @param  None
* @retval None
*/
void EndIdleMonitor (void)
{
    if( xTaskGetCurrentTaskHandle() == xIdleHandle )
    {
        /* Store the handle to the idle task. */
        FreeRTOS_CPU_IdleSpentTime = xTaskGetTickCount() - FreeRTOS_CPU_IdleStartTime;
        FreeRTOS_CPU_TotalIdleTime += FreeRTOS_CPU_IdleSpentTime;
    }
}

/**
* @brief  Stop Idle monitor
* @param  None
* @retval None
*/
uint16_t FreeRTOS_GetCPUUsage (void)
{
    return (uint16_t)FreeRTOS_CPU_Usage;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
