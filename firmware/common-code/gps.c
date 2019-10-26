#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include "gps.h"

static void delay(int max) {
    int i;
    for (i = 0; i < max; i++) { /* Wait a bit. */
        __asm__("nop");
    }
}


static void nmea_setup(void) {

    uint8_t sirfbinary2nmea[] = {
        0xa0, 0xa2, 0x00, 0x18, 0x81, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x12, 0xc0, 0x01, 0x6a, 0xb0, 0xb3};

    usart_set_baudrate(USART2, 9600);
    usart_enable(USART2);
    delay(200000*50);
    for (int n = 0; n < 32; n++) {
        usart_send_blocking(USART2, sirfbinary2nmea[n]);
    }
    usart_send_blocking(USART2, '\r');
    usart_send_blocking(USART2, '\n');
    usart_set_baudrate(USART2, 4800);
}

void gps_setup(void) {
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

    nmea_setup();
}

