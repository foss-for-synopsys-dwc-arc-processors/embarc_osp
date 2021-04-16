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

/**
 * \defgroup	DEVICE_HAL_WNIC		Wireless Network Interface Controller Device HAL Interface
 * \ingroup	DEVICE_HAL_DEF
 * \brief	definitions for wnic device hardware layer (\ref dev_wnic.h)
 * \details	provide interfaces for wnic driver to implement
 *  Here is a diagram for the wnic interface.
 *
 *  \htmlonly
 *  <div class="imagebox">
 *      <div style="width: 900px">
 *          <img src="pic/dev_wnic_hal.jpg" alt="WNIC Device HAL Interface Diagram"/>
 *          <p>WNIC Device HAL Interface Diagram</p>
 *      </div>
 *  </div>
 *  \endhtmlonly
 *
 * @{
 *
 * \file
 * \brief	wireless network interface controller device hardware layer definitions
 * \details	provide common definitions for wnic device,
 * 	then software developer can develop wnic driver
 * 	following this definitions, and the applications
 * 	can directly call this definition to realize functions
 */

#ifndef _DEVICE_HAL_WNIC_H_
#define _DEVICE_HAL_WNIC_H_

#include "dev_common.h"

/**
 * \defgroup	DEVICE_HAL_WNIC_CTRLCMD	WNIC Device Control Command
 * \ingroup	DEVICE_HAL_WNIC
 * \brief	definitions for wnic control command, used in \ref dev_wnic::wnic_control
 * @{
 */
/*
 * defines for system control commands of wnic
 * commands need by wnic_control::ctrl_cmd
 */
#define WNIC_CMD_CHG_FREQ			DEV_SET_SYSCMD(0x0)	/*!< ctrl command: change wnic working frequency(Hz) */
#define WNIC_CMD_CHG_MTHD			DEV_SET_SYSCMD(0x1)	/*!< ctrl command: change wnic working method(interrupt or poll) */
/** @} */

/**
 * \defgroup	DEVICE_HAL_WNIC_GETINFOCMD	WNIC Device Get Info Command
 * \ingroup	DEVICE_HAL_WNIC
 * \brief	definitions for wnic get info command, used in \ref dev_wnic::wnic_get_info
 * @{
 */
/*
 * defines for system command to get wnic info
 * commands need by wnic_get_info::cmd
 */
#define WNIC_CMD_GETINFO_ALL			DEV_SET_SYSCMD(0x0)	/*!< get info command: get all info of wnic */
/** @} */

/**
 * \defgroup	DEVICE_HAL_WNIC_MACROS	WNIC Device Enums and Macros
 * \ingroup	DEVICE_HAL_WNIC
 * \brief	wnic related enumations, macros and structures
 * @{
 */
#define WNIC_HDR_LEN				(6)			/*!< mac address length */
#define WNIC_NAME_LEN				(20)			/*!< wnic name length 	*/
#define WNIC_SSID_MAX_LEN			(32)			/*!< wnic max ssid length */
#define WNIC_BSSID_MAX_LEN			(6)			/*!< wnic max bssid length */
#define WNIC_MAX_NUM_RATES			(8)			/*!< wnic max rates number */
#define WNIC_IF_NAME_LEN			(2)			/*!< wnic ifname len */

typedef enum wnic_network_type {
	WNIC_NETWORK_TYPE_MIN			= 1,
	WNIC_NETWORK_TYPE_INFRASTRUCTURE	= 1,
	WNIC_NETWORK_TYPE_ADHOC			= 2,
	WNIC_NETWORK_TYPE_P2P			= 3,
	WNIC_NETWORK_TYPE_SOFTAP		= 4,
	WNIC_NETWORK_TYPE_MAX			= 4
} WNIC_NETWORK_TYPE;

typedef enum wnic_init_state {
	WNIC_NOT_INITIALIZED			= 1,
	WNIC_DURING_INITIALIZATION		= 2,
	WNIC_INIT_SUCCESSFUL			= 3,
	WNIC_INIT_FAILED			= 4
} WNIC_INIT_STATE;

typedef enum wnic_conn_state {
	WNIC_NOT_CONNECTED			= 1,
	WNIC_DURING_CONNECTION			= 2,
	WNIC_DURING_DISCONNECTION		= 3,
	WNIC_CONNECTED				= 4,
	WNIC_CONN_TEMP_LOST			= 5
} WNIC_CONN_STATE;

typedef enum wnic_conn_type {
	WNIC_CONN_NONE				= 0,
	WNIC_CONN_INFRASTRUCTURE		= 1,
	WNIC_CONN_ADHOC				= 2,
	WNIC_CONN_P2P				= 3
} WNIC_CONN_TYPE;

