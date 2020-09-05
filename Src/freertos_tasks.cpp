
#include "freertos_tasks.h"
#include "freertos_memory.h"
#include "plants.h"


osThreadId defaultTaskHandle;
osThreadId SDCardTaskHandle;
osThreadId WirelessCommTaskHandle;
osThreadId IrrigationTaskHandle;
osMessageQId timestampQueueHandle;

osMailQDef(timestamp_box, 1, TimeStamp_t);
osMailQId timestamp_box;

void DefaultTask(void const * argument);
void SDCardTask(void const *argument);
void WirelessCommTask(void const *argument);
void IrrigationTask(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, DefaultTask, osPriorityIdle, 0, configMINIMAL_STACK_SIZE);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  osThreadDef(sdcardTask, SDCardTask, osPriorityNormal, 0, 20*configMINIMAL_STACK_SIZE);
  SDCardTaskHandle = osThreadCreate(osThread(sdcardTask), NULL);

  osThreadDef(wirelessTask, WirelessCommTask, osPriorityNormal, 0, 15*configMINIMAL_STACK_SIZE);
  WirelessCommTaskHandle = osThreadCreate(osThread(wirelessTask), NULL);

  osThreadDef(irrigationTask, IrrigationTask, osPriorityHigh, 0, 20*configMINIMAL_STACK_SIZE);
  IrrigationTaskHandle = osThreadCreate(osThread(irrigationTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void DefaultTask(void const * argument)
{
	RTC_TimeTypeDef rtc_time;
	RTC_DateTypeDef rtc_date;

	timestamp_box = osMailCreate(osMailQ(timestamp_box), NULL);
	TimeStamp_t *timestamp;
	timestamp = (TimeStamp_t*)osMailAlloc(timestamp_box, osWaitForever);

	for(;;)
	{
		HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
		timestamp->day = rtc_date.Date;
		timestamp->hours = rtc_time.Hours;
		timestamp->minutes = rtc_time.Minutes;
		timestamp->month = rtc_date.Month;
		timestamp->seconds = rtc_time.Seconds;
		timestamp->weekday = rtc_date.WeekDay;
		timestamp->year = rtc_date.Year;
		osMailPut(timestamp_box, timestamp);
		osDelay(1);
	}
}

void SDCardTask(void const *argument)
{

	osEvent evt;
	TimeStamp_t *timestamp;

	FATFS file_system;
    uint8_t fName[] = "testfile.txt\0";
    FIL file;
	uint8_t string[40] = "hello this is FREERTOS and FATFS\n";
	//FRESULT fR;
	UINT bytesCnt= 0;
    //BYTE work[_MAX_SS]; /* Work area (larger is better for processing time) */
	FIL schedule_file;
	char schedule_line[42] = ""; /* Line buffer */
	char test[1] = {8};
	std::string schedule;



    /* Format the default drive with default parameters */
    //fR = f_mkfs("", FM_FAT32, 4096, work, sizeof(work));
    //osDelay(1000);


	if(f_mount(&file_system, SDPath, 1) == FR_OK)
	{
		if(f_open(&file,(char *)fName, FA_WRITE | FA_OPEN_APPEND) == FR_OK)
		{
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
			if (f_write(&file, string, sizeof(string), &bytesCnt) != FR_OK){
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14, GPIO_PIN_RESET);
			}
			osDelay(10);
			while (f_close(&file) != FR_OK);
		}

	    /* Open a text file */
		if (f_open(&schedule_file, "sector1.txt", FA_READ) == FR_OK){
			/* Read every line and display it */
			while (f_gets(schedule_line, sizeof(schedule_line), &schedule_file)) {
				schedule.append(schedule_line);
			}
			osDelay(10);
			while (f_close(&schedule_file) != FR_OK);
		}
	}


    for( ;; )
    {
    	evt = osMailGet(timestamp_box, 10);
    	if (evt.status == osEventMail) {
    		timestamp = (TimeStamp_t*)evt.value.p;
        	osMailFree(timestamp_box, timestamp);
    	}

		if (f_open(&schedule_file, "sector1.txt", FA_READ) == FR_OK){
			/* Read every line and display it */
			while (f_gets(schedule_line, sizeof(schedule_line), &schedule_file)) {
				test[0] = schedule_line[0];
			}
			osDelay(10);
			while (f_close(&schedule_file) != FR_OK);
	    	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		}



    	osDelay(500);
    }
}


void WirelessCommTask(void const *argument){

	RTC_TimeTypeDef rtc_time;
	RTC_DateTypeDef rtc_date;
	rtc_time.Hours = 15;
	rtc_time.Minutes = 21;
	rtc_time.Seconds = 0;
	rtc_time.TimeFormat = 0;
	rtc_time.DayLightSaving = 0;
	rtc_date.WeekDay = RTC_WEEKDAY_SUNDAY;
	rtc_date.Date = 30;
	rtc_date.Month = 8;
	rtc_date.Year = 20;

	HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);


	for( ;; )
	{
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
		osDelay(20);
	}
}

void IrrigationTask(void const *argument){

	osEvent evt;
	TimeStamp_t *timestamp;
	plantstatus_s plant1 = {"KROTON", 0, 11.7};
	Plant *pelargonia = new Plant("Pelargonia", 0);

	std::string name = pelargonia->nameGet();

	for( ;; )
	{
    	evt = osMailGet(timestamp_box, 10);
    	if (evt.status == osEventMail) {
    		timestamp = (TimeStamp_t*)evt.value.p;
        	osMailFree(timestamp_box, timestamp);
    	}
    	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		osDelay(100);
	}
}



