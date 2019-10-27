#include "mb15e03sl.h"
#include "usart.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>
#include <stdlib.h>

#define PLL_DEBUG 1

#if 0
static void delay(int max) {
    int i;
    for (i = 0; i < max; i++) { /* Wait a bit. */
        __asm__("nop");
    }
}
#endif

typedef union {
    uint32_t whole;
    struct {
        uint16_t lower;
        uint16_t upper;
    } word;
    uint8_t byte[4];
} reg_t;

static void writer(uint32_t input) {
    // Clock (PA5)
    // Data (PB0)
    // LE (PB2)
    gpio_clear(GPIOA, GPIO5);
    gpio_clear(GPIOB, GPIO0);
    gpio_clear(GPIOB, GPIO2);

#define NOF_BITS 19
    int bits = 0;
    for (int i = 0; i <= 3; i++) {
        if (bits >= NOF_BITS)
            break;
        uint8_t data[4];
        data[0] = input & 0x000000FF;
        data[1] = (input & 0x0000FF00) >> 8;
        data[2] = (input & 0x00FF0000) >> 16;
        data[3] = (input & 0xFF000000) >> 24;
        uint8_t byte = data[i];
        for (int j = 0; j < 8; j++) {
            /* Set CLK low */
            gpio_clear(GPIOA, GPIO5);

            NOP_DELAY();

            /* Set SDATA */
            if (byte & 0x01) {
                gpio_set(GPIOB, GPIO0);
                printf("1");
            }
            else {
                gpio_clear(GPIOB, GPIO0);
                printf("0");
            }

            NOP_DELAY();

            /* Set CLK high */
            gpio_set(GPIOA, GPIO5);

            NOP_DELAY();

#if PLL_DEBUG == 1
            /* Debug printing */
            //printf("%d", (byte & 0x01) > 0);
#endif

            byte = byte >> 1;
            bits++;
            if (bits >= NOF_BITS)
                break;
        }
    }

    /* Set SCLK low and strobe latch */
    gpio_clear(GPIOA, GPIO5);
    gpio_clear(GPIOB, GPIO0);

    gpio_set(GPIOB, GPIO2);
    gpio_set(GPIOB, GPIO0);
    NOP_DELAY();
    gpio_clear(GPIOB, GPIO0);
    gpio_clear(GPIOB, GPIO2);
    NOP_DELAY();
}

/*
 * We need to send LSB first
 */

/** Will write out reg with MSB first. Thus the first bit will be reg->byte[3]
 * MSB
 *
 *  TODO: Implement semophores?
 *
 *  If @p readback is one, do not turn off SLE after write. This is used by
 * readback function
 */
void pll_write(void) {
    printf("\r\n");
    /* Reset pins */
    gpio_clear(GPIOA, GPIO5);
    gpio_clear(GPIOB, GPIO0);
    gpio_clear(GPIOB, GPIO2);

    /* Data set as seen in spreadsheet, LSB is first */

    struct ref_reg {
        uint8_t cnt : 1;
        uint16_t ratio : 14;
        uint8_t sw : 1;
        uint8_t fc : 1;
        uint8_t lds : 1;
        uint8_t cs : 1;
        uint16_t empty : 13;
    } __attribute__((packed));

    struct div_reg {
        uint8_t cnt : 1;
        uint8_t swallow : 7;
        uint16_t programmable : 11;
        uint16_t empty : 13;
    } __attribute__((packed));

    union pll_1_config_t {
        reg_t whole_reg;
        struct ref_reg reg;
    };

    union pll_2_config_t {
        reg_t whole_reg;
        struct div_reg reg;
    };

    union pll_1_config_t init1;
    union pll_2_config_t init2;
    init1.whole_reg.whole = 0x00000000;
    init2.whole_reg.whole = 0x00000000;

    init1.reg.cnt = 1;
    init1.reg.ratio = 512;
    init1.reg.sw = 1;
    init1.reg.fc = 0;
    init1.reg.lds = 0;
    init1.reg.cs = 0;

    init2.reg.cnt = 0;
    init2.reg.swallow = 57;
    init2.reg.programmable = 630;

    // MSB first, should be sent in reverse
    // ...0001000010000000001 0x00008401
    // ...0100111011001110010 0x00027672
    uint32_t myint = init2.whole_reg.whole;
    printf("init1: 0x%08lX\r\n", init1.whole_reg.whole);
    printf("init2: 0x%08lX\r\n", init2.whole_reg.whole);
#if 0
    printf("init2: 0x%08lX\r\n", myint);
    printf("init2: 0x%02X%02X%02X%02X\r\n", init2.whole_reg.byte[3],
           init2.whole_reg.byte[2], init2.whole_reg.byte[1],
           init2.whole_reg.byte[0]);

    printf("init1.reg.cnt:          %d\r\n", init1.reg.cnt);
    printf("init1.reg.ratio:        %d\r\n", init1.reg.ratio);
    printf("init1.reg.sw:           %d\r\n", init1.reg.sw);
    printf("init1.reg.fc:           %d\r\n", init1.reg.fc);
    printf("init1.reg.lds:          %d\r\n", init1.reg.lds);
    printf("init1.reg.cs:           %d\r\n", init1.reg.cs);

    printf("init2.reg.cnt:          %d\r\n", init2.reg.cnt);
    printf("init2.reg.swallow:      %d\r\n", init2.reg.swallow);
    printf("init2.reg.programmable: %d\r\n", init2.reg.programmable);
#endif
    uint16_t pll_ok = gpio_get(GPIOC, GPIO15);
    printf("PLL LOCK OK = %x\r\n", pll_ok);
    printf("PLL LOCK OK = %x\r\n", gpio_get(GPIOC, GPIO15));

#if PLL_DEBUG == 1
    printf("Expected init1: 1000000000100001000\r\n");
    printf("                ");
#endif
    writer(init1.whole_reg.whole);
#if PLL_DEBUG == 1
    printf("\r\n                ");
#endif
    writer(init2.whole_reg.whole);
#if PLL_DEBUG == 1
    printf("\r\nExpected init2: 0100111001101110010\n");
#endif
}

void pll_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);

    // Clock (PA5)
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO5);
    // Data (PB0)
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO0);
    // LE (PB2)
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO2);
    // LOCK OK (PC15)
    gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO15);

    gpio_clear(GPIOA, GPIO5);
    gpio_clear(GPIOB, GPIO0);
    gpio_clear(GPIOB, GPIO2);
}
