/*
 *
 * Copyright (C) 2016 by Synopsys, Inc.
 *
 *
 *
 */
/*!
\defgroup    EMBARC_APP_FREERTOS_SEC_WOLFSSL_SERVER embARC wolfSSL Secure Server
\ingroup EMBARC_APPS_TOTAL
\ingroup EMBARC_APPS_OS_FREERTOS
\ingroup EMBARC_APPS_MID_LWIP
\ingroup EMBARC_APPS_MID_WOLFSSL
\brief   embARC Example for wolfSSL secure server
\details
### Extra Required Tools

### Extra Required Peripherals
    - Digilent PMOD WIFI(MRF24WG0MA)
    - SD Card to store certification file, key file

### Design Concept
    The port of wolfSSL server example for embARC. Same as wolfSSL server ample.
    For usage, please refer the manual of wolfSSL.

### Usage Manual
    - Hardware:
      - EMSK node: PMOD WiFi to J5, SD Card to SD card slot
      - PC node/another EMSK node running wolfSSL secure client
    - WiFi Network: *SSID:embARC, PASSWORD:qazwsxedc*
    - Run steps:
      - Copy the **certs** dir in middleware/wolfssl into the SD card of secure server node, it will load CA file and key file.
      - Run this example, and when initialization finished and network connected, run different commands in ntshell to start server node, and wait for client to connect.
        - tls server cmd: **main -b -p 1111**
        - dtls server cmd: **main -b -p 1111 -u**
      - Refer to \ref EMBARC_APP_FREERTOS_SEC_WOLFSSL_CLIENT "wolfSSL client" about how to run client node.
      - To get help, run **main -?**.

    ![ScreenShot of wolfSSL secure server](pic/images/example/emsk/freertos_sec_wolfssl_server.jpg)

### Extra Comments
    Run **main -?** will print all help messages and exit the application.
*/

