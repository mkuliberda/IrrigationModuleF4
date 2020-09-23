
#include <scheduler.h>
#include "freertos_tasks.h"
#include "freertos_memory.h"
#include "plants.h"


osThreadId defaultTaskHandle;
osThreadId SDCardTaskHandle;
osThreadId WirelessCommTaskHandle;
osThreadId IrrigationControlTaskHandle;
osMessageQId timestampQueueHandle;

//osMailQDef(timestamp_box, 1, TimeStamp_t);
//osMailQId timestamp_box;


void DefaultTask(void const * argument);
void SDCardTask(void const *argument);
void WirelessCommTask(void const *argument);
void IrrigationControlTask(void const *argument);

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

  osThreadDef(sdcardTask, SDCardTask, osPriorityNormal, 0, 150*configMINIMAL_STACK_SIZE);
  SDCardTaskHandle = osThreadCreate(osThread(sdcardTask), NULL);

  osThreadDef(wirelessTask, WirelessCommTask, osPriorityNormal, 0, 20*configMINIMAL_STACK_SIZE);
  WirelessCommTaskHandle = osThreadCreate(osThread(wirelessTask), NULL);

  osThreadDef(irrigationTask, IrrigationControlTask, osPriorityHigh, 0, 60*configMINIMAL_STACK_SIZE);
  IrrigationControlTaskHandle = osThreadCreate(osThread(irrigationTask), NULL);

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
	/*RTC_TimeTypeDef rtc_time;
	RTC_DateTypeDef rtc_date;

	timestamp_box = osMailCreate(osMailQ(timestamp_box), NULL);
	TimeStamp_t *timestamp;
	timestamp = (TimeStamp_t*)osMailAlloc(timestamp_box, osWaitForever);*/

	for(;;)
	{
		/*HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
		timestamp->day = rtc_date.Date;
		timestamp->hours = rtc_time.Hours;
		timestamp->minutes = rtc_time.Minutes;
		timestamp->month = rtc_date.Month;
		timestamp->seconds = rtc_time.Seconds;
		timestamp->weekday = rtc_date.WeekDay;
		timestamp->year = rtc_date.Year;
		osMailPut(timestamp_box, timestamp);*/
		osDelay(1);
	}
}

