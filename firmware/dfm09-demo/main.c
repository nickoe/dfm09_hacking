#include <libopencm3/cm3/assert.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
//#include "uart.h"
#include <errno.h>
#include <stdio.h>
int _write(int file, char *ptr, int len);

static void my_rcc_clock_init(void) {
    /* Enable internal high-speed oscillator. */
    rcc_osc_on(RCC_HSI);
    rcc_wait_for_osc_ready(RCC_HSI);

    /* Select HSI as SYSCLK source. */
    rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSICLK);

    /* Enable external high-speed oscillator 8MHz. */
    rcc_osc_on(RCC_HSE);
    rcc_wait_for_osc_ready(RCC_HSE);
    rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSECLK);

    /*
     * Set prescalers for AHB, ADC, ABP1, ABP2.
     * Do this before touching the PLL (TODO: why?).
     */
    rcc_set_hpre(RCC_CFGR_HPRE_SYSCLK_NODIV);   /* Set. 24MHz Max. 72MHz */
    rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV2); /* Set. 12MHz Max. 14MHz */
    rcc_set_ppre1(RCC_CFGR_PPRE1_HCLK_NODIV);   /* Set. 24MHz Max. 36MHz */
    rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_NODIV);   /* Set. 24MHz Max. 72MHz */

    /*
     * Sysclk runs with 24MHz -> 0 waitstates.
     * 0WS from 0-24MHz
     * 1WS from 24-48MHz
     * 2WS from 48-72MHz
     */
    flash_set_ws(FLASH_ACR_LATENCY_0WS);

    /* Select HSE as SYSCLK source. */
    rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSECLK);

    /* Set the peripheral clock frequencies used */
    rcc_ahb_frequency = 10240000;
    rcc_apb1_frequency = 10240000;
    rcc_apb2_frequency = 10240000;
}

void rcc_clock_setup_in_hse_10_24mhz_out_20_48mhz(void) {
    /* Enable internal high-speed oscillator. */
    rcc_osc_on(RCC_HSI);
    rcc_wait_for_osc_ready(RCC_HSI);

    /* Select HSI as SYSCLK source. */
    rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSICLK);

    /* Enable external high-speed oscillator 8MHz. */
    rcc_osc_on(RCC_HSE);
    rcc_wait_for_osc_ready(RCC_HSE);
    rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSECLK);

    /*
     * Set prescalers for AHB, ADC, ABP1, ABP2.
     * Do this before touching the PLL (TODO: why?).
     */
    rcc_set_hpre(RCC_CFGR_HPRE_SYSCLK_NODIV);   /* Set. 24MHz Max. 72MHz */
    rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV2); /* Set. 12MHz Max. 14MHz */
    rcc_set_ppre1(RCC_CFGR_PPRE1_HCLK_NODIV);   /* Set. 24MHz Max. 36MHz */
    rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_NODIV);   /* Set. 24MHz Max. 72MHz */

    /*
     * Sysclk runs with 24MHz -> 0 waitstates.
     * 0WS from 0-24MHz
     * 1WS from 24-48MHz
     * 2WS from 48-72MHz
     */
    flash_set_ws(FLASH_ACR_LATENCY_0WS);

    /*
     * Set the PLL multiplication factor to 2.
     * 10.24MHz (external) * 2 (multiplier) = 20.48MHz
     */
    rcc_set_pll_multiplication_factor(RCC_CFGR_PLLMUL_PLL_CLK_MUL2);

    /* Select HSE as PLL source. */
    rcc_set_pll_source(RCC_CFGR_PLLSRC_HSE_CLK);

    /*
     * External frequency undivided before entering PLL
     * (only valid/needed for HSE).
     */
    rcc_set_pllxtpre(RCC_CFGR_PLLXTPRE_HSE_CLK);

    /* Enable PLL oscillator and wait for it to stabilize. */
    rcc_osc_on(RCC_PLL);
    rcc_wait_for_osc_ready(RCC_PLL);

    /* Select PLL as SYSCLK source. */
    rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_PLLCLK);

    /* Set the peripheral clock frequencies used */
    rcc_ahb_frequency = 20480000;
    rcc_apb1_frequency = 20480000;
    rcc_apb2_frequency = 20480000;
}

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO13);

    // Port A, used for USART
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_USART1);

    // Enable USART TX
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);
    // Enable USART RX
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO10);
    /* Setup UART parameters. */
    usart_set_baudrate(USART1, 115200);
    // usart_set_baudrate(USART1, 57600);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_set_mode(USART1, USART_MODE_TX);

    /* Finally enable the USART. */
    usart_enable(USART1);

    rcc_periph_clock_enable(RCC_USART2);

    // Enable USART TX
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2);
    // Enable USART RX
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO3);
    /* Setup UART parameters. */
    usart_set_baudrate(USART2, 9600);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
    usart_set_mode(USART2, USART_MODE_TX_RX);

    /* Finally enable the USART. */
    usart_enable(USART2);

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

int _write(int file, char *ptr, int len) {
    int i;

    if (file == 1) {
        for (i = 0; i < len; i++)
            usart_send_blocking(USART1, ptr[i]);
        return i;
    }

    errno = EIO;
    return -1;
}

int main(void) {
    int i;
    // my_rcc_clock_init();
    rcc_clock_setup_in_hse_10_24mhz_out_20_48mhz();
    gpio_setup();
    // stdout = usart_setup(USART1, 115200);
    int j = 0;
    int c = 'K';
    uint8_t sirfbinary2nmea[] = {
        0xa0, 0xa2, 0x00, 0x18, 0x81, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x12, 0xc0, 0x01, 0x6a, 0xb0, 0xb3};

    delay(DELAY_A * 50);

    usart_set_baudrate(USART2, 9600);
    usart_enable(USART2);
    for (j = 0; j < 6; j++) {
        printf(".\n");
        // delay(DELAY_A*50);
    }
    for (int n = 0; n < 32; n++) {
        usart_send_blocking(USART2, sirfbinary2nmea[n]);
    }
    usart_send_blocking(USART2, '\r');
    usart_send_blocking(USART2, '\n');
    usart_set_baudrate(USART2, 4800);

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
        //delay(230);
#endif
    }

    return 0;
}