typedef enum wnic_mac_state {
	WNIC_MAC_NOT_UPDATED			= 0,
	WNIC_MAC_UPDATED			= 1
} WNIC_MAC_STATE;

typedef enum wnic_scan_state {
	WNIC_NOT_SCANED				= 0,    /*!< scan not started */
	WNIC_DURING_SCAN			= 1,    /*!< during scan */
	WNIC_SCAN_FINISHED			= 2,    /*!< scan is finished */
	WNIC_SCAN_OUTOFDATE			= 3	/*!< when scan is done, need to update its result */
} WNIC_SCAN_STATE;

typedef enum wnic_power_state {
	WNIC_POWER_OFF				= 0,
	WNIC_POWER_NORMAL			= 1,
	WNIC_POWER_LOW				= 2,
	WNIC_POWER_HIBERNATE			= 3
} WNIC_POWER_STATE;

typedef enum wnic_busy_state {
	WNIC_IS_FREE				= 0,
	WNIC_IS_BUSY				= 1
} WNIC_BUSY_STATE;
/**
 * \brief wnic auth security mode
 */
typedef enum wnic_auth_security_mode
{
    AUTH_SECURITY_OPEN				= 0,
    AUTH_SECURITY_WEP_40			= 1,
    AUTH_SECURITY_WEP_104			= 2,
    AUTH_SECURITY_WPA_WITH_KEY			= 3,
    AUTH_SECURITY_WPA_WITH_PASS_PHRASE		= 4,
    AUTH_SECURITY_WPA2_WITH_KEY			= 5,
    AUTH_SECURITY_WPA2_WITH_PASS_PHRASE		= 6,
    AUTH_SECURITY_WPA_AUTO_WITH_KEY		= 7,
    AUTH_SECURITY_WPA_AUTO_WITH_PASS_PHRASE	= 8,
    AUTH_SECURITY_WPS_PUSH_BUTTON		= 9,
    AUTH_SECURITY_WPS_PIN			= 10,

    AUTH_MAX_SECURITY_TYPE                     = 10
} AUTH_SECURITY_MODE;

typedef struct wnic_cfg_ssid {
	uint8_t ssid[WNIC_SSID_MAX_LEN];
	uint8_t ssid_len;
} WNIC_CFG_SSID, * WNIC_CFG_SSID_PTR;

typedef union wnic_ap_config {
	uint8_t apConfig;
	struct apcfg {
		uint8_t ie:1;
		uint8_t reserved:3;
		uint8_t privacy:1;
		uint8_t preamble:1;
		uint8_t wpa:1;
		uint8_t wpa2:1;
	} ap_cfg;
} WNIC_AP_CONFIG;

typedef struct wnic_auth_key {
	const uint8_t *key;		/*!< WEP key for shared key authentication */
	uint8_t key_len;		/*!< length of WEP key for shared key authentication */
	uint8_t key_idx;		/*!< index of WEP key for shared key authentication */
} WNIC_AUTH_KEY, * WNIC_AUTH_KEY_PTR;

typedef struct wnic_scan_result {
	uint8_t bssid[WNIC_BSSID_MAX_LEN];	/*!< Network BSSID value */
	uint8_t ssid[WNIC_SSID_MAX_LEN];	/*!< Network SSID value  */
	uint8_t ssidlen;			/*!< Number of valid characters in ssid */
	/**
	 * * Access point configuration
	 *   | Bit 7      | Bit 6      | Bit 5      | Bit 4      | Bit 3      | Bit 2      | Bit 1      | Bit 0      |
	 *   | -----      | -----      | -----      | -----      | -----      | -----      | -----      | -----      |
	 *   | WPA2       | WPA        | Preamble   | Privacy    | Reserved   | Reserved   | Reserved   | IE         |
         *
	 * * IE
	 *   - 1 if AP broadcasting one or more Information Elements, else 0
	 * * Privacy
	 *   - 0 : AP is open (no security)
	 *   - 1: AP using security,  if neither WPA and WPA2 set then security is WEP.
	 * * Preamble
	 *   - 0: AP transmitting with short preamble
	 *   - 1: AP transmitting with long preamble
	 * * WPA
	 *   - Only valid if Privacy is 1.
	 *   - 0: AP does not support WPA
	 *   - 1: AP supports WPA
	 * * WPA2
	 *   - Only valid if Privacy is 1.
	 *   - 0: AP does not support WPA2
	 *   - 1: AP supports WPA2
	 */
    WNIC_AP_CONFIG ap_config;
    uint8_t rssi;	/*!< Signal strength of received frame beacon or probe response */
    uint8_t bsstype;	/*!< refer to \ref WNIC_NETWORK_TYPE */
    uint8_t channel;	/*!< channel number */
} WNIC_SCAN_RESULT, * WNIC_SCAN_RESULT_PTR;
/** @} */

