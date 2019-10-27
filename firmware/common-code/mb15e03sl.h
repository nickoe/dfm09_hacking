#ifndef MB15E03SL_H
#define MB15E03SL_H

#include <stdlib.h>
#include <stdio.h>
#include <libopencm3/stm32/gpio.h>

#define NOP_DELAY() ({__asm__("nop"); __asm__("nop"); __asm__("nop");})


void pll_write(void);
void pll_setup(void);

#endif
