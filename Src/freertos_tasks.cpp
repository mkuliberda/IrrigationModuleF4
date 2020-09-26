
#include "freertos_tasks.h"
#include "freertos_memory.h"
#include "plants.h"


osThreadId SysMonitorTaskHandle;
osThreadId SDCardTaskHandle;
osThreadId WirelessCommTaskHandle;
osThreadId IrrigationControlTaskHandle;
//osMessageQId timestampQueueHandle;
//osMailQDef(timestamp_box, 1, TimeStamp_t);
//osMailQId timestamp_box;

osMailQId activities_box;
osMailQDef(activities_box, 42, activity_msg);
osMailQId exceptions_box;
osMailQDef(exceptions_box, 6, exception_msg);
osMailQId sys_logs_box;
osMailQDef(sys_logs_box, 5, log_msg);
osMailQId irg_logs_box;
osMailQDef(irg_logs_box, 5, log_msg);
osMailQId wls_logs_box;
osMailQDef(wls_logs_box, 5, log_msg);


void SysMonitorTask(void const * argument);
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

//bool retreiveScheduleMsg(T1 mail_box, T2 *event, T3 *msg)
//{
//	do{
//		evt = osMailGet(mail_box, 1);
//		if (event.status == osEventMail){
//			msg = (activity_msg*)event.value.p;
//			switch (msg->sector_nbr){
//			case 0:
//				sector_schedule[0].addActivity(msg->activity);
//				break;
//			case 1:
//				sector_schedule[1].addActivity(activity->activity);
//				break;
//			case 2:
//				sector_schedule[2].addActivity(activity->activity);
//				break;
//			default:
//				break;
//			}
//		}
//		osMailFree(activities_box, activity);
//	}while(evt.status == osEventMail);
//    return true;
//}
//
//template bool receiveScheduleMsg<osMailQId, activity_msg>

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
  osThreadDef(sysmonitorTask, SysMonitorTask, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE);
  SysMonitorTaskHandle = osThreadCreate(osThread(sysmonitorTask), NULL);

  osThreadDef(sdcardTask, SDCardTask, osPriorityNormal, 0, 120*configMINIMAL_STACK_SIZE);
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
void SysMonitorTask(void const * argument)
{
	size_t min_heap_size;
	std::string log_string = "Sys monitor task started\n";
	if (log_string.length() > LOG_TEXT_LEN){
		log_string = log_string.substr(log_string.length() - LOG_TEXT_LEN, LOG_TEXT_LEN);
	}
	log_msg *message;
	sys_logs_box = osMailCreate(osMailQ(sys_logs_box), osThreadGetId());
	message = (log_msg*)osMailAlloc(sys_logs_box, osWaitForever);
	message->reporter_id = reporter_t::sysmonitor_task;
	message->len = log_string.length();
	log_string.copy(message->text, std::min((uint8_t)log_string.length(), (uint8_t)LOG_TEXT_LEN), 0);
	while (osMailPut(sys_logs_box, message) != osOK);

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
    	min_heap_size = xPortGetMinimumEverFreeHeapSize();
		osDelay(1000);
	}
}

