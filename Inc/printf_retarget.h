/*
 * printf_retarget.h
 *
 *  Created on: 01.02.2021
 *      Author: Mati
 */

#ifndef PRINTF_RETARGET_H_
#define PRINTF_RETARGET_H_

int __io_putchar(int ch);
int _write(int file,char *ptr, int len);

#endif /* PRINTF_RETARGET_H_ */