/**
 * \defgroup	DEVICE_HAL_WNIC_DEVSTRUCT	WNIC Device Structure
 * \ingroup	DEVICE_HAL_WNIC
 * \brief	contains definitions of wnic device structure.
 * \details	this structure will be used in user implemented code, which was called
 * 	Device Driver Implement Layer for wnic to realize in user code.
 * @{
 */

typedef struct dev_wnic DEV_WNIC, * DEV_WNIC_PTR;
typedef int32_t (*ON_WNIC_OPS)(DEV_WNIC *wnic_ptr);
/**
 * \brief	wnic on state operations
 * 	such as init success, init failed
 * 	hardware assert, connected, disconnected,
 * 	mac updated, scan finished
 */
typedef struct dev_wnic_on_ops {
	int32_t (* on_init_success) (DEV_WNIC *wnic_ptr);
	int32_t (* on_init_fail) (DEV_WNIC *wnic_ptr);
	int32_t (* on_connected) (DEV_WNIC *wnic_ptr);
	int32_t (* on_disconnected) (DEV_WNIC *wnic_ptr);
	int32_t (* on_mac_updated) (DEV_WNIC *wnic_ptr);
	int32_t (* on_scan_finished) (DEV_WNIC *wnic_ptr);
	int32_t (* on_rxdata_comes) (DEV_WNIC *wnic_ptr);
	int32_t (* on_dev_asserted) (DEV_WNIC *wnic_ptr);
} DEV_WNIC_ON_OPS, *DEV_WNIC_ON_OPS_PTR;

/**
 * @brief  WNIC SET ON OPERATIONS
 */
#define WNIC_SET_ON_INIT_SUCCESS(wnic_ptr, on_ops)		\
	((DEV_WNIC *)wnic_ptr)->wnic_on_ops.on_init_success	= (ON_WNIC_OPS )(on_ops)

#define WNIC_SET_ON_INIT_FAIL(wnic_ptr, on_ops)			\
	((DEV_WNIC *)wnic_ptr)->wnic_on_ops.on_init_fail	= (ON_WNIC_OPS )(on_ops)

#define WNIC_SET_ON_CONNECTED(wnic_ptr, on_ops)			\
	((DEV_WNIC *)wnic_ptr)->wnic_on_ops.on_connected	= (ON_WNIC_OPS )(on_ops)

#define WNIC_SET_ON_DISCONNECTED(wnic_ptr, on_ops)		\
	((DEV_WNIC *)wnic_ptr)->wnic_on_ops.on_disconnected	= (ON_WNIC_OPS )(on_ops)

#define WNIC_SET_ON_MAC_UPDATED(wnic_ptr, on_ops)		\
	((DEV_WNIC *)wnic_ptr)->wnic_on_ops.on_mac_updated	= (ON_WNIC_OPS )(on_ops)

#define WNIC_SET_ON_SCAN_FINISHED(wnic_ptr, on_ops)		\
	((DEV_WNIC *)wnic_ptr)->wnic_on_ops.on_scan_finished	= (ON_WNIC_OPS )(on_ops)

#define WNIC_SET_ON_RXDATA_COMES(wnic_ptr, on_ops)		\
	((DEV_WNIC *)wnic_ptr)->wnic_on_ops.on_rxdata_comes	= (ON_WNIC_OPS )(on_ops)

#define WNIC_SET_ON_DEV_ASSERTED(wnic_ptr, on_ops)		\
	((DEV_WNIC *)wnic_ptr)->wnic_on_ops.on_dev_asserted	= (ON_WNIC_OPS )(on_ops)

/**
 * \brief	wnic information struct definition
 * \details	informations about wnic open state, working state,
 * 	frequence, working method
 */
typedef struct dev_wnic_info {
	uint8_t	name[WNIC_NAME_LEN];		/*!< wnic name */
	uint8_t ifname[WNIC_IF_NAME_LEN];	/*!< wnic interface name */
	WNIC_CFG_SSID ssid;			/*!< wnic connected ssid */
	void *ctrl;				/*!< wnic control struct defined by user */
	void *extra;				/*!< wnic extra information used by user */
	uint32_t opn_flg:1;			/*!< indicate wnic open state, 1: opened, 0: closed */
	uint32_t err_flg:1;			/*!< indicate wnic error state, 1: error, 0: good */
	uint32_t network_type;			/*!< wnic network working type (refer to \ref wnic_network_type) */
	uint32_t init_status;			/*!< wnic init state change during initialization (refer to \ref wnic_init_state) */
	uint32_t conn_status;			/*!< wnic connection status (refer to \ref wnic_conn_state) */
	uint32_t conn_type;			/*!< wnic connection type (refer to \ref wnic_conn_type)  */
	uint32_t mac_status;			/*!< wnic mac update status */
	uint32_t scan_status; 			/*!< wnic scan result status */
	uint32_t scan_results;			/*!< wnic scan result list count */
	uint8_t  mac_addr[WNIC_HDR_LEN];	/*!< mac address setting */
	uint32_t busy_status;			/*!< wnic operation busy status */
	uint32_t tx_frame_cnt;			/*!< wnic tx frame count */
	uint32_t tx_speed;			/*!< wnic tx speed bytes per second */
	uint32_t tx_pending;			/*!< wnic tx pending count */
	uint32_t rx_frame_cnt;			/*!< wnic rx frame count */
	uint32_t rx_speed;			/*!< wnic rx speed bytes per second */
	uint32_t rx_pending;			/*!< wnic rx pending count */
	uint32_t power_status;			/*!< wnic power status */
} DEV_WNIC_INFO, * DEV_WNIC_INFO_PTR;