void SDCardTask(void const *argument)
{

	RTC_TimeTypeDef rtc_time;
	RTC_DateTypeDef rtc_date;
	TimeStamp_t timestamp;

	const char log_filename[] = "LOG.TXT";
    FIL log_file;
	FIL schedule_file;
	UINT bytesCnt= 0;
	char logical_drive[4];   /* SD logical drive path */
	FATFS file_system;    /* File system object for SD logical drive */
	DIR directory;
	FILINFO file_info;
	char cwd_buffer[80] = "/";

	const std::array<std::string, 4> poss_sched_filename = {"SECTOR1.TXT", "SECTOR2.TXT", "SECTOR3.TXT",  "SECTOR4.TXT"};
	Scheduler schedule = Scheduler("PARSER");
	bool mount_success = false;
	activities_box = osMailCreate(osMailQ(activities_box), osThreadGetId());
	activity_msg *activity;
	exceptions_box = osMailCreate(osMailQ(exceptions_box), osThreadGetId());
	exception_msg *exception;
	osEvent evt;
	log_msg *sys_message;
	log_msg *irg_message;
	log_msg *wls_message;


	if(f_mount(&file_system, logical_drive, 1) == FR_OK){
		mount_success = true;
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		if(f_open(&log_file, log_filename, FA_WRITE | FA_OPEN_APPEND) == FR_OK){
			char log_str[] = "SDCard mount successful\n";
			if (f_write(&log_file, log_str, sizeof(log_str), &bytesCnt) != FR_OK){
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
			}
			while (f_close(&log_file) != FR_OK);
		}
		else{
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

					if (file == sdcard_sched_filename){
						uint8_t sector_nbr = atoi(sdcard_sched_filename.substr(6,1).c_str()) - 1;
						/* Open a text file */
						if (f_open(&schedule_file, file_info.fname, FA_READ) == FR_OK){
							char schedule_line[43] = "";
							while (f_gets(schedule_line, sizeof(schedule_line), &schedule_file)){
								if(schedule_line[0] == 'A'){
									activity = (activity_msg*)osMailAlloc(activities_box, osWaitForever);
									activity->sector_nbr = sector_nbr;
									activity->activity = schedule.parseActivity(schedule_line);
									while (osMailPut(activities_box, activity) != osOK);
								}
								else if(schedule_line[0] == 'E'){
									exception = (exception_msg*)osMailAlloc(exceptions_box, osWaitForever);
									exception->sector_nbr = sector_nbr;
									exception->exception = schedule.parseException(schedule_line);
									while (osMailPut(exceptions_box, exception) != osOK);
								}
							}
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

		if(mount_success){
			do{
				evt = osMailGet(sys_logs_box, 1);
				if (evt.status == osEventMail){
					sys_message = (log_msg*)evt.value.p;
					if(f_open(&log_file, log_filename, FA_WRITE | FA_OPEN_APPEND) == FR_OK){
						f_write(&log_file, sys_message->text, sys_message->len, &bytesCnt);
						while (f_close(&log_file) != FR_OK);
					}
				}
				osMailFree(sys_logs_box, irg_message);
			}while(evt.status == osEventMail);

			do{
				evt = osMailGet(irg_logs_box, 1);
				if (evt.status == osEventMail){
					irg_message = (log_msg*)evt.value.p;
					if(f_open(&log_file, log_filename, FA_WRITE | FA_OPEN_APPEND) == FR_OK){
						f_write(&log_file, irg_message->text, irg_message->len, &bytesCnt);
						while (f_close(&log_file) != FR_OK);
					}
				}
				osMailFree(irg_logs_box, irg_message);
			}while(evt.status == osEventMail);

			do{
				evt = osMailGet(wls_logs_box, 1);
				if (evt.status == osEventMail){
					wls_message = (log_msg*)evt.value.p;
					if(f_open(&log_file, log_filename, FA_WRITE | FA_OPEN_APPEND) == FR_OK){
						f_write(&log_file, wls_message->text, wls_message->len, &bytesCnt);
						while (f_close(&log_file) != FR_OK);
					}
				}
				osMailFree(wls_logs_box, wls_message);
			}while(evt.status == osEventMail);
		}

		//bool sector_watering = schedule[0].isActive(timestamp);

    	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
    	osDelay(500);
    }
}


void WirelessCommTask(void const *argument)
{
	std::string log_string = "Wireless Comm task started\n";
	if (log_string.length() > LOG_TEXT_LEN){
		log_string = log_string.substr(log_string.length() - LOG_TEXT_LEN, LOG_TEXT_LEN);
	}
	log_msg *message;
	wls_logs_box = osMailCreate(osMailQ(wls_logs_box), osThreadGetId());
	message = (log_msg*)osMailAlloc(wls_logs_box, osWaitForever);
	message->reporter_id = reporter_t::wireless_task;
	message->len = log_string.length();
	log_string.copy(message->text, std::min((uint8_t)log_string.length(), (uint8_t)LOG_TEXT_LEN), 0);
	while (osMailPut(wls_logs_box, message) != osOK);


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

	std::string log_string = "Irrigation Ctrl task started\n";
	if (log_string.length() > LOG_TEXT_LEN){
		log_string = log_string.substr(log_string.length() - LOG_TEXT_LEN, LOG_TEXT_LEN);
	}
	log_msg *message;
	irg_logs_box = osMailCreate(osMailQ(irg_logs_box), osThreadGetId());
	message = (log_msg*)osMailAlloc(irg_logs_box, osWaitForever);
	message->reporter_id = reporter_t::irrigation_task;
	message->len = log_string.length();
	log_string.copy(message->text, std::min((uint8_t)log_string.length(), (uint8_t)LOG_TEXT_LEN), 0);
	//TODO: Logger class: message->len = logger.prepareMessage("Irrigation Ctrl task started\n", message->text, LOG_TEXT_LEN);
	while (osMailPut(irg_logs_box, message) != osOK);

	RTC_TimeTypeDef rtc_time;
	RTC_DateTypeDef rtc_date;
	TimeStamp_t timestamp;

	osEvent evt;
	activity_msg *activity;
	exception_msg *exception;
	Scheduler sector_schedule[3] = {Scheduler("SECTOR1"), Scheduler("SECTOR2"), Scheduler("SECTOR3")};
	uint8_t activities_cnt[3]={0,0,0};
	uint8_t exceptions_cnt[3]={0,0,0};

	Plant pelargonia("Pelargonia", 0);
	std::string name = pelargonia.nameGet();

	osDelay(1000);  //leave some time for read from SDCard
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

	for( ;; )
	{

		//TODO: place msg retrieval into a function
		do{
			evt = osMailGet(activities_box, 1);
			if (evt.status == osEventMail){
				activity = (activity_msg*)evt.value.p;
				switch (activity->sector_nbr){
				case 0:
					sector_schedule[0].addActivity(activity->activity);
					break;
				case 1:
					sector_schedule[1].addActivity(activity->activity);
					break;
				case 2:
					sector_schedule[2].addActivity(activity->activity);
					break;
				default:
					break;
				}
			}
			osMailFree(activities_box, activity);
		}while(evt.status == osEventMail);


		do{
			evt = osMailGet(exceptions_box, 1);
			if (evt.status == osEventMail){
				exception = (exception_msg*)evt.value.p;
				switch (exception->sector_nbr){
				case 0:
					sector_schedule[0].addException(exception->exception);
					break;
				case 1:
					sector_schedule[1].addException(exception->exception);
					break;
				case 2:
					sector_schedule[2].addException(exception->exception);
					break;
				default:
					break;
				}
			}
			osMailFree(exceptions_box, exception);
		}while(evt.status == osEventMail);


		HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
		timestamp.day = rtc_date.Date;
		timestamp.hours = rtc_time.Hours;
		timestamp.minutes = rtc_time.Minutes;
		timestamp.month = rtc_date.Month;
		timestamp.seconds = rtc_time.Seconds;
		timestamp.weekday = rtc_date.WeekDay;
		timestamp.year = rtc_date.Year;

		if (sector_schedule[0].isActive(timestamp)){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		}
		else{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		}
		activities_cnt[0] = sector_schedule[0].getActivitiesCount();
		activities_cnt[1] = sector_schedule[1].getActivitiesCount();
		activities_cnt[2] = sector_schedule[2].getActivitiesCount();
		exceptions_cnt[0] = sector_schedule[0].getExceptionsCount();
		exceptions_cnt[1] = sector_schedule[1].getExceptionsCount();
		exceptions_cnt[2] = sector_schedule[2].getExceptionsCount();

		//Placeholder for rest of the code

    	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		osDelay(100);
	}
}




