/*
 *
 * Copyright (C) 2016 by Synopsys, Inc.
 *
 *
 *
 */
/*!
\defgroup    EMBARC_APP_FREERTOS_SEC_MBEDTLS_DTLS_CLIENT embARC DTLS Secure Client using mbed TLS
\ingroup EMBARC_APPS_TOTAL
\ingroup EMBARC_APPS_OS_FREERTOS
\ingroup EMBARC_APPS_MID_LWIP
\ingroup EMBARC_APPS_MID_MBEDTLS
\brief   embARC example for DTLS secure client using mbed TLS
\details
### Extra Required Tools

### Extra Required Peripherals
    - Digilent PMOD WIFI(MRF24WG0MA)
    - SD Card to store certification file, key file

### Design Concept
    This example takes from mbedtls dtls_client to test dtls client functionality.

### Usage Manual
    - In pc node or EMSK mbedtls dtls_server node, run the server application to listen, refer to \ref EMBARC_APP_FREERTOS_SEC_MBEDTLS_DTLS_SERVER "Run mbedtls dtls_server".
    - Copy the **tests** dir in middleware/mbedtls into the SD card of secure server node, it will load CA file and key file.
    - Change SERVER_NAME and SERVER_ADDR in dtls_client.c to the dtls_server node's IP address. And build this application again.
    - When initialization finished and network connected, run **main** in ntshell, it will try to do dtls communication.

    ![ScreenShot of mbedtls dtls client](pic/images/example/emsk/freertos_sec_mbedtls_dtls_client.jpg)

### Extra Comments

*/

