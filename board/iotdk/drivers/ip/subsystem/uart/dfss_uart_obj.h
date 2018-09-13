/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
#ifndef _DFSS_UART_H_
#define _DFSS_UART_H_

#include "io_config.h"
#include "ip/ip_hal/inc/dev_uart.h"

#ifdef IO_UART0_PRESENT
#define USE_DFSS_UART_0				1	/*!< enable use DFSS UART 0 */
#else
#define USE_DFSS_UART_0				0	/*!< disable use DFSS UART 0 */
#endif

#ifdef IO_UART1_PRESENT
#define USE_DFSS_UART_1				1	/*!< enable use DFSS UART 1 */
#else
#define USE_DFSS_UART_1				0	/*!< disable use DFSS UART 1 */
#endif

#ifdef IO_UART2_PRESENT
#define USE_DFSS_UART_2				1	/*!< enable use DFSS UART 2 */
#else
#define USE_DFSS_UART_2				0	/*!< disable use DFSS UART 2 */
#endif

#ifdef IO_UART3_PRESENT
#define USE_DFSS_UART_3				1	/*!< enable use DFSS UART 3 */
#else
#define USE_DFSS_UART_3				0	/*!< disable use DFSS UART 3 */
#endif


#define DFSS_UART_NUM	(4)	/*!< DFSS UART valid number */

#define DFSS_UART_0_ID		0	/*!< UART 0 ID macro */
#define DFSS_UART_1_ID		1	/*!< UART 1 ID macro */
#define DFSS_UART_2_ID		2	/*!< UART 2 ID macro */
#define DFSS_UART_3_ID		3	/*!< UART 3 ID macro */

#ifdef __cplusplus
extern "C" {
#endif

extern void dfss_uart_all_install(void);

#ifdef __cplusplus
}
#endif

#endif /* _DFSS_UARTH_ */