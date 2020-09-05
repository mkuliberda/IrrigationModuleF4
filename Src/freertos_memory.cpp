/*
 * freertos_memory.cpp
 *
 *  Created on: 05.09.2020
 *      Author: Mati
 */

#include "freertos_memory.h"

/*inline void *operator new(size_t size)
{
	void *p;

	if(uxTaskGetNumberOfTasks())
		p=pvPortMalloc(size);
	else
		p=malloc(size);
	return p;
}

inline void operator delete(void *p) noexcept
{
	if(uxTaskGetNumberOfTasks())
		vPortFree( p );
	else
		free( p );
	p = NULL;
}*/

// Define the ‘new’ operator for C++ to use the freeRTOS memory management
// functions. THIS IS NOT OPTIONAL!
//
void *operator new(size_t size){
 void *p;

 if(uxTaskGetNumberOfTasks())
  p=pvPortMalloc(size);
 else
  p=malloc(size);

/*#ifdef __EXCEPTIONS


 if (p==0) // did pvPortMalloc succeed?
  throw std::bad_alloc(); // ANSI/ISO compliant behavior

#endif*/


 return p;
}

//
// Define the ‘delete’ operator for C++ to use the freeRTOS memory management
// functions. THIS IS NOT OPTIONAL!
//
void operator delete(void *p) noexcept
{

 if(uxTaskGetNumberOfTasks())
  vPortFree( p );
 else
  free( p );


 p = NULL;
}

void *operator new[](size_t size){
 void *p;

 if(uxTaskGetNumberOfTasks())
  p=pvPortMalloc(size);
 else
  p=malloc(size);


/*#ifdef __EXCEPTIONS


 if (p==0) // did pvPortMalloc succeed?
  throw std::bad_alloc(); // ANSI/ISO compliant behavior

#endif*/


 return p;
}

//
// Define the ‘delete’ operator for C++ to use the freeRTOS memory management
// functions. THIS IS NOT OPTIONAL!
//
void operator delete[](void *p) noexcept
{

 if(uxTaskGetNumberOfTasks())
  vPortFree( p );
 else
  free( p );


 p = NULL;
}

/* Optionally you can override the ‘nothrow’ versions as well.
   This is useful if you want to catch failed allocs with your
   own debug code, or keep track of heap usage for example,
   rather than just eliminate exceptions.
 */

/*inline void* operator new(std::sizet size, const std::nothrowt&) {
    return malloc(size);
}

inline void* operator new[](std::sizet size, const std::nothrowt&) {
    return malloc(size);
}

inline void operator delete(void* ptr, const std::nothrow_t&) noexcept{
    free(ptr);
}

inline void operator delete[](void* ptr, const std::nothrow_t&) noexcept{
    free(ptr);
}*/


