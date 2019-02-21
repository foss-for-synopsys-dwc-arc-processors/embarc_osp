/* ------------------------------------------
 * Copyright (c) 2016, Synopsys, Inc. All rights reserved.

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

#ifndef _IOTDK_H_
#define _IOTDK_H_

#include "arc_em.h"

#include "drivers/ip/designware/i2s/dw_i2s_obj.h"
#include "drivers/ip/designware/sdio/dw_sdio_obj.h"
#include "drivers/ip/designware/pwm_timer/dw_pwm_timer_obj.h"
#include "drivers/ip/subsystem/iic/dfss_iic_obj.h"
#include "drivers/ip/subsystem/uart/dfss_uart_obj.h"
#include "drivers/ip/subsystem/spi/dfss_spi_obj.h"
#include "drivers/ip/subsystem/gpio/dfss_gpio_obj.h"
#include "drivers/ntshell/ntshell_io.h"
#include "drivers/sdcard/sdcard.h"
#include "drivers/sysconf/sysconf.h"
#include "common/iotdk_timer.h"
#include "drivers/pmwifi/pmwifi.h"
#include "drivers/smic/smic_adc/smic_adc.h"
#include "drivers/smic/smic_eflash/smic_eflash.h"
#include "drivers/smic/smic_bootspi/smic_bootspi.h"
#include "dev_pinmux.h"

#include "drivers/flash_obj/flash_obj.h"
#include "iotdk_hardware.h"

/*-----------------------------------------------------------------------------*/

#define BOARD_SYS_TIMER_ID			TIMER_0
#define BOARD_SYS_TIMER_INTNO			INTNO_TIMER0
#define BOARD_SYS_TIMER_HZ			(1000)

/** board timer count frequency (HZ) */
#define BOARD_SYS_TIMER_MS_HZ			(1000)
/** board timer count frequency convention based on the global timer counter */
#define BOARD_SYS_TIMER_MS_CONV			(BOARD_SYS_TIMER_MS_HZ/BOARD_SYS_TIMER_HZ)

#define BOARD_OS_TIMER_ID			TIMER_0
#define BOARD_OS_TIMER_INTNO			INTNO_TIMER0

#define BOARD_CPU_CLOCK				CLK_CPU
#define BOARD_DEV_CLOCK				CLK_BUS_APB

#define OSP_DELAY_OS_COMPAT_ENABLE		(1)
#define OSP_DELAY_OS_COMPAT_DISABLE		(0)

#define OSP_GET_CUR_SYSHZ()			(gl_iotdk_sys_hz_cnt)
#define OSP_GET_CUR_MS()			(gl_iotdk_ms_cnt)
#define OSP_GET_CUR_US()			board_get_cur_us()
#define OSP_GET_CUR_HWTICKS()			board_get_hwticks()

/*-----------------------------------------------------------------------------*/

/* common macros must be defined by iotdk boards */

#define BOARD_TEMP_SENSOR_IIC_ID		DFSS_IIC_1_ID
#define BOARD_TEMP_IIC_SLVADDR			TEMP_I2C_SLAVE_ADDRESS

#define BOARD_CONSOLE_UART_ID			DFSS_UART_0_ID
#define BOARD_CONSOLE_UART_BAUD			UART_BAUDRATE_115200

#define BOARD_ONBOARD_NTSHELL_ID		(IOTDK_NTSHELL_0_ID)

/** board exists flash device */
#define BOARD_FLASH_EXIST			(1)

/* iotdk DFSS SPI configuration */
#ifndef BOARD_SPI_FREQ
#define BOARD_SPI_FREQ				(1000000)
#endif

#ifndef BOARD_SPI_CLKMODE
#define BOARD_SPI_CLKMODE			(SPI_CLK_MODE_0)
#endif

#define WF_IPADDR_1			(192)
#define WF_IPADDR_2			(168)
#define WF_IPADDR_3			(43)
#define WF_IPADDR_4			(102)

#define WF_NETMASK_1			(255)
#define WF_NETMASK_2			(255)
#define WF_NETMASK_3			(255)
#define WF_NETMASK_4			(0)

#define WF_GATEWAY_1			(192)
#define WF_GATEWAY_2			(168)
#define WF_GATEWAY_3			(43)
#define WF_GATEWAY_4			(1)

#define WF_HOTSPOT_IS_OPEN		(0)

#define WF_IPADDR_DHCP			(1)

#define WF_HOTSPOT_NAME			"embARC"
#define WF_HOTSPOT_PASSWD		"qazwsxedc"

#define WF_ENABLE_MANUAL_SET_MAC	(0)

#define WF_MAC_ADDR0			(IOTDK_PMWIFI_0_MAC_ADDR0)
#define WF_MAC_ADDR1			(IOTDK_PMWIFI_0_MAC_ADDR1)
#define WF_MAC_ADDR2			(IOTDK_PMWIFI_0_MAC_ADDR2)
#define WF_MAC_ADDR3			(IOTDK_PMWIFI_0_MAC_ADDR3)
#define WF_MAC_ADDR4			(IOTDK_PMWIFI_0_MAC_ADDR4)
#define WF_MAC_ADDR5			(IOTDK_PMWIFI_0_MAC_ADDR5)

/** Possible Pmod WiFi choices */
#define PMWIFI_MRF24G			0
#define PMWIFI_RW009			1


#define BOARD_PMWIFI_0_ID		IOTDK_PMWIFI_0_ID
#define BOARD_WIFI_SPI_LINE		0
#define BOARD_WIFI_0_INT		4
#define BOARD_WIFI_0_RST		5

// Un-implemented LED/Switch/Button functions and macros
#define BOARD_LED_MASK			(0x0)
#define BOARD_LED_CNT			(0)
#define BOARD_BTN_MASK			(0x0)
#define BOARD_BTN_CNT			(0)
#define BOARD_SWT_MASK			(0x0)
#define BOARD_SWT_CNT			(0)

#define button_read(x) 			0
#define led_write(x, y)			EMBARC_PRINTF("led out: %x, %x\r\n", x, y)
#define led_read(mask)			(0x0)
#define led_toggle(mask)
#define switch_read(mask)		(0x0)

/* iotdk DFSS GPIO configuration */
// #define BOARD_GPIO_PORT_A			DFSS_GPIO_PORT_A
// #define BOARD_GPIO_PORT_B			DFSS_GPIO_PORT_B
// #define BOARD_GPIO_PORT_C			DFSS_GPIO_PORT_C
// #define BOARD_GPIO_PORT_D			DFSS_GPIO_PORT_D

#ifdef __cplusplus
extern "C" {
#endif

extern void board_init(void);
extern void board_timer_update(uint32_t precision);
extern void board_delay_ms(uint32_t ms, uint8_t os_compat);
extern uint64_t board_get_hwticks(void);
extern uint64_t board_get_cur_us(void);

#ifdef __cplusplus
}
#endif

#endif	/* _IOTDK_H_ */
