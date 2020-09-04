/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __freertos_memory_H
#define __freertos_memory_H

/*#ifdef __cplusplus
 extern "C" {
#endif*/

#include "cmsis_os.h"
#include "stdlib.h"

/*#ifdef __cplusplus
 }
#endif*/


void *operator new(size_t size)
{
	void *p;

	if(uxTaskGetNumberOfTasks())
		p=pvPortMalloc(size);
	else
		p=malloc(size);
	return p;
}

void operator delete(void *p)
{
	if(uxTaskGetNumberOfTasks())
		vPortFree( p );
	else
		free( p );
	p = NULL;
}

#endif //__freertos_memory_H
