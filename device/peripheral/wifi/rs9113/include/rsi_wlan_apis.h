/**
 * @file     rsi_wlan_apis.h
 * @version  0.1
 * @date     15 Aug,2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contain definitions and declarations of wlan APIs
 *
 *  @section Description  This file contains definitions and declarations related to
 *  wlan to configure wlan module
 *
 *
 */

#ifndef RSI_WLAN_APIS_H
#define RSI_WLAN_APIS_H

#include <rsi_data_types.h>

/******************************************************
 * *                      Macros
 * ******************************************************/

//! Enable feature
#define RSI_ENABLE                            1
//! Disable feature
#define RSI_DISABLE                           0

//! Enable profile config
#define RSI_ENABLE_CFG_PROFILE                2


//! eap username length
#define RSI_EAP_USER_NAME_LENGTH          64

//! eap password length
#define RSI_EAP_PASSWORD_LENGTH           128

//! total no of sockets
#define RSI_MN_NUM_SOCKETS                10

//! max no of stations
#define RSI_MAX_STATIONS                  8

//! Maximum length of SSID
#define RSI_SSID_LEN                      34

#define RSI_PSK_LEN                       64
#define RSI_PMK_LEN                       32

//! Maximum Acccess points that can be scanned
#define RSI_AP_SCANNED_MAX                11

//! MAC address length
#define RSI_MAC_ADDR_LEN                  6

//! max no of call backs
#define RSI_MAX_NUM_CALLBACKS            12

//! Maximum wifi direct device count
#define RSI_MAX_WFD_DEVICE_COUNT          10

#define RSI_INSTANT_BGSCAN                 1

#define RSI_BGSCAN_STOP                    0

//! feature bit map
/*=========================================================================*/
//! Securtity type open
#define FEAT_SECURITY_OPEN               BIT(0)

//! Securtity type WPA/WPA2
#define FEAT_SECURITY_PSK                BIT(1)

//! Aggregation support
#define FEAT_AGGREGATION                 BIT(2)

//! LP mode GPIO handshake
#define FEAT_LP_GPIO_BASED_HANDSHAKE     BIT(3)

//! ULP mode GPIO based handshake
#define FEAT_ULP_GPIO_BASED_HANDSHAKE    BIT(4)

//! To select ULP GPIO 1 for wake up indication
#define FEAT_DEV_TO_HOST_ULP_GPIO_1      BIT(5)

//! To supply 3.3 volt supply
#define FEAT_RF_SUPPY_VOL_3_3_VOLT       BIT(6)

//! To Disable WPS in AP mode
#define FEAT_WPS_DISABLE                 BIT(7)


/*=========================================================================*/

//! TCP/IP feature bit map parameters description !//
//! TCP/IP bypass feature
#define TCP_IP_FEAT_BYPASS               BIT(0)

//! Enable HTTP server feature
#define TCP_IP_FEAT_HTTP_SERVER          BIT(1)

//! Enable DHCPv4 client feature
#define TCP_IP_FEAT_DHCPV4_CLIENT        BIT(2)

//! Enable DHCPv6 client feature
#define TCP_IP_FEAT_DHCPV6_CLIENT        BIT(3)

//! Enable DHCPv4 server feature
#define TCP_IP_FEAT_DHCPV4_SERVER        BIT(4)

//! Enable DHCPv6 server feature
#define TCP_IP_FEAT_DHCPV6_SERVER        BIT(5)

//! Enable JSON objects
#define TCP_IP_FEAT_JSON_OBJECTS         BIT(6)

//! Enable HTTP client
#define TCP_IP_FEAT_HTTP_CLIENT          BIT(7)

//! Enable DNS client
#define TCP_IP_FEAT_DNS_CLIENT           BIT(8)

//! Enable SNMP client
#define TCP_IP_FEAT_SNMP_AGENT           BIT(9)

//! Enable SSL feature
#define TCP_IP_FEAT_SSL                  BIT(10)

//! Enable ICMP feature(ping)
#define TCP_IP_FEAT_ICMP                 BIT(11)

//! Enable HTTP client
#define TCP_IP_FEAT_HTTPS_SERVER         BIT(12)

//! Enable FTP client
#define TCP_IP_FEAT_FTP_CLIENT           BIT(15)

//! Enable SNTP client
#define TCP_IP_FEAT_SNTP_CLIENT		       BIT(16)

//! Enable  IPV6 support
#define TCP_IP_FEAT_IPV6                 BIT(17)

//! Enable  Raw data support
#define TCP_IP_FEAT_RAW_DATA             BIT(18)

//! Enable MDNSD
#define TCP_IP_FEAT_MDNSD		             BIT(19)

