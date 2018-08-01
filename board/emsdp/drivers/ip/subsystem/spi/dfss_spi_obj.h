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
#ifndef _DFSS_SPI_OBJ_H_
#define _DFSS_SPI_OBJ_H_

#include "io_config.h"
#include "ip/ip_hal/inc/dev_spi.h"


#define DFSS_SPI_NUM		(4)	/*!< DFSS SPI valid number */

#define DFSS_SPI_BASE_ID	10
#define DFSS_SPI_0_ID		10	/*!< SPI 0 id macro (io_spi_mst0) */
#define DFSS_SPI_1_ID		11	/*!< SPI 1 id macro (io_spi_mst1) */
#define DFSS_SPI_2_ID		12	/*!< SPI 2 id macro (io_spi_mst2) */
#define DFSS_SPI_3_ID		13	/*!< SPI 3 id macro (io_spi_slv0) */

#ifdef IO_SPI_MST0_PRESENT
#define USE_DFSS_SPI_0		1	/*!< enable use DFSS SPI 0 */
#else
#define USE_DFSS_SPI_0		0	/*!< disable use DFSS SPI 0 */
#endif

#ifdef IO_SPI_MST1_PRESENT
#define USE_DFSS_SPI_1		1	/*!< enable use DFSS SPI 1 */
#else
#define USE_DFSS_SPI_1		0	/*!< disable use DFSS SPI 1 */
#endif

#ifdef IO_SPI_MST2_PRESENT
#define USE_DFSS_SPI_2		1	/*!< enable use DFSS SPI 2 */
#else
#define USE_DFSS_SPI_2		0	/*!< disable use DFSS SPI 2 */
#endif

#ifdef IO_SPI_SLV0_PRESENT
#define USE_DFSS_SPI_3		1	/*!< enable use DFSS SPI 3 */
#else
#define USE_DFSS_SPI_3		0	/*!< disable use DFSS SPI 3 */
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern DEV_SPI_PTR dfss_spi_get_dev(int32_t);
extern void dfss_spi_all_install(void);

#ifdef __cplusplus
}
#endif

#endif /* _DFSS_SPI_OBJ_H_ */
