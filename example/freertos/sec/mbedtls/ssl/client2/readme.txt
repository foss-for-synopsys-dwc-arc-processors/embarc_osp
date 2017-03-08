/*
 *
 * Copyright (C) 2016 by Synopsys, Inc.
 *
 *
 *
 */
/*!
\defgroup    EMBARC_APP_FREERTOS_SEC_MBEDTLS_SSL_CLIENT2 embARC SSL Secure Client using mbed TLS
\ingroup EMBARC_APPS_TOTAL
\ingroup EMBARC_APPS_OS_FREERTOS
\ingroup EMBARC_APPS_MID_LWIP
\ingroup EMBARC_APPS_MID_MBEDTLS
\brief   embARC example for SSL secure client using mbed TLS
\details
### Extra Required Tools

### Extra Required Peripherals
    - Digilent PMOD WIFI(MRF24WG0MA)
    - SD Card to store certification file, key file

### Design Concept
    This example takes from mbedtls ssL_client2 to test ssl client functionality.

### Usage Manual
    - In pc node or EMSK mbedtls ssl server node, run the server application to listen, refer to \ref EMBARC_APP_FREERTOS_SEC_MBEDTLS_SSL_SERVER2 "Run mbedtls ssl server".
    - Copy the **tests** dir in middleware/mbedtls into the SD card of secure server node, it will load CA file and key file.
    - Here the mbedtls ssl server node's ip address is 192.168.43.76, we can get it from console output of uart.
    - When initialization finished and network connected, run **main server_addr=192.168.43.76 auth_mode=optional** in ntshell, it will try to do ssl communication.
    - Run **main -h** for how to run this example.

    ![ScreenShot of mbedtls ssl client2](pic/images/example/emsk/freertos_sec_mbedtls_ssl_client2.jpg)

### Extra Comments

*/