//! Enable SMTP client
#define TCP_IP_FEAT_SMTP_CLIENT		       BIT(20)

//! Enable Single SSL socket
#define TCP_IP_FEAT_SINGLE_SSL_SOCKET		 BIT(25)

//! Enable POP3 client
#define TCP_IP_FEAT_POP3_CLIENT	      	 BIT(29)

//! Enable OTAF client
#define TCP_IP_FEAT_OTAF   	      	     BIT(30)

#define TCP_IP_FEAT_EXTENSION_VALID     	     BIT(31)

//! Enable to Load public and private keys for TLS and SSL handshake
#define TCP_IP_FEAT_LOAD_PUBLIC_PRIVATE_CERTS   BIT(26)

#define EXT_TCP_FEAT_DHCP_OPT77      BIT(1)

//! Enable HTTP server root path (Default configuration page) bypass
#define EXT_TCP_IP_HTTP_SERVER_BYPASS   BIT(2)

//! Enabling this bit will give both IPV4 and IPV6 addresses in GOPARAMS command response
#define EXT_TCP_IP_GOPARAMS        BIT(3)

//! Enable this, to parse parameters in URL
#define EXT_TCP_IP_HTTP_URL        BIT(4)


//! Select No of sockets

#define TCP_IP_TOTAL_SOCKETS_1     (1<<21)
#define TCP_IP_TOTAL_SOCKETS_2     (2<<21)
#define TCP_IP_TOTAL_SOCKETS_3     (3<<21)
#define TCP_IP_TOTAL_SOCKETS_4     (4<<21)
#define TCP_IP_TOTAL_SOCKETS_5     (5<<21)
#define TCP_IP_TOTAL_SOCKETS_6     (6<<21)
#define TCP_IP_TOTAL_SOCKETS_7     (7<<21)
#define TCP_IP_TOTAL_SOCKETS_8     (8<<21)
#define TCP_IP_TOTAL_SOCKETS_9     (9<<21)
#define TCP_IP_TOTAL_SOCKETS_10    (10<<21)


//! Custom feature bit map paramters description !//

//! If this bit is set, AP is created in hidden mode
//! This bit is valid only in case of AP mode
#define CUSTOM_FEAT_AP_IN_HIDDEN_MODE        BIT(5)

//! Support for scanning in DFS channels() in 5GHZ band
//! This bit is valid in WiFi client mode
#define CUSTOM_FEAT_DFS_CHANNEL_SUPPORT      BIT(8)

//! If this bit is set it enables the LED blinking feature
//! after module initialisation.LED (GPIO_16) is used for
//! this feature and blinks when any TX Packet on air happens
//! and when we get a unicast packets addressing to our MAC
#define CUSTOM_FEAT_LED_FEATURE              BIT(9)


//! If this bit is enabled,module indicates the host
//! the wlan connection status asynchronously
//! This bit is valid in case of Wifi client mode
#define CUSTOM_FEAT_ASYNC_CONNECTION_STATUS  BIT(10)

//!
#define CUSTOM_FEAT_WAKE_ON_WIRELESS         BIT(11)


#define CUSTOM_FEAT_BT_IAP                 BIT(29)


//! Extention valid to use Extended custom feature bitmap
#define FEAT_CUSTOM_FEAT_EXTENTION_VALID         BIT(31)


//! Extended custom feature bitmap !//

//! To support 4096 size RSA KEY certificate
#define EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT       BIT(1)

//! Extended custom bitmap to support TELEC
#define EXT_FEAT_TELEC_SUPPORT                   BIT(2)

//! To support 4096 size KEY SSL certificate
#define EXT_FEAT_SSL_CERT_WITH_4096_KEY_SUPPORT  BIT(3)

//! Extended custom bitmap for AP Broadcast customization
#define EXT_FEAT_AP_BROADCAST_PKT_SND_B4_DTIM    BIT(4)

//! Extended custom bitmap to support FCC
#define EXT_FEAT_FCC_LOW_PWR    		         BIT(5)

//! To enable PUF
#define EXT_FEAT_PUF						     BIT(7)

//! Nokia Spectral mask extended custom bitmap
#define EXT_FEAT_SPECTRAL_MASK_NOKIA             BIT(8)

//! Extended feature bit map to skip default leading character '\' in HTTP header
#define EXT_HTTP_SKIP_DEFAULT_LEADING_CHARACTER  BIT(9)

//! To enable PUF private key
#define EXT_FEAT_PUF_PRIVATE_KEY                 BIT(10)

//! BAND COMMAND PARAMETERS OPTIONS
/*=========================================================================*/

//! module operates in 2.4GHz band
#define RSI_BAND_2P4GHZ                       0

