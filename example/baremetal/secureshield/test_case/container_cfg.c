#include "embARC.h"
#include "container1.h"
#include "container2.h"
#include "container3.h"
#include "container4.h"
#include "background_container.h"

static CONTAINER_AC_TABLE g_container1_act[] = {
	{tst_func0, 0, SECURESHIELD_AC_INTERFACE},
	{tst_func1, 1, SECURESHIELD_AC_INTERFACE},
	{tst_func2, 2, SECURESHIELD_AC_INTERFACE},
	{soft_interrupt1, INTNO_SWI1, SECURESHIELD_AC_IRQ}
 };

static CONTAINER_AC_TABLE g_container2_act[] = {
	{tst_func3, 3, SECURESHIELD_AC_INTERFACE},
	{tst_func5, 5, SECURESHIELD_AC_INTERFACE},
	{soft_interrupt2, INTNO_SWI2, SECURESHIELD_AC_IRQ}
};

static CONTAINER_AC_TABLE g_container3_act[] = {
	{tst_func4, 4, SECURESHIELD_AC_INTERFACE},
	{tst_func6, 6, SECURESHIELD_AC_INTERFACE},
	{soft_interrupt3, INTNO_SWI3, SECURESHIELD_AC_IRQ}
};


static CONTAINER_AC_TABLE g_container4_act[] = {
	{tst_func_sec1, 0, SECURESHIELD_AC_INTERFACE},
	{tst_func_sec2, 0, SECURESHIELD_AC_INTERFACE}
};

static CONTAINER_AC_TABLE g_main_container_act[] = {
#ifdef BOARD_EMSK
#define PERIPHERAL_ADDR_BASE 0xf0000000
	/* PINMUX, UART1 and GPIO0 are required */
	{(void *)(PERIPHERAL_ADDR_BASE + REL_REGBASE_PINMUX), 0x1000, SECURESHIELD_ACDEF_UPERIPH},
	{(void *)(PERIPHERAL_ADDR_BASE + REL_REGBASE_UART1), 0x1000, SECURESHIELD_ACDEF_UPERIPH},
	{(void *)(PERIPHERAL_ADDR_BASE + REL_REGBASE_GPIO0), 0x1000, SECURESHIELD_ACDEF_UPERIPH},
	{default_interrupt_handler, INTNO_GPIO, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_UART1, SECURESHIELD_AC_IRQ},
#endif
	/* Timer1, Timer 0, RTC and TIMIE BCR are accessible for background container*/
#if SECURESHIELD_VERSION == 1
	{(void *)0x21, 0x3, SECURESHIELD_AC_AUX},
	{(void *)0x100, 0x6, SECURESHIELD_AC_AUX},
	{(void *)0x75, 0x1, SECURESHIELD_AC_AUX},
#endif
	{(void *)0x43, 0x1, SECURESHIELD_AC_AUX},
	{default_interrupt_handler, INTNO_SWI0, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_TIMER0, SECURESHIELD_AC_IRQ},

};


SECURESHIELD_SET_MODE_AC(SECURESHIELD_ENABLED, g_main_container_act);

/* configure containers */
SECURESHIELD_CONTAINER_CONFIG(container1, g_container1_act, 1024);
SECURESHIELD_CONTAINER_CONFIG(container2, g_container2_act, 1024, SECURESHIELD_CONTAINER_SECURE);
SECURESHIELD_CONTAINER_CONFIG(container3, g_container3_act, 1024);
SECURESHIELD_CONTAINER_CONFIG(container4, g_container4_act, 1024, SECURESHIELD_CONTAINER_SECURE);