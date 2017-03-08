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
 * \date 2014-08-28
 * \author Huaqi Fang(Huaqi.Fang@synopsys.com)
--------------------------------------------- */
/**
 * \file
 * \ingroup	BOARD_EMSK_DRV_PMODTEMP
 * \brief	header file of emsk pmod temperature sensor driver
 */

/**
 * \addtogroup	BOARD_EMSK_DRV_PMODTEMP
 * @{
 */
#ifndef _EMSK_TEMPERATURE_H_
#define _EMSK_TEMPERATURE_H_

#include "dev_iic.h"

/**
 * I2C address of PmodTmp2 can be selected via jumpers: 0x48, 0x49, 0x4A, 0x4B
 * Default (all jumpers removed) is 0x4B
 */
/* ADT7420 address */
#define ADT7420_A0_PIN      1	/*!< I2C Serial Bus Address Selection Pin */
#define ADT7420_A1_PIN      1	/*!< I2C Serial Bus Address Selection Pin */
#define ADT7420_ADDRESS     (0x48 + (ADT7420_A1_PIN << 1) + ADT7420_A0_PIN)

#define TEMP_I2C_SLAVE_ADDRESS	ADT7420_ADDRESS

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t temp_sensor_init(uint32_t slv_addr);
extern int32_t temp_sensor_read(int32_t *val);

#ifdef __cplusplus
}
#endif

#endif /* _EMSK_TEMPERATURE_H_ */

/** @} end of group BOARD_EMSK_DRV_PMODTEMP */