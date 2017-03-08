#include "embARC.h"
#include "container1.h"
#include "container2.h"
#include "background_container.h"

/* General container set-up and main application */

static CONTAINER_AC_TABLE g_container1_act[] = {
	{init_secret, 0, SECURESHIELD_AC_INTERFACE},
	{operate_secret, 3, SECURESHIELD_AC_INTERFACE}
 };

static CONTAINER_AC_TABLE g_container2_act[] = {
	{trusted_ops, 0, SECURESHIELD_AC_INTERFACE}
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
#if LIB_SECURESHIELD_VERSION == 1
	{(void *)0x21, 0x3, SECURESHIELD_AC_AUX},
	{(void *)0x100, 0x6, SECURESHIELD_AC_AUX},
	{(void *)0x75, 0x1, SECURESHIELD_AC_AUX},
#endif
	{soft_interrupt1, INTNO_SWI, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_TIMER0, SECURESHIELD_AC_IRQ}
};

/* enable secureshield, set the access control table of background container */
SECURESHIELD_SET_MODE_AC(SECURESHIELD_ENABLED, g_main_container_act);

/* configure the secret container */
/* container1 is secure container, it has access to most of the system resources */
SECURESHIELD_CONTAINER_CONFIG(container1, g_container1_act, 1024, SECURESHIELD_CONTAINER_SECURE);
SECURESHIELD_CONTAINER_CONFIG(container2, g_container2_act, 1024);