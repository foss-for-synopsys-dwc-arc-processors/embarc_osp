/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */
#ifndef _SYSCONF_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct sysconf_reg {
	uint32_t reserved1;	/* 0x0 */
	uint32_t AHBCLKDIV;	/* AHB clock divisor */
	uint32_t APBCLKDIV; 	/* APB clock divisor */
	uint32_t APBCLKEN;	/* APB module clock enable */
	uint32_t CLKODIV;	/* AHB clock output enable and divisor set */
	uint32_t reserved2;      /* 0x14 */
	uint32_t RSTCON;	/* reset contrl */
	uint32_t RSTSTAT;	/* reset status */
	uint32_t AHBCLKDIV_SEL; /* AHB clock divisor select */
	uint32_t CLKSEL;	/* main clock source select */
	uint32_t PLLSTAT;	/* PLL status register */
	uint32_t PLLCON;	/* PLL control register */
	uint32_t reserved3;	/* 0x30 */
	uint32_t AHBCLKEN;	/* AHB module clock enbale */
	uint32_t reserved4[2];  /* 0x38, 0x3c */
	uint32_t I2S_TX_SCLKDIV; /* I2S TX SCLK divisor */
	uint32_t I2S_RX_SCLKDIV; /* I2S RX SCLK divisor */
	uint32_t I2S_RX_SCLKSEL; /* I2S RX SCLK source select */
	uint32_t SDIO_REFCLK_DIV; /* SDIO reference clock divisor */
	uint32_t GPIO4B_DBCLK_DIV; /* GPIO4B DBCLK divisor */
	uint32_t IMAGE_CHK;	/* Image pad status */
	uint32_t PROT_RANGE;	/* PROT range */
	uint32_t SPI_MST_CLKDIV; /* SPI master clock divisor */
	uint32_t DVFS_CLKDIV;	/* DFSS main clock domain divider */
	uint32_t DVFS_VDDSET;	/* VDD setting */
	uint32_t DVFS_VWTIME;	/* VDD adjust waiting time */
	uint32_t PMC_PUWTIME;	/* power up waiting time */
	uint32_t PMOD_MUX;	/* PMOD IO mux */
	uint32_t ARDUINO_MUX;	/* arduino IO mux */
	uint32_t USBPHY_PLL;	/* USBPHY PLL */
	uint32_t USBCFG;	/* USB configuration */
	uint32_t TIMER_PAUSE;	/* PWM timer puse */
	uint32_t GPIO8B_DBCLK_DIV; /* GPIO8B DBCLK divisor */
	uint32_t RESET_PD_VECTOR; /* reset powerdown vector */
	uint32_t UART3SCLK_DIV;  /* UART3SCLK_DIV */
} SYSCONF_REG, *SYSCONF_REG_PTR;


extern SYSCONF_REG_PTR sysconf_reg_ptr;

/* CLKSEL_CONST is not described in spec. */
#define CLKSEL_CONST	(0x5A690000)
#define CLKSEL_EXT_16M	(0 | CLKSEL_CONST)
#define CLKSEL_PLL	(1 | CLKSEL_CONST)
#define CLKSEL_EXT_32K	(2 | CLKSEL_CONST)

#define PLLCON_BIT_OFFSET_N 		0
#define PLLCON_BIT_OFFSET_M 		4
#define PLLCON_BIT_OFFSET_OD 		20
#define PLLCON_BIT_OFFSET_BP 		24
#define PLLCON_BIT_OFFSET_PLLRST	26


#define PLLSTAT_BIT_OFFSET_PLLSTB 	2
#define PLLSTAT_BIT_OFFSET_PLLRDY 	3


#define AHBCLKEN_BIT_I2S		0
#define AHBCLKEN_BIT_USB		1
#define AHBCLKEN_BIT_FLASH		2
#define AHBCLKEN_BIT_FMC		3
#define AHBCLKEN_BIT_DVFS		4
#define AHBCLKEN_BIT_PMC		5
#define AHBCLKEN_BIT_BOOT_SPI		6
#define AHBCLKEN_BIT_SDIO		7

