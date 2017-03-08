/*
 *
 * Copyright (C) 2016 by Synopsys, Inc.
 *
 *
 *
 */
/*!
\defgroup    EMBARC_APP_FREERTOS_SEC_MBEDTLS_DTLS_SERVER embARC DTLS Secure Server using mbed TLS
\ingroup EMBARC_APPS_TOTAL
\ingroup EMBARC_APPS_OS_FREERTOS
\ingroup EMBARC_APPS_MID_LWIP
\ingroup EMBARC_APPS_MID_MBEDTLS
\brief   embARC example for DTLS secure server using mbed TLS
\details
### Extra Required Tools

### Extra Required Peripherals
    - Digilent PMOD WIFI(MRF24WG0MA)
    - SD Card to store certification file, key file

### Design Concept
    This example takes from mbedtls dtls_server to test dtls server functionality.

### Usage Manual
    - Copy the **tests** dir in middleware/mbedtls into the SD card of secure server node, it will load CA file and key file.
    - When initialization finished and network connected, run **main** in ntshell, it will try to do dtls communication.
    - In pc node or EMSK mbedtls dtls client node, run the client application to connect to the server node, refer to \ref EMBARC_APP_FREERTOS_SEC_MBEDTLS_DTLS_CLIENT "Run mbedtls dtls_client".

    ![ScreenShot of mbedtls dtls server](pic/images/example/emsk/freertos_sec_mbedtls_dtls_server.jpg)

### Extra Comments

*/

