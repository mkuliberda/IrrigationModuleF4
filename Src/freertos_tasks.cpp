
#include "freertos_tasks.h"

#define REFRESH_RATE_SECONDS_OTHER 0.5
#define TASK_FREQ_MULTIPLIER 1000

osThreadId defaultTaskHandle;
osThreadId ledTaskHandle;

void StartDefaultTask(void const * argument);
void LedFlashTask(void const *argument);

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
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  osThreadDef(ledTask, LedFlashTask, osPriorityIdle, 0, 5*configMINIMAL_STACK_SIZE);
  ledTaskHandle = osThreadCreate(osThread(ledTask), NULL);


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
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

void LedFlashTask(void const *argument)
{

/*	BYTE lun = 0;
	BYTE cmd = GET_SECTOR_COUNT;
	DWORD buffer;
	SD_Driver.disk_initialize(lun);
	if (SD_Driver.disk_ioctl(lun, cmd, &buffer) == RES_OK){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	}
*/
	FATFS myfile;
    uint8_t fName[] = "testfile.txt\0";
    FIL file;
	uint8_t string[40] = "hello this is FREERTOS and FATFS Demo";
	//FRESULT fR;
	UINT bytesCnt= 0;

	if(f_mount(&myfile,SDPath,1) == FR_OK)
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		if(f_open(&file,(char *)fName, FA_WRITE|FA_CREATE_ALWAYS) == FR_OK)
		{
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);
			FRESULT fR = f_write(&file, string, sizeof(string), &bytesCnt);
			if (fR == FR_OK){
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
			}
			f_close(&file);

		}
	}
	else{
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
	}

    for( ;; )
    {
    	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
    	osDelay(500);
    }
}