//! Module operates in 5GHz band
#define RSI_BAND_5GHZ                         1

//! Module opertes in both 2.4GHz and 5GHz  band
//! This option is valid only in case of Wifi-Direct
//! and WiFi client mode
#define RSI_DUAL_BAND                         2

/*=========================================================================*/



//! SCAN COMMAND PARAMETERS OPTIONS
/*=========================================================================*/

//! Scan feature bitmap paramters !//

//! If this bit is set,module scans for the AP given in scan API
//! and posts the scan results immediately to the host after finding
//! one Accesspoint.This bit is valid only if specific channel and
//! ssid to scan is given.
#define RSI_ENABLE_QUICK_SCAN              BIT(0)

/*=========================================================================*/


//! ACCESS POINT CONFIGURATION PARAMETERS DESCRIPTIONS
/*=========================================================================*/

//! AP keep alive type bit !//

//! If this bit is enabled, AP performs keep alive functionality
//! by sending NULL DATA packet to the station. If no ACK is received
//! from the station after specific no of retries, AP discards the station
#define RSI_NULL_BASED_KEEP_ALIVE                (BIT(0) | BIT(1))

//! If this bit is enabled, AP performs keep alive functionality
//! based on the rx packets received from its stations. If no packet is
//! received from the station with in time out, AP discards it
#define RSI_DEAUTH_BASED_KEEP_ALIVE              BIT(0)



//! HT capabilities bitmap paramters !//
//! Bit map corresponding to high throughput capabilities
//! HT capabilities bitmap is valid only if HT capabilites RSI_ENABLE_HT_CAPABILITIES
//! macro is enabled.

//! If this bit is set,Short GI support for 20Mhz band width
#define RSI_ENABLE_SHORT_GI                       BIT(5)

//! If these bit is set, RX STBC support is enabled in AP mode
#define RSI_ENABLE_RX_STBC_SUPPORT               (BIT(8) | ~BIT(9))

//! If these bit is set, Green field support is enabled in AP mode
#define RSI_ENABLE_GREEN_FIELD_SUPPORT           BIT(4)

/*=========================================================================*/






//! JOIN COMMAND PARAMETERS OPTIONS
/*=========================================================================*/

//! Tx Power level !//
//! Low Tx power level
//! (7+/-1)dBm in 2.4GHz band
//! (5+/-1)dBm in 5GHz band
#define RSI_POWER_LEVEL_LOW                0

//! Medium Tx power level !//
//! (5+/-1)dBm in 2.4GHz band
//! (7+/-1)dBm in 5GHz band
#define RSI_POWER_LEVEL_MEDIUM             1

//! High Tx power level !//
//! Highest power that can be
//! used in that channel
#define RSI_POWER_LEVEL_HIGH               2


//! DATA Rates used //!
#define RSI_DATA_RATE_AUTO                  0
#define RSI_DATA_RATE_1                     1
#define RSI_DATA_RATE_2                     2
#define RSI_DATA_RATE_5P5                   3
#define RSI_DATA_RATE_11                    4
#define RSI_DATA_RATE_6                     5
#define RSI_DATA_RATE_9                     6
#define RSI_DATA_RATE_12                    7
#define RSI_DATA_RATE_54                    12
#define RSI_DATA_RATE_20                    20
#define RSI_DATA_RATE_18                    18

/*=========================================================================*/





//! Multicast filter cmds
/*=========================================================================*/
#define RSI_MULTICAST_MAC_ADD_BIT        0
#define RSI_MULTICAST_MAC_CLEAR_BIT      1
#define RSI_MULTICAST_MAC_CLEAR_ALL      2
#define RSI_MULTICAST_MAC_SET_ALL        3

/*=========================================================================*/





//! SSL features
/*=========================================================================*/
#define SSL_ALL_CIPHERS 0
#define BIT_TLS_RSA_WITH_AES_256_CBC_SHA256    (1<<0)
#define BIT_TLS_RSA_WITH_AES_128_CBC_SHA256    (1<<1)
#define BIT_TLS_RSA_WITH_AES_256_CBC_SHA       (1<<2)
#define BIT_TLS_RSA_WITH_AES_128_CBC_SHA       (1<<3)
#define BIT_TLS_RSA_WITH_AES_128_CCM_8         (1<<4)
#define BIT_TLS_RSA_WITH_AES_256_CCM_8         (1<<5)
/*=========================================================================*/

//! ssl version
#define RSI_SSL_V_1                             BIT(2)
#define RSI_SSL_V_2                             BIT(3)


//! socket feature
/*=======================================================================*/

//! Type of service. possible values are 0 to 7
#define RSI_TOS                        0

