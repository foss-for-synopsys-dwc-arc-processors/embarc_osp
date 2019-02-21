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
--------------------------------------------- */

#ifndef _HSDK_IO_H_
#define _HSDK_IO_H_

#include "../drivers/ip/designware/iic/dw_iic_obj.h"

#define HSDK_I2C_ADDR_CY8C95XX_IO		(0x20)
#define HSDK_I2C_ADDR_CY8C95XX_EEP		(0x40)
#define HSDK_I2C_ID_CY8C95XX			(DW_IIC_0_ID)

#ifdef __cplusplus
extern "C" {
#endif

extern void hsdk_io_init(void);
extern void led_write(uint32_t led_val, uint32_t mask);
extern uint32_t led_read(uint32_t mask);

// Un-implemented Switch/Button functions
// extern void hsdk_button_init(void);
// extern void hsdk_switch_init(void);
// extern uint32_t switch_read(uint32_t mask);
// extern uint32_t button_read(uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif /* _HSDK_IO_H_ */