#define APBCLKEN_BIT_ADC		0
#define APBCLKEN_BIT_I2S_TX		1
#define APBCLKEN_BIT_I2S_RX		2
#define APBCLKEN_BIT_RTC		3
#define APBCLKEN_BIT_PWM		4
#define APBCLKEN_BIT_I3C		5


#define SPI_MASTER_0			0
#define SPI_MASTER_1			1
#define SPI_MASTER_2			2

#define GPIO8B_BANK0			0
#define GPIO8B_BANK1			1
#define GPIO8B_BANK2			2
#define GPIO8B_BANK3			3

#define GPIO4B_BANK0			0
#define GPIO4B_BANK1			1
#define GPIO4B_BANK2			2

/* reset caused by power on */
#define SYS_RST_SOFTWARE_ON		0x2


#define DVFS_PERF_LEVEL0		0
#define DVFS_PERF_LEVEL1		1
#define DVFS_PERF_LEVEL2		2
#define DVFS_PERF_LEVEL3		3

/* pmode mux definition */
#define PMOD_MUX_PMA			(0x1)
#define PMOD_MUX_PMB			(0x2)
#define PMOD_MUX_PMC			(0x4)

/* arduino mux definition */
#define ARDUINO_MUX_UART		(0x1)
#define ARDUINO_MUX_SPI			(0x2)
#define ARDUINO_MUX_PWM0		(0x4)
#define ARDUINO_MUX_PWM1		(0x8)
#define ARDUINO_MUX_PWM2		(0x10)
#define ARDUINO_MUX_PWM3		(0x20)
#define ARDUINO_MUX_PWM4		(0x40)
#define ARDUINO_MUX_PWM5		(0x80)
#define ARDUINO_MUX_I2C			(0x100)
#define ARDUINO_MUX_ADC0		(0x400)
#define ARDUINO_MUX_ADC1		(0x800)
#define ARDUINO_MUX_ADC2		(0x1000)
#define ARDUINO_MUX_ADC3		(0x2000)
#define ARDUINO_MUX_ADC4		(0x4000)
#define ARDUINO_MUX_ADC5		(0x8000)

#define PWM_TIMER0			0
#define PWM_TIMER1			1
#define PWM_TIMER2			2
#define PWM_TIMER3			3
#define PWM_TIMER4			4
#define PWM_TIMER5			5


extern void pll_conf_reg(uint32_t val);
extern int32_t pll_fout_config(uint32_t freq);

extern void ahb_clk_divisor(uint8_t div);
extern void apb_clk_divisor(uint8_t div);
extern void ahb_clk_enable(uint8_t dev);
extern void ahb_clk_disable(uint8_t dev);
extern void sdio_clk_divisor(uint8_t div);
extern void spi_master_clk_divisor(uint8_t id, uint8_t div);
extern void gpio8b_dbclk_div(uint8_t bank, uint8_t div);
extern void gpio4b_dbclk_div(uint8_t bank, uint8_t div);
extern void i2s_tx_clk_div(uint8_t div);
extern void i2s_rx_clk_div(uint8_t div);
extern void i2s_rx_clk_sel(uint8_t sel);
extern void syscon_reset(void);
extern uint32_t is_poweron_rst(void);
extern void dvfs_clk_divisor(uint8_t level, uint8_t div);
extern void dvfs_vdd_config(uint8_t level, uint8_t val);
extern void dvfs_vwtime_config(uint8_t time);
extern void pmc_pwwtime_config(uint8_t time);
extern void uart3_clk_divisor(uint8_t div);
extern void reset_powerdown_vector(uint32_t addr);
extern void eflash_clk_div(uint8_t div);


#ifdef __cplusplus
}
#endif
#endif /* _SYSCONF_H_ */