/**
 * \brief	wnic device interface definition
 * \details	define wnic device interface, like wnic information structure,
 * 	functions to get wnic info, open/close/control wnic, send/receive data by wnic
 * \note	all this details are implemented by user in user porting code
 */
struct dev_wnic {
	DEV_WNIC_INFO wnic_info;				/*!< wnic device information */
	DEV_WNIC_ON_OPS wnic_on_ops;				/*!< wnic on operations */
	int32_t (*wnic_get_info) (uint32_t cmd, void *rinfo);	/*!< get wnic information */
	int32_t (*wnic_control) (uint32_t ctrl_cmd, void *param);	/*!< control wnic device */
	int32_t (*wnic_init) (uint32_t network_type);		/*!< init wnic with \ref WNIC_NETWORK_TYPE, bring wnic device into init process, may block some time, and should return imediately when another init is in progress */
	int32_t (*poll_init_status) (void);			/*!< check init status, return immediately */
	int32_t (*poll_busy_status) (void);			/*!< polling device busy status */
	int32_t (*set_network_type) (uint32_t type);		/*!< set network type */
	int32_t (*set_macaddr) (uint8_t *mac);			/*!< set device mac address */
	int32_t (*get_macaddr) (uint8_t *mac);			/*!< get current mac address of wnic device */
	int32_t (*start_scan) (void);				/*!< start trigger a scan, should return immediately when scan is in progress */
	int32_t (*stop_scan) (void);				/*!< if scan is in progress, stop it */
	int32_t (*poll_scan_status) (void);			/*!< poll current scan status */
	int32_t (*get_scan_result_cnt) (void);			/*!< get the count of scan count */
	int32_t (*get_scan_result) (uint32_t index, WNIC_SCAN_RESULT *result); /*!< get scan result in this index */
	int32_t (*wnic_connect) (uint32_t security, const uint8_t *ssid, WNIC_AUTH_KEY *key);	/* connect to an network */
	int32_t (*poll_conn_status) (void);			/*!< poll connection status */
	int32_t (*wnic_disconnect) (void);			/*!< if connected to one network, disconnect it */
	int32_t (*prepare_tx) (uint32_t tx_len);		/*!< prepare for transmit data */
	int32_t (*add_tx_data) (uint8_t *p_buf, uint32_t len);	/*!< add tx data to tx buffer, buffer index will increase by adding, if success return bytes_add count */
	int32_t (*commit_tx) (uint32_t len);			/*!< commit data in tx buffer, total data size that has add to tx buffer */
	int32_t (*prepare_rx) (void);				/*!< prepare for receive data, return data bytes length available in rx buffer */
	int32_t (*get_rx_data) (uint8_t *p_buf, uint32_t len);	/*!< get data from rx buffer, when get success, buffer index will increase and return bytes_get count */
	int32_t (*accept_rx) (void);				/*!< tell the rx process data has been acknowledged by the application, the rx buffer can free now */
	int32_t (*config_power_mode) (int32_t power_mode);	/*!< change wnic device power state, refer to \ref WNIC_POWER_STATE */
	int32_t (*poll_power_mode) (void);			/*!< poll current power status of wnic device */
	int32_t (*wnic_reset) (void);				/*!< bring wnic device into reset */
	void (*period_process) (void *ptr);			/*!< processing wnic events, should be periodic called as fast as possible after wnic is initialized, or as a periodic task */
};
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief	get an \ref dev_wnic "wnic device" by wnic device id.
 *  	For how to use wnic device hal refer to \ref dev_wnic "Functions in wnic device structure"
 * \param[in]	wnic_id		id of wnic, defined by user
 * \retval	!NULL		pointer to an wnic device structure (\ref dev_wnic)
 * \retval	NULL		failed to find the wnic device by \ref wnic_id
 * \note	need to implemented by user in user code
 */
extern DEV_WNIC_PTR wnic_get_dev(int32_t wnic_id);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _DEVICE_HAL_WNIC_H_ */
