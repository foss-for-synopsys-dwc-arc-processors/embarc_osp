/*
 *
 * Copyright (C) 2016 by Synopsys, Inc.
 *
 *
 *
 */
/*!
\defgroup    EMBARC_APP_FREERTOS_SEC_MBEDTLS_SSL_SERVER2 embARC SSL Secure Server using mbed TLS
\ingroup EMBARC_APPS_TOTAL
\ingroup EMBARC_APPS_OS_FREERTOS
\ingroup EMBARC_APPS_MID_LWIP
\ingroup EMBARC_APPS_MID_MBEDTLS
\brief   embARC example for SSL secure server using mbed TLS
\details
### Extra Required Tools

### Extra Required Peripherals
    - Digilent PMOD WIFI(MRF24WG0MA)
    - SD Card to store certification file, key file

### Design Concept
    This example takes from mbedtls ssl_server2 to test ssl server functionality.

### Usage Manual
    - Copy the **tests** dir in middleware/mbedtls into the SD card of secure server node, it will load CA file and key file.
    - When initialization finished and network connected, run **main auth_mode=optional** in ntshell, it will wait for ssl client to connect to it.
    - Refer to \ref EMBARC_APP_FREERTOS_SEC_MBEDTLS_SSL_CLIENT2 "Run mbedtls ssl client" about how to run ssl client example.

    ![ScreenShot of mbedtls ssl server](pic/images/example/emsk/freertos_sec_mbedtls_ssl_server2.jpg)


### Extra Comments

*/