//! power save handshake types
/*=======================================================================*/
//! Type of service. possible values are 0 to 7
#define MSG_BASED                         1
#define GPIO_BASED                        2

/*=======================================================================*/

//! PER mode values
/*=======================================================================*/
//! Possible data rates
#define RSI_RATE_1                        0x0
#define RSI_RATE_2                        0x2
#define RSI_RATE_5_5                      0x4
#define RSI_RATE_11                       0x6
#define RSI_RATE_6                        0x8b
#define RSI_RATE_9                        0x8f
#define RSI_RATE_12                       0x8a
#define RSI_RATE_18                       0x8e
#define RSI_RATE_24                       0x89
#define RSI_RATE_36                       0x8d
#define RSI_RATE_48                       0x88
#define RSI_RATE_54                       0x8c
#define RSI_RATE_MCS0                     0x100
#define RSI_RATE_MCS1                     0x101
#define RSI_RATE_MCS2                     0x102
#define RSI_RATE_MCS3                     0x103
#define RSI_RATE_MCS4                     0x104
#define RSI_RATE_MCS5                     0x105
#define RSI_RATE_MCS6                     0x106
#define RSI_RATE_MCS7                     0x107
#define RSI_RATE_MCS7_SG                  0x307



/*=========================================================================*/



/*=========================================================================*/



//! Transmit test supported modes
#define RSI_BURST_MODE                     0
#define RSI_CONTINUOUS_MODE                1

/*=======================================================================*/

//! Power save LP/ULP sleep type
/*=======================================================================*/
#define RSI_LP_MODE 				               0
#define RSI_ULP_WITH_RAM_RET               1
#define RSI_ULP_WITHOUT_RAM_RET            2

/*=======================================================================*/

//! Antenna Selection parameters
/*=======================================================================*/
#define RSI_TYPE_RF_OUT_2                  0         //! Internal Antenna
#define RSI_TYPE_RF_OUT_1                  1         //! uFL connector

#define RSI_ANTENNA_PATH_INTERNAL          1
#define RSI_ANTENNA_PATH_EXTERNAL          2

#define RSI_ANTENNA_TYPE_REDPINE           1
#define RSI_ANTENNA_TYPE_FRACTUS           2
#define RSI_ANTENNA_TYPE_MOLEX             3

/*=======================================================================*/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

//! enumerations for security type of access point to join
typedef enum rsi_security_mode_e
{
  //! open mode
  RSI_OPEN = 0,
  //! WPA security with PSK
  RSI_WPA,
  //! WPA2 security with PSK
  RSI_WPA2,
  //! WEP security
  RSI_WEP,
  //! Enterprise WPA security
  RSI_WPA_EAP,
  //! Enterprise WPA2 security
  RSI_WPA2_EAP,
  //! Enterprise WPA2/WPA security
  RSI_WPA_WPA2_MIXED,
  //! WPA security with PMK
  RSI_WPA_PMK,
  //! WPA2 security with PMK
  RSI_WPA2_PMK,
  //! WPS pin method
  RSI_WPS_PIN,
  //! WPS generated pin method
  RSI_USE_GENERATED_WPSPIN,
  //! WPS push button method
  RSI_WPS_PUSH_BUTTON,

}rsi_security_mode_t;

//! enumerations for call back types
typedef enum rsi_callback_id_e
{
  RSI_JOIN_FAIL_CB                           =   0,
  RSI_IP_FAIL_CB                             =   1,
  RSI_REMOTE_SOCKET_TERMINATE_CB             =   2,
  RSI_IP_CHANGE_NOTIFY_CB                    =   3,
  RSI_STATIONS_DISCONNECT_NOTIFY_CB          =   4,
  RSI_STATIONS_CONNECT_NOTIFY_CB             =   5,
  RSI_WLAN_DATA_RECEIVE_NOTIFY_CB            =   6,
  RSI_WLAN_WFD_DISCOVERY_NOTIFY_CB           =   7,
  RSI_WLAN_RECEIVE_STATS_RESPONSE_CB         =   8,
  RSI_WLAN_WFD_CONNECTION_REQUEST_NOTIFY_CB  =   9,
  RSI_WLAN_SCAN_RESPONSE_HANDLER             =   10,
  RSI_WLAN_JOIN_RESPONSE_HANDLER             =   11,
  RSI_WLAN_RAW_DATA_RECEIVE_HANDLER          =   12
}rsi_callback_id_t;

//! enumerations for encryption types
typedef enum rsi_encryption_mode_e
{
  RSI_NONE,
  RSI_TKIP,
  RSI_CCMP
}rsi_encryption_mode_t;