void SDCardTask(void const *argument)
{

	/*osEvent evt;
	TimeStamp_t *timestamp;*/
	RTC_TimeTypeDef rtc_time;
	RTC_DateTypeDef rtc_date;
	TimeStamp_t timestamp;

	std::string log_string = "System init...\n";
	std::string log_filename = "LOG.TXT";
    FIL log_file;
	FIL schedule_file;
	UINT bytesCnt= 0;
	char logical_drive[4];   /* SD logical drive path */
	FATFS file_system;    /* File system object for SD logical drive */
	DIR directory;
	FILINFO file_info;
	char cwd_buffer[80] = "/";

	std::array<std::string, 3> poss_sched_filename = {"SECTOR1.TXT", "SECTOR2.TXT", "SECTOR3.TXT"};
	Scheduler schedule[3] = {Scheduler("SECTOR1"), Scheduler("SECTOR2"), Scheduler("SECTOR3")};
	bool mount_success = false;


	if(f_mount(&file_system, logical_drive, 1) == FR_OK)
	{
		mount_success = true;
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		if(f_open(&log_file, log_filename.c_str(), FA_WRITE | FA_OPEN_APPEND) == FR_OK)
		{
			if (f_write(&log_file, log_string.c_str(), log_string.length(), &bytesCnt) != FR_OK){
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
			}
			osDelay(10);
			while (f_close(&log_file) != FR_OK);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		}

		if (f_opendir(&directory, cwd_buffer) == FR_OK){
			do{
				// Read an entry from the directory.
				// Check for error and break if there is a problem.
				if( f_readdir(&directory, &file_info) != FR_OK){
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
					break;
				}

				for(auto const &file: poss_sched_filename){

					std::string sdcard_sched_filename(file_info.fname);

					if (file == sdcard_sched_filename)
					{
						uint8_t sector_nbr = atoi(sdcard_sched_filename.substr(6,1).c_str()) - 1;
						/* Open a text file */
						if (f_open(&schedule_file, file_info.fname, FA_READ) == FR_OK)
						{
							char schedule_line[43] = "";
							while (f_gets(schedule_line, sizeof(schedule_line), &schedule_file)) {
								schedule[sector_nbr].addLine(schedule_line);
							}
							//osDelay(10); //TODO: check later if this delay is necessary
							while (f_close(&schedule_file) != FR_OK);
						}
						else{
							HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
						}
					}
				}
			// If the file name is blank, then this is the end of the listing.
			}while(file_info.fname[0]);
		}
	}


	log_string = "for loop\n";
    for( ;; )
    {
		HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
		timestamp.day = rtc_date.Date;
		timestamp.hours = rtc_time.Hours;
		timestamp.minutes = rtc_time.Minutes;
		timestamp.month = rtc_date.Month;
		timestamp.seconds = rtc_time.Seconds;
		timestamp.weekday = rtc_date.WeekDay;
		timestamp.year = rtc_date.Year;

		if(mount_success)
		{
			if(f_open(&log_file,log_filename.c_str(), FA_WRITE | FA_OPEN_APPEND) == FR_OK)
			{
				if (f_write(&log_file, log_string.c_str(), log_string.length(), &bytesCnt) == FR_OK){
					//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
				}
//				else{
//					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
//				}
				//osDelay(10);
				while (f_close(&log_file) != FR_OK);
			}
		}

		bool sector_watering = schedule[0].isActive(timestamp);


    	/*evt = osMailGet(timestamp_box, 10);
    	if (evt.status == osEventMail) {
    		timestamp = (TimeStamp_t*)evt.value.p;
        	osMailFree(timestamp_box, timestamp);
    	}*/
    	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
    	osDelay(500);
    }
}


void WirelessCommTask(void const *argument){

	RTC_TimeTypeDef rtc_time;
	RTC_DateTypeDef rtc_date;

	//TODO: set time based on wireless communication from external computer
	rtc_time.Hours = 18;
	rtc_time.Minutes = 59;
	rtc_time.Seconds = 50;
	rtc_time.TimeFormat = 0;
	rtc_time.DayLightSaving = 0;
	rtc_date.WeekDay = RTC_WEEKDAY_MONDAY;
	rtc_date.Date = 14;
	rtc_date.Month = 9;
	rtc_date.Year = 20;

	HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);


	for( ;; )
	{
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
		osDelay(20);
	}
}

void IrrigationControlTask(void const *argument){

	RTC_TimeTypeDef rtc_time;
	RTC_DateTypeDef rtc_date;
	TimeStamp_t timestamp;

	/*osEvent evt;
	TimeStamp_t *timestamp;*/
	//plantstatus_s plant1 = {"KROTON", 0, 11.7};
	Plant pelargonia("Pelargonia", 0);

	std::string name = pelargonia.nameGet();
	Scheduler schedule[3] = {Scheduler("SECTOR1"), Scheduler("SECTOR2"), Scheduler("SECTOR3")};
	bool sector_watering[3] = {false, false, false};



	for( ;; )
	{

		HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
		timestamp.day = rtc_date.Date;
		timestamp.hours = rtc_time.Hours;
		timestamp.minutes = rtc_time.Minutes;
		timestamp.month = rtc_date.Month;
		timestamp.seconds = rtc_time.Seconds;
		timestamp.weekday = rtc_date.WeekDay;
		timestamp.year = rtc_date.Year;

		sector_watering[0] = schedule[0].isActive(timestamp);

    	/*evt = osMailGet(timestamp_box, 10);
    	if (evt.status == osEventMail) {
    		timestamp = (TimeStamp_t*)evt.value.p;
        	osMailFree(timestamp_box, timestamp);
    	}*/
    	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		osDelay(100);
	}
}



