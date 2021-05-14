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
#include "embARC.h"
#include "embARC_debug.h"

#define MAX_SYS_COUNTER_VALUE       (0xffffffff)

#ifndef BOARD_SYS_TIMER_HZ
#define BOARD_SYS_TIMER_HZ      (1000)
#endif

typedef struct main_args {
	int argc;
	char *argv[];
} MAIN_ARGS;

/** Change this to pass your own arguments to main functions */
MAIN_ARGS s_main_args = { 1, { "main" } };

/** board timer interrupt reset count */
static uint32_t cyc_hz_count = (BOARD_CPU_CLOCK / BOARD_SYS_TIMER_HZ);

/** board timer counter in timer interrupt */
static volatile uint64_t gl_sys_hz_cnt = 0;
/** board 1ms counter */
static volatile uint32_t gl_ms_cnt = 0;

#define HZ_COUNT_CONV(precision, base)  ((precision) / (base))

/**
 * @brief Board bare-metal timer interrupt.
 *  Interrupt frequency is based on the defined @ref BOARD_SYS_TIMER_HZ
 */
static void board_timer_isr(void *ptr)
{
	arc_timer_int_clear(BOARD_SYS_TIMER_ID);

	board_timer_update(BOARD_SYS_TIMER_HZ);
}

/**
 * @brief Initialise bare-metal board timer and interrupt
 * @details
 * This function is called in @ref board_init, and
 * it initializes the 1-MS timer interrupt for bare-metal mode
 */
static void board_timer_init(void)
{
	if (arc_timer_present(BOARD_SYS_TIMER_ID)) {
		int_disable(BOARD_SYS_TIMER_INTNO);                                                     /* disable first then enable */
		int_handler_install(BOARD_SYS_TIMER_INTNO, board_timer_isr);
		arc_timer_start(BOARD_SYS_TIMER_ID, TIMER_CTRL_IE | TIMER_CTRL_NH, cyc_hz_count);       /* start 1ms timer interrupt */

		int_enable(BOARD_SYS_TIMER_INTNO);
	}
	arc_timer_calibrate_delay(BOARD_CPU_CLOCK);
}
#if defined(MID_FATFS)
static FATFS sd_card_fs;        /* File system object for each logical drive */
#endif /* MID_FATFS */

#if defined(EMBARC_USE_BOARD_MAIN)
/*--- When new embARC Startup process is used----*/

#ifdef OS_FREERTOS

#define MIN_CALC(x, y)          (((x) < (y))?(x):(y))
#define MAX_CALC(x, y)          (((x) > (y))?(x):(y))
/* Note: Task size in unit of StackType_t */
/* Note: Stack size should be small than 65536, since the stack size unit is uint16_t */
#define MIN_STACKSZ(size)       (MIN_CALC(size, configTOTAL_HEAP_SIZE) / sizeof(StackType_t))

#ifdef MID_LWIP
#include "lwip_wifi.h"

#ifndef TASK_WIFI_PERIOD
#define TASK_WIFI_PERIOD        50 /* WiFi connection task polling period, unit: kernel ticks */
#endif

#ifndef TASK_STACK_SIZE_WIFI
/* WiFi task stack size */
#define TASK_STACK_SIZE_WIFI    MIN_STACKSZ(1024)
#endif

#ifndef TASK_PRI_WIFI
#define TASK_PRI_WIFI           (configMAX_PRIORITIES - 1) /* WiFi task priority */
#endif

static DEV_WNIC *wifi_wnic;
static TaskHandle_t task_handle_wifi;

#endif /* MID_LWIP */

#ifdef MID_NTSHELL

#ifndef TASK_STACK_SIZE_NTSHELL
#define TASK_STACK_SIZE_NTSHELL MIN_STACKSZ(4096)
#endif

#ifndef TASK_PRI_NTSHELL
#define TASK_PRI_NTSHELL        1       /* NTSHELL task priority */
#endif
static TaskHandle_t task_handle_ntshell;

#else /* No middleware ntshell,will activate main task */

#ifndef TASK_STACK_SIZE_MAIN
#define TASK_STACK_SIZE_MAIN    MIN_STACKSZ(4096)
#endif

#ifndef TASK_PRI_MAIN
#define TASK_PRI_MAIN           1       /* Main task priority */
#endif
static TaskHandle_t task_handle_main;

#endif /* MID_NTSHELL */

