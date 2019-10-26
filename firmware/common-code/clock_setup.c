#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/rcc.h>
#include "clock_setup.h"

void rcc_clock_setup_in_hse_10_24mhz_out_10_24mhz(void) {
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