//! enumerations for certificates types
typedef enum rsi_certificate_type_e
{
  RSI_EAP_CLIENT                 = 1,
  RSI_FAST_PAC_FILE              = 2,
  RSI_SSL_CLIENT                 = 3,
  RSI_SSL_CLIENT_PRIVATE_KEY     = 4,
  RSI_SSL_CA_CERTIFICATE         = 5,
  RSI_SSL_SERVER_CERTIFICATE     = 6,
  RSI_SSL_SERVER_PRIVATE_KEY     = 7,
  RSI_EAP_PRIVATE_KEY            = 17,
  RSI_EAP_PUBLIC_KEY             = 33,
  RSI_EAP_CA_CERTIFICATE         = 49

}rsi_certificate_type_t;

//! enumerations for set commands
typedef enum rsi_wlan_set_cmd_e
{
  RSI_SET_MAC_ADDRESS           = 1,
  RSI_MULTICAST_FILTER          = 2

}rsi_wlan_set_cmd_t;

//! enumerations for query commands
typedef enum rsi_wlan_query_cmd_e
{
  RSI_FW_VERSION                = 1,
  RSI_MAC_ADDRESS               = 2,
  RSI_RSSI                      = 3,
  RSI_WLAN_INFO                 = 4,
  RSI_CONNECTION_STATUS         = 5,
  RSI_STATIONS_INFO             = 6,
  RSI_SOCKETS_INFO              = 7

}rsi_wlan_query_cmd_t;

/******************************************************
 * *                 Type Definitions
 * ******************************************************/
//! wifi direct device info structure
typedef struct rsi_wfd_device_info_s
{
    //! If New device  1; Device left 0
    uint8_t  device_state;

    //! Name the device found or left 32 bytes
    uint8_t  device_name[32];

    //! Mac address of the device
    uint8_t  mac_address[6];

    //! Type of the device 1st byte inidcates primary device type;
    //! 2nd byte indicates sub catagory
    uint8_t  device_type[2];

}rsi_wfd_device_info_t;

//! wifi direct device info response structure
typedef struct  rsi_rsp_wfd_device_info_s
{
       //! device count
       uint8_t   device_count;

       //! 32 maximum responses from scan command
       rsi_wfd_device_info_t   wfd_dev_info[RSI_MAX_WFD_DEVICE_COUNT];

} rsi_rsp_wfd_device_info_t;

/*P2P connection request from wi-fi device*/
typedef struct rsi_rsp_p2p_connection_request_s
{
  //! device name
  uint8_t   device_name[32];

}rsi_rsp_p2p_connection_request_t;


//! Scan information response structure
typedef struct rsi_scan_info_s
{
  //! channel number of the scanned AP
  uint8_t rf_channel;

  //! security mode of the scanned AP
  uint8_t security_mode;

  //! rssi value of the scanned AP
  uint8_t rssi_val;

  //! network type of the scanned AP
  uint8_t network_type;

  //! SSID of the scanned AP
  uint8_t ssid[RSI_SSID_LEN];

  //! BSSID of the scanned AP
  uint8_t bssid[RSI_MAC_ADDR_LEN];

  //! reserved
  uint8_t reserved[2];
} rsi_scan_info_t;

//! multicast command request structure
typedef struct rsi_req_multicast_filter_info_s
{
  //! cmd_type
  uint8_t    cmd_type;

  //! MAC address to apply filter
  uint8_t    mac_address[6];

} rsi_req_multicast_filter_info_t;

//! wireless information
typedef struct rsi_rsp_wireless_info_s
{
    //! wlan state: connected or disconnected in station mode
    //! wlan state: no of stations connected in AP mode
    uint16_t  wlan_state;

    //! channel number of connected AP
    uint16_t  channel_number;

    //! uint8[32], SSID of connected access point
    uint8_t  ssid[RSI_SSID_LEN];

    //! Mac address
    uint8_t  mac_address[6];

    //! security type
    uint8_t  sec_type;

    //! PSK
    uint8_t  psk[64];

    //! uint8[4], Module IP Address
    uint8_t  ipv4_address[4];

    //! uint8[16], Module IPv6 Address
    uint8_t  ipv6_address[16];

    //! reserved1
    uint8_t  reserved1[2];

    //! reserved2
    uint8_t  reserved2[2];

} rsi_rsp_wireless_info_t;

typedef struct  rsi_go_sta_info_s
{
   //! IP version if the connected client
   uint8_t   ip_version[2];

   //! Mac Address of the connected client
   uint8_t   mac[6];
  union
  {
    //! IPv4 Address of the Connected client
    uint8_t   ipv4_address[4];

    //! IPv6 Address of the Connected client
    uint8_t   ipv6_address[16];
  }ip_address;

}rsi_go_sta_info_t;

