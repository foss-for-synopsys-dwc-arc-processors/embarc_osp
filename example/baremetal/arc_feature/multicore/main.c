/* ------------------------------------------
 * Copyright (c) 2019, Synopsys, Inc. All rights reserved.

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
/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"

#if ARC_FEATURE_CONNECT_PRESENT
#define DELAY_COUNT     1000000

static ARC_SPINLOCK_T output_lock;

static void smp_printf(const char *fmt, ...)
{
	va_list arp;

	va_start(arp, fmt);

	arc_spin_lock(&output_lock);

	xvprintf(fmt, arp);

	arc_spin_unlock(&output_lock);

	va_end(arp);
}

#undef EMBARC_PRINTF
#define EMBARC_PRINTF smp_printf

static volatile uint32_t cores_t0_count[ARC_FEATURE_MP_NUM_CPUS];
static volatile uint32_t cores_delay_count[ARC_FEATURE_MP_NUM_CPUS];

static void cores_timer0_isr(void *ptr)
{
	arc_timer_int_clear(TIMER_0);
	cores_t0_count[arc_core_id()]++;
}

static void cores_ici_isr(void *ptr)
{
	uint32_t from;

	from = arc_find_lsb(arc_connect_ici_check_src()) - 1;
	EMBARC_PRINTF("core %d receives int from core %d\r\n", arc_core_id(), from);
	arc_connect_ici_ack(from);
}

static void cores_main(uint32_t id, void *arg)
{
	uint32_t next_id = (id + 1) % ARC_FEATURE_MP_NUM_CPUS;

	cpu_unlock();
	EMBARC_PRINTF("core %d starts to run\r\n", id);

	EMBARC_PRINTF("core %d in arc connect is %d\r\n", id, arc_connect_check_core_id());
	int_enable(INTNO_TIMER0);
	arc_timer_start(TIMER_0, TIMER_CTRL_IE, DELAY_COUNT);
	int_enable(INTNO_ICI);

	while (1) {
		while (cores_t0_count[id] < cores_delay_count[id]) {
			;
		}
		cores_t0_count[id] = 0;
		EMBARC_PRINTF("core %d delay %d ticks\r\n", id, cores_delay_count[id]);
		arc_connect_ici_generate(next_id);
		while (arc_connect_ici_read_status(next_id) != 0) {
			;
		}
	}
}

static void slave_cores_entry(uint32_t id, void *arg)
{
	exc_int_init();
	cores_main(id, arg);
}

/* stack size for slave cores */
#define SLAVE_CORE_STACK_SIZE   (2048)
static uint32_t slave_core_stacks[ARC_FEATURE_MP_NUM_CPUS - 1][SLAVE_CORE_STACK_SIZE / 4];

#endif /* ARC_FEATURE_CONNECT_PRESENT */

int32_t main(void)
{
#if ARC_FEATURE_CONNECT_PRESENT
	int32_t i;

	int_handler_install(INTNO_TIMER0, cores_timer0_isr);
	int_handler_install(INTNO_ICI, cores_ici_isr);

	cores_delay_count[0] = 5;

	for (i = 1; i < ARC_FEATURE_MP_NUM_CPUS; i++) {
		cores_delay_count[i] = (i << 2) + 5;
		arc_start_slave_cpu(i, (uint8_t *)slave_core_stacks[i - 1],
				    SLAVE_CORE_STACK_SIZE, slave_cores_entry, NULL);
	}

	cores_main(0, NULL);
#else /* ARC_FEATURE_CONNECT_PRESENT */
	EMBARC_PRINTF("This example is not supportted under current configures \r\n");
#endif /* ARC_FEATURE_CONNECT_PRESENT */

}
