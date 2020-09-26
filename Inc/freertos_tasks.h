/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __freertos_tasks_H
#define __freertos_tasks_H


#include "scheduler.h"
#include <algorithm>

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

#define LOG_TEXT_LEN 26

enum class reporter_t: uint8_t{
	sector1 		= 0x01,
	sector2 		= 0x02,
	sector3 		= 0x03,
	sector4 		= 0x04,
	sdcard_task 	= 0x05,
	irrigation_task = 0x06,
	wireless_task	= 0x07,
	sysmonitor_task = 0x08,
	anonymous		= 0xff
};

struct activity_msg{
	uint8_t sector_nbr;
	activity_s activity;
};

struct exception_msg{
	uint8_t sector_nbr;
	exception_s exception;
};

struct log_msg{
	char 		text[LOG_TEXT_LEN];
	uint8_t		len;
	reporter_t 	reporter_id;
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
