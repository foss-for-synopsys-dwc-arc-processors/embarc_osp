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
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */

/**
 * \file
 * \ingroup	BOARD_EMSK_DRV_RW009
 * \brief	rw009 wifi module driver
 */

#ifndef _RW009_H_
#define _RW009_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dev_wnic.h"

extern int32_t rw009_wnic_get_info(DEV_WNIC *rw009_wnic, uint32_t cmd, void *rinfo);
extern int32_t rw009_wnic_control(DEV_WNIC *rw009_wnic, uint32_t ctrl_cmd, void *param);
extern int32_t rw009_wnic_init(DEV_WNIC *rw009_wnic, uint32_t network_type);
extern int32_t rw009_poll_init_status(DEV_WNIC *rw009_wnic);
extern int32_t rw009_poll_busy_status(DEV_WNIC *rw009_wnic);
extern int32_t rw009_set_network_type(DEV_WNIC *rw009_wnic, uint32_t type);
extern int32_t rw009_set_macaddr(DEV_WNIC *rw009_wnic, uint8_t *mac);
extern int32_t rw009_get_macaddr(DEV_WNIC *rw009_wnic, uint8_t *mac);
extern int32_t rw009_start_scan(DEV_WNIC *rw009_wnic);
extern int32_t rw009_stop_scan(DEV_WNIC *rw009_wnic);
extern int32_t rw009_poll_scan_status(DEV_WNIC *rw009_wnic);
extern int32_t rw009_get_scan_result_cnt(DEV_WNIC *rw009_wnic);
extern int32_t rw009_get_scan_result(DEV_WNIC *rw009_wnic, uint32_t index, WNIC_SCAN_RESULT *result);
extern int32_t rw009_wnic_connect(DEV_WNIC *rw009_wnic, uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key);
extern int32_t rw009_poll_conn_status(DEV_WNIC *rw009_wnic);
extern int32_t rw009_wnic_disconnect(DEV_WNIC *rw009_wnic);
extern int32_t rw009_prepare_tx(DEV_WNIC *rw009_wnic, uint32_t tx_len);
extern int32_t rw009_add_tx_data(DEV_WNIC *rw009_wnic, uint8_t *p_buf, uint32_t len);
extern int32_t rw009_commit_tx(DEV_WNIC *rw009_wnic, uint32_t len);
extern int32_t rw009_prepare_rx(DEV_WNIC *rw009_wnic);
extern int32_t rw009_get_rx_data(DEV_WNIC *rw009_wnic, uint8_t *p_buf, uint32_t len);
extern int32_t rw009_accept_rx(DEV_WNIC *rw009_wnic);
extern int32_t rw009_config_power_mode(DEV_WNIC *rw009_wnic, int32_t power_mode);
extern int32_t rw009_poll_power_mode(DEV_WNIC *rw009_wnic);
extern int32_t rw009_wnic_reset(DEV_WNIC *rw009_wnic);
extern void rw009_period_process(DEV_WNIC *rw009_wnic, void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* _RW009_H_ */
