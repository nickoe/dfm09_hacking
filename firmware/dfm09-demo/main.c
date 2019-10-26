#include "clock_setup.h"
#include "gps.h"
#include "usart.h"
#include <errno.h>
#include <libopencm3/cm3/assert.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO13);

    // Modulation output
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO4);

    // RF Enable (PB12)
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO12);
    // gpio_set(GPIOB, GPIO12);
    gpio_clear(GPIOB, GPIO12);

    // 5.120 MHz ref clock for PLL (PB9)
    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_TIM4_CH4);

    rcc_periph_clock_enable(RCC_TIM4);
    timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_period(TIM4, 1);
    timer_set_oc_value(TIM4, TIM_OC4, 1);
    // timer_disable_preload(TIM4);
    timer_continuous_mode(TIM4);
    timer_enable_counter(TIM4);

    timer_set_oc_mode(TIM4, TIM_OC4, TIM_OCM_TOGGLE);
    timer_enable_oc_output(TIM4, TIM_OC4);
}

void delay(int max) {
    int i;
    for (i = 0; i < max; i++) { /* Wait a bit. */
        __asm__("nop");
    }
}

#define USE_HSE 0

#define DELAY_A 200000 / 6
#define DELAY_B 8000000 / 6

int main(void) {
    int i;
    // rcc_clock_setup_in_hse_10_24mhz_out_10_24mhz();
    rcc_clock_setup_in_hse_10_24mhz_out_20_48mhz();

    gpio_setup();
    gps_setup();
    stdio_setup();

    int j = 0;
    int c = 'K';

    while (1) {
#if 0
        // This does not work
        while( (char) (c & 0x00ff) != (char)'$') {
            c = usart_recv_blocking(USART2);
        }
        printf("Got start marker of message\r\n");
        printf("%c", c);

        while( (char)(c & 0x00ff) != (char)'*') {
            //c = usart_recv(USART2);
            c = usart_recv_blocking(USART2);
            printf("%c", c);
        }
        printf("\r\nGot end of message\r\n");
#else
        // This works
        c = usart_recv_blocking(USART2);
        printf("%c", c);
        gpio_toggle(GPIOA, GPIO4);
        // delay(230);
#endif
    }

    return 0;
}