//! stations information
typedef struct rsi_rsp_stations_info_s
{
    //! Number of stations Connected to GO
    uint8_t  sta_count[2];

    rsi_go_sta_info_t sta_info[RSI_MAX_STATIONS];

} rsi_rsp_stations_info_t;


typedef struct wep_key_ds_s
{
	uint8_t index[2];
	uint8_t key[4][32];
} wep_key_ds_t;

//! Store config profile structure

typedef struct network_profile
{
  uint8_t     tcp_stack_used;

  uint8_t     dhcp_enable;
	uint8_t     ip_address[4];
	uint8_t     sn_mask[4];
	uint8_t     default_gw[4];

	uint8_t     dhcp6_enable;
	uint8_t     prefix_length[2];
	uint8_t     ip6_address[16];
  uint8_t     deafault_gw[16];

} network_profile_t;

typedef struct ap_profile
{
  uint8_t    ap_profile_magic_word[4];
  uint8_t    wlan_feature_bit_map[4];
  uint8_t    tcp_ip_feature_bit_map[4];
  uint8_t    custom_feature_bit_map[4];
  uint8_t    data_rate;
  uint8_t    tx_power;
  uint8_t    band;
  uint8_t    channel[2];
  uint8_t    ssid[RSI_SSID_LEN];
  uint8_t    security_type;
  uint8_t    encryption_type;
  uint8_t    psk[RSI_PSK_LEN];
  uint8_t    beacon_interval[2];
  uint8_t    dtim_period[2];
  uint8_t    keep_alive_type;
  uint8_t    keep_alive_counter;
  uint8_t    max_no_sta[2];

  //! Network config profie
  network_profile_t network_profile;

} ap_profile_t;

typedef struct client_profile
{
  uint8_t     client_profile_magic_word[4];
  uint8_t     wlan_feature_bit_map[4];
  uint8_t     tcp_ip_feature_bit_map[4];
  uint8_t     custom_feature_bit_map[4];
  uint8_t     listen_interval[4];
  uint8_t     data_rate;
  uint8_t     tx_power;
  uint8_t     band;
  uint8_t     ssid[RSI_SSID_LEN];
  uint8_t     ssid_len;
  uint8_t     channel[2];
  uint8_t     scan_feature_bitmap;
  uint8_t     scan_chan_bitmap_magic_code[2];
  uint8_t     scan_chan_bitmap_2_4_ghz[4];
  uint8_t     scan_chan_bitmap_5_0_ghz[4];
  uint8_t     security_type;
  uint8_t     encryption_type;
  uint8_t     psk[RSI_PSK_LEN];
  uint8_t     pmk[RSI_PMK_LEN];
  wep_key_ds_t wep_key;

  //! Network config profie
  network_profile_t network_profile;

} client_profile_t;

typedef struct eap_client_profile
{
  uint8_t     eap_profile_magic_word[4];
  uint8_t     wlan_feature_bit_map[4];
  uint8_t     tcp_ip_feature_bit_map[4];
  uint8_t     custom_feature_bit_map[4];
  uint8_t     listen_interval;
  uint8_t     data_rate;
  uint8_t     tx_power;
  uint8_t     band;
  uint8_t     ssid[RSI_SSID_LEN];
  uint8_t     ssid_len;
  uint8_t     channel[2];
  uint8_t     scan_feature_bitmap;
  uint8_t     scan_chan_bitmap_magic_code[2];
  uint8_t     scan_chan_bitmap_2_4_ghz[4];
  uint8_t     scan_chan_bitmap_5_0_ghz[4];
  uint8_t     security_type;
  uint8_t     eap_method[32];
  uint8_t     inner_method[32];
  uint8_t     user_identity[64];
  uint8_t     passwd[128];

  //! Network config profie
  network_profile_t network_profile;
} eap_client_profile_t;


typedef struct p2p_profile
{

  uint8_t     p2p_profile_magic_word[4];
  uint8_t     wlan_feature_bit_map[4];
  uint8_t     tcp_ip_feature_bit_map[4];
  uint8_t     custom_feature_bit_map[4];
  uint8_t     data_rate;
  uint8_t     tx_power;
  uint8_t     band;
  uint8_t     join_ssid[RSI_SSID_LEN];
  uint8_t     go_intent[2];
  uint8_t     device_name[64];
  uint8_t     operating_channel[2];
  uint8_t     ssid_postfix[64];
  uint8_t     psk_key[64];;

  //! Network config profie
  network_profile_t network_profile;

} p2p_profile_t;





