#include "embARC.h"
#include "embARC_debug.h"
#include <stdio.h>
#include "lipro_common_defs.h"

#define LED_TOGGLE_MASK		BOARD_LED_MASK

/**
 * \brief	Test hardware board without any peripheral
 */
int main(void)
{
	uint16_t led_toggle_val = LED_TOGGLE_MASK;

    printf("main has started\n");
    printf("main: shared_data size %u at %p\n", sizeof(shared_data_t), gp_shared_data);
    uint32_t id = _lr(0x4);
    printf("main: id = 0x%x\n", id);

	while (1) {
		led_write(led_toggle_val, BOARD_LED_MASK);
		led_toggle_val = ~led_toggle_val;
		board_delay_ms(400, 1);
                printf ("counter is %d\n", gp_shared_data->counter);
	}

	return E_SYS;
}
