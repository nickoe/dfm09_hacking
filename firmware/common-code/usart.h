#ifndef COMMON_USART_H
#define COMMON_USART_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>

int _write(int file, char *ptr, int len);
void stdio_setup(void);

#endif
