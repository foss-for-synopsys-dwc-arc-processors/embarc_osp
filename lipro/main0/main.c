#include "embARC.h"
#include "embARC_debug.h"
#include <stdio.h>
#include "shared_data.h"
#include "tx_api.h"
#include "arc_timer.h"
#include "board.h"

const uint32_t DEMO_STACK_SIZE = 1024;
CHAR stack_buffer[DEMO_STACK_SIZE];

#define MS_TO_TICKS(msval) ((ULONG)((ULONG)msval * (ULONG)BOARD_SYS_TIMER_MS_CONV))

void blink_thread_func(ULONG param)
{
	const uint8_t LED_TOGGLE_MASK = BOARD_LED_MASK;
	uint8_t led_toggle_val = LED_TOGGLE_MASK;
	uint32_t mycounter = 0;
	uint32_t tick_delay = 500;
	printf("blink_thread_func has started\n");
	while (1) {
		led_write(led_toggle_val, (uint8_t)BOARD_LED_MASK);	
		tx_thread_sleep(tick_delay);	
        mycounter++;
        EMBARC_PRINTF ("shared counter is %d, my counter is %d\n", gp_shared_data->counter, mycounter);

	}
}

void _tx_timer_0(void);
static void setup_timer_interrupt(void)
{
	unsigned int cyc = BOARD_CPU_CLOCK / BOARD_SYS_TIMER_HZ;

	int_disable(BOARD_OS_TIMER_INTNO); /* disable os timer interrupt */
	timer_stop(BOARD_OS_TIMER_ID);
	timer_start(BOARD_OS_TIMER_ID, TIMER_CTRL_IE | TIMER_CTRL_NH, cyc);

	exc_entry_install(BOARD_OS_TIMER_INTNO, (EXC_ENTRY)_tx_timer_0);
	int_pri_set(BOARD_OS_TIMER_INTNO, INT_PRI_MIN);
	int_enable(BOARD_OS_TIMER_INTNO);
}

void tx_application_define(void * first_unused_memory)
{
	TX_THREAD blink_thread;

	// setup timer interrupt here, to override the kernel_init configuration 
	// TBD: this will be replaced by setting up the correct value in the timer init code in the low level kernel code
	setup_timer_interrupt();


    printf("main: tx_application_define has started\n");
    printf("main: tx_application_define: shared_data size %u at %p\n", sizeof(shared_data_t), gp_shared_data);
    uint32_t id = _lr(0x4);
    printf("main: tx_application_define: id = 0x%x\n", id);


	tx_thread_create (&blink_thread, (char *) "Blink Thread", blink_thread_func, (uint32_t) NULL, stack_buffer, DEMO_STACK_SIZE, 1, 1,
                      TX_NO_TIME_SLICE, TX_AUTO_START);
}



int main(void)
{
    tx_kernel_enter ();
    return 0;
}