typedef struct rsi_config_profile_s
{
  uint8_t     profile_type[4];

  union
  {
    //! AP config profile
    ap_profile_t      ap_profile;

    //! Client config profile
    client_profile_t  client_profile;

    //! EAP client config profile
    eap_client_profile_t eap_client_profile;

    //! P2P config profile
    p2p_profile_t  p2p_profile;

  } wlan_profile_struct;

} rsi_config_profile_t;

typedef struct rsi_profile_req_s
{
  uint8_t     profile_type[4];

} rsi_profile_req_t;

typedef struct rsi_auto_config_enable_s
{
   uint8_t    config_enable;
   uint8_t    profile_type[4];

} rsi_auto_config_enable_t;

typedef struct rsi_req_debug_print_s
{
   uint8_t assertion_type[4];
   uint8_t assertion_value[4];

} rsi_req_debug_print_t;


//! socket information structure
typedef struct rsi_sock_info_query_s
{
    //! 2 bytes, socket id
    uint8_t  sock_id[2];

    //! 2 bytes, socket type
    uint8_t  sock_type[2];

    //! 2 bytes, source port number
    uint8_t  source_port[2];

    //! 2 bytes, remote port number
    uint8_t  dest_port[2];

    union{
    //!  remote IPv4 Address
        uint8_t   ipv4_address[4];

    //!  remote IPv6 Address
        uint8_t   ipv6_address[16];

      }dest_ip_address;

}rsi_sock_info_query_t;

//! sockets information
typedef struct rsi_rsp_sockets_info_s
{
    //! number of sockets opened
    uint8_t  num_open_socks[2];

    //!sockets information array
    rsi_sock_info_query_t   socket_info[RSI_MN_NUM_SOCKETS];

} rsi_rsp_sockets_info_t;

//! Scan command response structure
typedef struct rsi_rsp_scan_s
{
  //! number of access points scanned
  uint8_t scan_count[4];

  //! reserved
  uint8_t reserved[4];

  //! scanned access points information
  rsi_scan_info_t scan_info[RSI_AP_SCANNED_MAX];

} rsi_rsp_scan_t;




//! Response structure for Query firmware version command
typedef struct rsi_rsp_fw_version_s
{

  //! Firmware version
  uint8_t firmwre_version[20];

}rsi_rsp_fw_version_t;




//! Response structure for Query RSSI command
typedef struct rsi_rsp_rssi_s
{

  //! RSSI value of the accesspoint to which station is connected
  uint8_t rssi_value[2];

}rsi_rsp_rssi_t;




//! Response structure for Query MAC address command
typedef struct rsi_rsp_mac_s
{

  //! MAC address of the module
  uint8_t mac_address[6];

}rsi_rsp_mac_t;


//! Response structure rx stats command
typedef struct rsi_rsp_rx_stats_s
{
  //! no. of tx pkts
  uint8_t tx_pkts[2];

  //! no. of rx pkts
  uint8_t reserved_1[2];

  //! no. of tx retries
  uint8_t tx_retries[2];

  //! no. of pkts that pass crc
  uint8_t crc_pass[2];

  //! no. of pkts failing crc chk
  uint8_t crc_fail[2];

  //! no. of times cca got stuck
  uint8_t cca_stk[2];

  //! no of times cca didn't get stuck
  uint8_t cca_not_stk[2];

  //! no. of pkt aborts
  uint8_t pkt_abort[2];

  //! no. of false rx starts
  uint8_t fls_rx_start[2];

  //! cca idle time
  uint8_t cca_idle[2];

  //! Reserved fields
  uint8_t reserved_2[26];

  //! no. of rx retries
  uint8_t rx_retries[2];

  //! rssi value
  uint8_t reserved_3[2];

  //! cal_rssi
  uint8_t cal_rssi[2];

  //! lna_gain bb_gain
  uint8_t reserved_4[4];

  //! number of tx packets dropped after maximum retries
  uint8_t xretries[2];

  //! consecutive pkts dropped
  uint8_t max_cons_pkts_dropped[2];

  //! Reserved fields
  uint8_t reserved_5[2];

  //! BSSID matched broadcast packets count
  uint8_t bss_broadcast_pkts[2];

  //! BSSID matched multicast packets count
  uint8_t bss_multicast_pkts[2];

  //! BSSID & multicast filter matched packets count
  uint8_t bss_filter_matched_multicast_pkts[2];

}rsi_rsp_rx_stats_t;


//! IPV4 ipconfig command response  structure
typedef struct rsi_rsp_ipv4_parmas_s
{
  //! MAC address of this module
  uint8_t    macAddr[6];

  //! Configured IP address
  uint8_t   ipaddr[4];

  //! Configured netmask
  uint8_t    netmask[4];

  //! Configured default gateway
  uint8_t    gateway[4];

} rsi_rsp_ipv4_parmas_t;



