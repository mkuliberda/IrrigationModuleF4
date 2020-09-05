/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __freertos_memory_H
#define __freertos_memory_H


#include "cmsis_os.h"
#include "stdlib.h"


inline void *operator new(size_t size);
inline void *operator new[](size_t size);
inline void operator delete(void *p) noexcept;
inline void operator delete[](void *p) noexcept;

#endif //__freertos_memory_H
