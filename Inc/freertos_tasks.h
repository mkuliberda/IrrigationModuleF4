/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __freertos_tasks_H
#define __freertos_tasks_H


#include "scheduler.h"

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "adc.h"
#include "fatfs.h"
#include "iwdg.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "sd_diskio.h"
#include "stm32f4xx_hal_rtc.h"

struct activity_msg{
	uint8_t sector_nbr;
	activity_s activity;
};

struct exception_msg{
	uint8_t sector_nbr;
	exception_s exception;
};

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );
/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );
void MX_FREERTOS_Init(void);


#ifdef __cplusplus
}
#endif
#endif