//! IPV6 ipconfig command response structure
typedef struct  rsi_rsp_ipv6_parmas_s
{
  //! prefix length
  uint8_t   prefixLength[2];

  //! Configured IPv address
  uint8_t   ipaddr6[16];

  //! Router IPv6 address
  uint8_t   defaultgw6[16];

} rsi_rsp_ipv6_parmas_t;


//! Set wep keys command request structure
typedef struct rsi_wep_keys_s
{
  //! select the wep key to use
  uint8_t   index[2];

  //! 4 wep keys
  uint8_t   key[4][32];

}rsi_wep_keys_t;

typedef struct rsi_eap_credentials_s
{
  //! Username
  uint8_t   username[RSI_EAP_USER_NAME_LENGTH];

  //! Password
  uint8_t   password[RSI_EAP_PASSWORD_LENGTH];

}rsi_eap_credentials_t;


/******************************************************
 * *                    Structures
 * ******************************************************/

/******************************************************
 * *                 Global Variables
 * ******************************************************/

/******************************************************
 * *               Function Declarations
 * ******************************************************/
extern int32_t rsi_wlan_scan(int8_t *ssid, uint8_t chno,rsi_rsp_scan_t *result,uint32_t length);
extern int32_t rsi_wlan_scan_async(int8_t *ssid, uint8_t chno, void(*callback)(uint16_t status, const uint8_t *buffer, const uint16_t length));
extern int32_t rsi_wlan_connect(int8_t *ssid, rsi_security_mode_t sec_type, void *secret_key);
extern int32_t rsi_wlan_connect_async(int8_t *ssid, rsi_security_mode_t sec_type, void *secret_key, void(*scan_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length));
extern int32_t rsi_wlan_ap_start(int8_t *ssid, uint8_t channel, rsi_security_mode_t security_type,rsi_encryption_mode_t encryption_mode, uint8_t *password, uint16_t beacon_interval, uint8_t dtim_period);
extern int32_t rsi_wlan_execute_post_connect_cmds(void);
extern int32_t rsi_wlan_disconnect(void);
extern int32_t rsi_wlan_disconnect_stations(uint8_t *mac_address);
extern int32_t rsi_wlan_set_certificate(uint8_t certificate_type, uint8_t *buffer, uint32_t certificate_length);
extern int32_t rsi_wlan_get_status(void);
extern int32_t rsi_wlan_wps_generate_pin(uint8_t *response, uint16_t length );
extern int32_t rsi_wlan_wps_enter_pin(int8_t *wps_pin);
extern int32_t rsi_wlan_wps_push_button_event(int8_t *ssid);
extern int32_t rsi_transmit_test_start(uint16_t power, uint32_t rate, uint16_t length, uint16_t mode, uint16_t channel);
extern int32_t rsi_transmit_test_stop(void);
extern int32_t rsi_wlan_receive_stats_start(uint16_t channel);
extern int32_t rsi_wlan_receive_stats_stop(void);
extern int32_t rsi_wlan_get(rsi_wlan_query_cmd_t cmd_type, uint8_t *response, uint16_t length);
extern int32_t rsi_wlan_set(rsi_wlan_set_cmd_t cmd_type, uint8_t *request, uint16_t length);
extern uint16_t rsi_wlan_register_callbacks(uint32_t callback_id, void (*callback_handler_ptr)(uint16_t status, uint8_t *buffer, const uint32_t length));
extern int32_t rsi_wlan_send_data(uint8_t* buffer, uint32_t length);
extern int32_t rsi_wlan_wfd_start_discovery(uint16_t go_intent, int8_t *device_name,
                                    uint16_t channel, int8_t *ssid_post_fix, uint8_t *psk,
                                    void(*wlan_wfd_discovery_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length),
                                    void(*wlan_wfd_connection_request_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length));
extern int32_t rsi_wlan_wfd_connect(int8_t *device_name,  void (*join_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length));
extern int32_t rsi_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
extern uint16_t rsi_send_raw_data(uint8_t* buffer, uint32_t length);
extern uint8_t* rsi_fill_config_profile(uint32_t type, uint8_t *profile_buffer);
extern int32_t rsi_wlan_add_profile(uint32_t type, uint8_t *profile);
extern int32_t rsi_wlan_get_profile(uint32_t type, rsi_config_profile_t *profile_rsp, uint16_t length);
extern int32_t rsi_wlan_delete_profile(uint32_t type);
extern int32_t rsi_wlan_enable_auto_config(uint8_t enable, uint32_t type);
extern int32_t rsi_wlan_bgscan_profile(uint8_t cmd,rsi_rsp_scan_t *result,uint32_t length );




#endif
