/*
 * printf_retarget.c
 *
 *  Created on: 01.02.2021
 *      Author: Mati
 */


#include "usart.h"

/* (Re)Define stdio functionality, so printf would output to USART1 */
int __io_putchar(int ch) {
 uint8_t c[1];
 c[0] = ch & 0x00FF;
 HAL_UART_Transmit_DMA(&huart2, &c[0], 1);
 return ch;
}

int _write(int file,char *ptr, int len) {
 int DataIdx;
 for(DataIdx= 0; DataIdx< len; DataIdx++) {
  __io_putchar(*ptr++);
 }
 return len;
}
