#ifndef GPS_H
#define GPS_H

#include "gps.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

void gps_setup(void);

#endif