#ifdef MID_LWIP
static void task_wifi(void *par)
{
	WNIC_AUTH_KEY auth_key;
	int flag = 0;

#ifdef USE_SLIP
	wifi_wnic = wnic_get_dev(BOARD_SLIPWIFI_0_ID);
#else
	wifi_wnic = wnic_get_dev(BOARD_PMWIFI_0_ID);
#endif

#if WF_HOTSPOT_IS_OPEN
	auth_key.key = NULL;
	auth_key.key_len = 0;
	auth_key.key_idx = 0;
#else
	auth_key.key = (const uint8_t *)WF_HOTSPOT_PASSWD;
	auth_key.key_len = sizeof(WF_HOTSPOT_PASSWD);
	auth_key.key_idx = 0;
#endif
	lwip_wifi_init();
	EMBARC_PRINTF("\r\nNow trying to connect to WIFI hotspot, please wait about 30s!\r\n");

	while (1) {
		wifi_wnic->period_process(par);
#if WF_HOTSPOT_IS_OPEN
		wifi_wnic->wnic_connect(AUTH_SECURITY_OPEN, (const uint8_t *)WF_HOTSPOT_NAME, &auth_key);
#else
		wifi_wnic->wnic_connect(AUTH_SECURITY_WPA_AUTO_WITH_PASS_PHRASE, (const uint8_t *)WF_HOTSPOT_NAME, &auth_key);
#endif
		if ((flag == 0) && lwip_wifi_isup()) {
			flag = 1;
			EMBARC_PRINTF("WiFi connected \r\n");
#ifndef MID_NTSHELL /* resume main task when ntshell task is not defined */
			if (task_handle_main) {
				vTaskResume(task_handle_main);
			}
#else
			EMBARC_PRINTF("Please run NT-Shell command(main) to start your application.\r\n");
			EMBARC_PRINTF("main command may required some arguments, please refer to example's document.\r\n");
#endif
			/* consider to generate a event to notify network is ready */
		}
		vTaskDelay(TASK_WIFI_PERIOD);
	}
}
#endif /* MID_LWIP */

static void task_main(void *par)
{
	int ercd;

#if defined(MID_LWIP) && !defined(MID_NTSHELL)
	EMBARC_PRINTF("Enter to main function....\r\n");
	EMBARC_PRINTF("Wait until WiFi connected...\r\n");
	vTaskSuspend(NULL);
#endif

	if ((par == NULL) || (((int)par) & 0x3)) {
		/* null or aligned not to 4 bytes */
		ercd = arc_goto_main(0, NULL);
	} else {
		MAIN_ARGS *main_arg = (MAIN_ARGS *)par;
		ercd = arc_goto_main(main_arg->argc, main_arg->argv);
	}

	EMBARC_PRINTF("Exit from main function, error code:%d....\r\n", ercd);
	while (1) {
		vTaskSuspend(NULL);
	}
}
#endif /* OS_FREERTOS */

EMBARC_WEAK void platform_main(void)
{
#ifdef MID_COMMON
	xprintf_setup();
#endif
	// platform_print_banner();
#ifdef MID_FATFS
	if (f_mount(&sd_card_fs, "", 0) != FR_OK) {
		EMBARC_PRINTF("FatFS failed to initialize!\r\n");
	} else {
		EMBARC_PRINTF("FatFS initialized successfully!\r\n");
	}
#endif

#ifdef ENABLE_OS
	os_hal_exc_init();
#endif

#ifdef MID_NTSHELL
	NTSHELL_IO *nt_io;
	nt_io = get_ntshell_io(BOARD_ONBOARD_NTSHELL_ID);
#endif

#ifdef OS_FREERTOS
#ifdef MID_NTSHELL
	xTaskCreate((TaskFunction_t)ntshell_task, "ntshell-console", TASK_STACK_SIZE_NTSHELL,
		    (void *)nt_io, TASK_PRI_NTSHELL, &task_handle_ntshell);
#endif
#ifdef MID_LWIP
	xTaskCreate((TaskFunction_t)task_wifi, "wifi-conn", TASK_STACK_SIZE_WIFI,
		    (void *)1, TASK_PRI_WIFI, &task_handle_wifi);
#endif
	xTaskCreate((TaskFunction_t)task_main, "main", TASK_STACK_SIZE_MAIN,
		    (void *)(&s_main_args), TASK_PRI_MAIN, &task_handle_main);
	// vTaskStartScheduler() Will not return unless a task calls vTaskEndScheduler
	vTaskStartScheduler();
#else /* OS_FREERTOS not defined */
	cpu_unlock();   /* unlock cpu to let interrupt work */
#ifdef MID_NTSHELL
	/** enter ntshell command routine no return */
	ntshell_task((void *)nt_io);
	// no return
#endif
	// task_main((void *)(&s_main_args));
	arc_goto_main(0, NULL);
#endif /* OS_FREERTOS */
}

EMBARC_WEAK void board_main(void)
{
#if defined(__MW__)
	/* Metaware toolchain C++ init */
	arc_mwdt_init();
#elif defined(__GNU__)
	/* ARC GNU toolchain C++ init */
	arc_gnu_do_global_ctors_aux();
	arc_gnu_do_init_array_aux();
#endif
	/* init core level interrupt & exception management */
	exc_int_init();
	/* init cache */
	arc_cache_init();
	/* necessary board level init */
	board_init();
	/* Initialise bare-metal board timer and interrupt */
	board_timer_init();
	/* platform (e.g RTOS, baremetal)level init */
	platform_main();
/* board level exit */
#if defined(__MW__)
	arc_mwdt_fini();
#elif defined(__GNU__)
	arc_gnu_do_global_dtors_aux();
#endif
}

