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
 * \date 2015-04-24
 * \author Huaqi Fang(Huaqi.Fang@synopsys.com)
--------------------------------------------- */
/**
 * \file
 * \ingroup	BOARD_EMSK_DRV_BLE_HM1X
 * \brief	HM-1X BLE driver
 * \details	implement hm-1x ble driver header file
 */

/**
 * \addtogroup	BOARD_EMSK_DRV_BLE_HM1X
 * @{
 */

#include <stdint.h>

/**
 * HM1X working role: master or slave
 */
#define BLE_HM1X_MASTER_ROLE	1
#define BLE_HM1X_SLAVE_ROLE	0

/**
 * HM1X working mode:
 * 0: transparent
 * 1: PIO sample & remote control & transparent
 * 2: remote control & transparent
 */
#define BLE_HM1X_MODE_0		0
#define BLE_HM1X_MODE_1		1
#define BLE_HM1X_MODE_2		2

/**
 * HM1X working type
 * 0: connect without password
 * 1: simple match
 * 2: match with password
 * 3: match and bind
 */
#define BLE_HM1X_TYPE_0		0
#define BLE_HM1X_TYPE_1		1
#define BLE_HM1X_TYPE_2		2
#define BLE_HM1X_TYPE_3		3

#ifdef __cplusplus
extern "C" {
#endif

extern void hm1x_flush(void);
extern int32_t hm1x_init(uint32_t baudrate);
extern int32_t hm1x_disconnect(void);
extern int32_t hm1x_restart(void);
extern int32_t hm1x_renew(void);

extern int32_t hm1x_baudrate_map(uint32_t baudrate);
extern uint32_t hm1x_write(uint8_t *buf, uint32_t cnt);
extern uint32_t hm1x_read(uint8_t *buf, uint32_t cnt);
extern int32_t hm1x_exec_command(char *cmd, char *resp, uint32_t wait_ms);
extern int32_t hm1x_exec_cmd_chkresp(char *cmd, char *resp_prefix, char *resp_data, uint32_t max_try);

/** only for return resp with OK+Set: */
extern int32_t hm1x_set_param(char *param, char *val);
/** only for return resp with OK+Get: */
extern int32_t hm1x_get_param(char *param, char *r_val);
/** only for return resp with OK+Set: */
extern int32_t hm1x_set_param_u8(char *param, uint8_t val);

extern int32_t hm1x_set_role(uint32_t role);
extern int32_t hm1x_set_mode(uint32_t mode);
extern int32_t hm1x_set_type(uint32_t type);
extern int32_t hm1x_set_baud(uint32_t baudrate);

#ifdef __cplusplus
}
#endif

/** @} end of group BOARD_EMSK_DRV_BLE_HM1X */