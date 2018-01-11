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
 * \date 2017-8-28
 * \author Xiangcai Huang(xiangcai@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_BAREMETAL_OPENTHREAD_NCP	embARC OpenThread NCP example on MRF24J40
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_BAREMETAL
 * \ingroup	EMBARC_APPS_MID_OPENTHREAD
 * \brief	OpenThread Network Co-Processor (NCP) on MRF24J40
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     * 2 x EMSK
 *     * 2 x Digilent PMOD RF2 (MRF24J40)
 *     * 2 x SD card
 *
 * ### Design Concept
 *     This example is an OpenThread Network Co-Processor (NCP) application on PMOD RF2 (MRF24J40).
 *     This example can serve as a low-power wireless Network Co-Processor (NCP) to join Thread network
 *         and communicate with Userspace WPAN Network Daemon (wpantund) on Unix-like operating systems.
 *     This example can be used along with OpenThread wpantund to provide Internet connectivity to the Thread network.
 *         For more information about NCP and wpantund, see README.md for reference.
 *     The NCP status can not be shown on the terminal via UART, it might show messy code.
 *
 * ### Usage Manual
 *     - See \ref EMBARC_BOARD_CONNECTION "EMSK" to connect PMOD RF2 (MRF24J40).
 *
 *     - How to use this example
 *
 *       * Program the secondary bootloader application into onboard SPI flash of EMSKs.
 *       * Generate boot.bin of the Openthread NCP example using "make bin".
 *       * Run an Openthread CLI example first with boot.bin from SD card, see `cli\README.md` for more information.
 *         - Insert SD Card back to one EMSK. It will run the CLI example. Press the reset button to reboot it. Wait for loading boot.bin from SD card.
 *         - Enter “1” and press Enter button in the Tera Term. Enter the number here to generate pseudo random number for OpenThread.
 *         - Enter the following commands in the two terminal windows, "panid 0x1234", "ifconfig up", "thread start", to start
 *           Thread process.
 *         - Wait 20 seconds for completing Thread configuration. Enter “state” to see the state of the node, it will be the leader.
 *       * Run Openthread NCP example with boot.bin from SD card. Make sure Bit 4 of the onboard DIP switch is ON to enable
 *         the secondary bootloader.
 *         - Insert the other SD card to the other EMSK. It will run the NCP example. Press the reset button to reboot it. Wait for loading boot.bin from SD card.
 *         - Enter **2** and press Enter button in the NCP's Tera Term.
 *         - After entering **2** in the NCP's Tera Term, the NCP will start up automatically.
 *           It will show you some messy code in the last line, just ignore it.
 *         - Install and Run OpenThread wpantund, and join the NCP to the Thread network created by CLI. See README.md for more information.
 *         - Ping to each other between Linux host and CLI Node.
 *
 *
 *
 * ### Extra Comments
 *     * Use AC adapter to ensure a steady power supply.
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
 * \ingroup	EMBARC_APP_BAREMETAL_OPENTHREAD_NCP
 * \brief  example of how to use OpenThread Network Co-Processor (NCP)
 *   This example demonstrates a minimal OpenThread application to show the OpenThread NCP
 *   joins Thread network and communicates with wpantund on Unix-like operating systems.
 */

/**
 * \addtogroup	EMBARC_APP_BAREMETAL_OPENTHREAD_NCP
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"

#include <openthread/config.h>

#include <assert.h>

#include <openthread/diag.h>
#include <openthread/ncp.h>
#include <openthread/openthread.h>
#include <openthread/platform/platform.h>

#define NUM_INPUT_MANUAL (-1)
#define NUM_NCP          (3)

#if OPENTHREAD_ENABLE_MULTIPLE_INSTANCES
void *otPlatCAlloc(size_t aNum, size_t aSize)
{
	return calloc(aNum, aSize);
}

void otPlatFree(void *aPtr)
{
	free(aPtr);
}
#endif

void otTaskletsSignalPending(otInstance *aInstance)
{
	(void)aInstance;
}

int main(void)
{
	otInstance *sInstance;

#if OPENTHREAD_ENABLE_MULTIPLE_INSTANCES
	size_t otInstanceBufferLength = 0;
	uint8_t *otInstanceBuffer = NULL;
#endif
	int argc = 0;
	char **argv = NULL;

	PlatformInit(argc, argv, NUM_INPUT_MANUAL);

#if OPENTHREAD_ENABLE_MULTIPLE_INSTANCES
	// Call to query the buffer size
	(void)otInstanceInit(NULL, &otInstanceBufferLength);

	// Call to allocate the buffer
	otInstanceBuffer = (uint8_t *)malloc(otInstanceBufferLength);
	assert(otInstanceBuffer);

	// Initialize OpenThread with the buffer
	sInstance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);
#else
	sInstance = otInstanceInitSingle();
#endif
	assert(sInstance);

	otNcpInit(sInstance);

#if OPENTHREAD_ENABLE_DIAG
	otDiagInit(sInstance);
#endif

	while (1)
	{
		otTaskletsProcess(sInstance);
		PlatformProcessDrivers(sInstance);
	}

	// otInstanceFinalize(sInstance);
#if OPENTHREAD_ENABLE_MULTIPLE_INSTANCES
	// free(otInstanceBuffer);
#endif

	return 0;
}

/** @} */