#else /*-- Old embARC Startup process */

EMBARC_WEAK void platform_main(void)
{
#ifdef MID_COMMON
	xprintf_setup();
#endif
	// platform_print_banner();
#ifdef MID_FATFS
	if (f_mount(&sd_card_fs, "", 0) != FR_OK) {
		EMBARC_PRINTF("FatFS failed to initialize!\r\n");
	} else {
		EMBARC_PRINTF("FatFS initialized successfully!\r\n");
	}
#endif
	// cpu_unlock();
	arc_goto_main(0, NULL);
}

EMBARC_WEAK void board_main(void)
{
#if defined(__MW__)
/* Metaware toolchain C++ init */
	arc_mwdt_init();
#elif defined(__GNU__)
/* ARC GNU toolchain C++ init */
	arc_gnu_do_global_ctors_aux();
	arc_gnu_do_init_array_aux();
#endif
	/* init core level interrupt & exception management */
	exc_int_init();
	/* init cache */
	arc_cache_init();

	platform_main();
#if defined(__MW__)
	arc_mwdt_fini();
#elif defined(__GNU__)
	arc_gnu_do_global_dtors_aux();
#endif
}
#endif /* EMBARC_USE_BOARD_MAIN */

/**
 * @brief Update timer counter and other MS period operation
 * in cycling interrupt and must be called periodically.
 * @param precision interrupt-period precision in Hz
 */
void board_timer_update(uint32_t precision)
{
	static uint32_t sys_hz_update = 0;
	static uint32_t sys_ms_update = 0;
	uint32_t hz_conv = 0;

	/** count sys hz */
	hz_conv = HZ_COUNT_CONV(precision, BOARD_SYS_TIMER_HZ);
	sys_hz_update++;
	if (sys_hz_update >= hz_conv) {
		sys_hz_update = 0;
		gl_sys_hz_cnt++;
	}

	/** count ms */
	hz_conv = HZ_COUNT_CONV(precision, BOARD_SYS_TIMER_MS_HZ);
	sys_ms_update++;
	if (sys_ms_update >= hz_conv) {
		sys_ms_update = 0;
		gl_ms_cnt++;
	}
}

/**
 * @brief Get current timer's counter value in ticks
 * @retval Ticks count in 64 bit format
 */
uint64_t board_get_hwticks(void)
{
	uint32_t sub_ticks;
	uint64_t total_ticks;

	arc_timer_current(TIMER_0, &sub_ticks);

	total_ticks = (uint64_t)OSP_GET_CUR_MS() * (BOARD_CPU_CLOCK / BOARD_SYS_TIMER_HZ);
	total_ticks += (uint64_t)sub_ticks;

	return total_ticks;
}

/**
 * @brief Get current passed us since timer init
 * @retval us Count in 64 bit format
 */
uint64_t board_get_cur_us(void)
{
	uint32_t sub_us;
	uint64_t total_us;

	arc_timer_current(TIMER_0, &sub_us);

	sub_us = ((uint64_t)sub_us * 1000000) / BOARD_CPU_CLOCK;
	total_us = ((uint64_t)OSP_GET_CUR_MS()) * 1000 + (uint64_t)sub_us;

	return total_us;
}

/**
 * @brief Get current passed ms since timer init
 * @retval ms Count in 32 bit format
 */
uint32_t board_get_cur_ms(void)
{
	return gl_ms_cnt;
}

/**
 * @brief Get board timer counter in timer interrupt
 * @retval Count in 64 bit format
 */
uint64_t board_get_cur_syshz(void)
{
	return gl_sys_hz_cnt;
}

/**
 * \brief	provide MS delay function
 * \details
 * 		this function needs a 1-MS timer interrupt to work.
 * 	For bare-metal, it is implemented in this file.
 * 	For OS, you must call \ref board_timer_update in
 * 	the OS 1-MS timer interrupt when the bare-metal timer interrupt
 * 	is not ready
 * \param[in]	ms		MS to delay
 * \param[in]	os_compat	Enable or disable
 *	When this delay is enabled, use the OS delay function, if one is provided.
 *	See \ref OSP_DELAY_OS_COMPAT_ENABLE and
 *	\ref OSP_DELAY_OS_COMPAT_DISABLE
 */
void board_delay_ms(uint32_t ms, uint8_t os_compat)
{
	uint64_t start_us, us_delayed;

#ifdef ENABLE_OS
	if (os_compat == OSP_DELAY_OS_COMPAT_ENABLE) {
		/** \todo add different os delay functions */
#ifdef OS_FREERTOS
		vTaskDelay(ms);
		return;
#endif
	}
#endif
	us_delayed = ((uint64_t)ms * 1000);
	start_us = board_get_cur_us();
	while ((board_get_cur_us() - start_us) < us_delayed) {
		;
	}
}