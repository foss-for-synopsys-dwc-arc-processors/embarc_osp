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
 * \version 2017.03
 * \date 2016-11-14
 * \author Qiang Gu(Qiang.Gu@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_BAREMETAL_OPENTHREAD	embARC OpenThread example on MRF24J40
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_BAREMETAL
 * \ingroup	EMBARC_APPS_MID_OPENTHREAD
 * \brief	OpenThread command line interface example on MRF24J40
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     * 2 x EMSK
 *     * 2 x Digilent PMOD RF2 (MRF24J40)
 *     * 1 x SD card
 *
 * ### Design Concept
 *     This example is an OpenThread Command Line Interface (CLI) application on PMOD RF2 (MRF24J40).
 *     The mesh network is established, and IPv6 is configured with using bi-/multi-boards as Thread nodes.
 *     The node status can be shown on the terminal via UART. There are dozens of commands supported in the CLI example.
 *     The OpenThread CLI reference is in README.md.
 *
 * ### Usage Manual
 *     - See \ref EMBARC_BOARD_CONNECTION "EMSK" to connect PMOD RF2 (MRF24J40).
 *
 *     - How to use this example
 *
 *       * Program the secondary bootloader application into onboard SPI flash of EMSKs.
 *       * Generate boot.bin of the Openthread CLI example using "make bin".
 *       * Run Openthread CLI example with boot.bin from SD card. Make sure Bit 4 of the onboard DIP switch is ON to enable
 *         the secondary bootloader.
 *         - Insert SD Card back to one EMSK. Press the reset button to reboot it. Wait for loading boot.bin from SD card.
 *         - Get SD card from one EMSK and insert it to the other EMSK. Press the reset button to reboot it. Wait for loading boot.bin from SD card.
 *         - Enter “1” and press Enter button in one Tera Term. Enter “2” and press Enter button in the other one.
 *           The number will not be shown directly in the Tera Term until pressing Enter button from the keyboard.
 *         - Enter the following commands in the two terminal windows, "panid 0x1234", "ifconfig up", "thread start", to start
 *           start Thread process.
 *         - Wait 20 seconds for completing Thread configuration. Enter “state” to see the state of the node, one leader and one router.
 *         - Enter other commands in the OpenThread CLI reference (README.md) to get more information. For example,
 *           “ipaddr” is used to show the IP address of the Thread node. Enter “ipaddr” in one ternimal to get the Thread node IP address,
 *           such as **fdde:ad00:beef:0:0:ff:fe00:ec00**. Enter “ping fdde:ad00:beef:0:0:ff:fe00:ec00” in the other ternimal to ping the Thread node.
 *           The Internet Control Messages Protocol (ICMP) is implemented in the OpenThread for **ping** command.
 *
 * ![ScreenShot of Thread nodes and OpenThread startup](pic/images/example/emsk/emsk_openthread_connection.jpg)
 * ![ScreenShot of 'start' and 'ping' in OpenThread](pic/images/example/emsk/emsk_openthread_configuration.jpg)
 *
 *
 * ### Extra Comments
 *     * A few seconds are required to make connections of Thread nodes.
 *     * Use AC adapter to ensure a steady power supply.
 *     * Open two Tera Term emulators to connect EMSKs with different COM ports.
 *     * The self-boot mode preparation is included in the above steps.
 *     * “make run” is not supported because EMSK boards are both v2.x and it can lead to conflict.
 *       See \ref sect_example_usage_HowToDebugMultiBoards "how to debug multiple boards in embARC" for more information.
 *     * Enter number to generate the pseudo-random number for OpenThread. Recommend enter number in order, such as “1”, “2” and “3”.
 *       Using same number in different nodes may lead error.
 *     * Make sure the compiler configuration (TOOLCHAIN, BD_VER, CUR_CORE) of the secondary bootloader and bin file.
 *       For example, the bootloader for EMSK 2.3/ARCEM7D cannot boot the program for EMSK 2.3/ARCEM11D.
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_BAREMETAL_OPENTHREAD
 * \brief  example of how to use OpenThread command-line interface
 *   This example demonstrates a minimal OpenThread application to
 *   show the OpenThread configuration and management interfaces via a basic command-line interface.
 */

/**
 * \addtogroup	EMBARC_APP_BAREMETAL_OPENTHREAD
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"

#ifdef OPENTHREAD_CONFIG_FILE
#include OPENTHREAD_CONFIG_FILE
#else
#include <openthread-config.h>
#endif

#include <openthread-core-config.h>
#include <openthread.h>
#include <openthread-diag.h>
#include <openthread-tasklet.h>
#include "cli/cli-uart.h"
#include "platform/platform.h"
#include <assert.h>

#ifdef TASK_STACK_SIZE_MAIN
#undef TASK_STACK_SIZE_MAIN
#endif
#define TASK_STACK_SIZE_MAIN	MIN_STACKSZ(131070)

void otSignalTaskletPending(otInstance *aInstance)
{
	(void)aInstance;
}

int main(void)
{
	otInstance *sInstance;

	int argc = 0;
	char **argv = NULL;

	PlatformInit(argc, argv);

	sInstance = otInstanceInit();
	assert(sInstance);

	otCliUartInit(sInstance);

	EMBARC_PRINTF("OpenThread Start\r\n");

#if OPENTHREAD_ENABLE_DIAG
	diagInit(sInstance);
#endif

	while (1)
	{
		otProcessQueuedTasklets(sInstance);
		PlatformProcessDrivers(sInstance);

		//mrf24j40_delay_ms(70);
	}

	return 0;
}

/** @} */
