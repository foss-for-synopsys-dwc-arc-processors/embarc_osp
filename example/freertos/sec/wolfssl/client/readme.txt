/*
 *
 * Copyright (C) 2016 by Synopsys, Inc.
 *
 *
 *
 */
/*!
\defgroup	EMBARC_APP_FREERTOS_SEC_WOLFSSL_CLIENT embARC wolfSSL Secure Client
\ingroup	EMBARC_APPS_TOTAL
\ingroup	EMBARC_APPS_OS_FREERTOS
\ingroup	EMBARC_APPS_MID_LWIP
\ingroup	EMBARC_APPS_MID_WOLFSSL
\brief	embARC Example for wolfSSL secure client

\details
### Extra Required Tools

### Extra Required Peripherals
    - Digilent PMOD WIFI(MRF24WG0MA)
    - SD Card to store certification file, key file

### Design Concept
    The port of wolfssl secure client example for embARC. Same as wolfssllient example.
    For usage, please refer the manual of wolfSSL.

### Usage Manual
    - Hardware:
      - EMSK node: PMOD WiFi to J5, SD Card to SD card slot
      - PC node/another EMSK node running wolfSSL secure server
    - WiFi Network: *SSID:embARC, PASSWORD: qazwsxedc*
    - Run steps:
      - Start an wolfssl server node example before run this example, refer to \ref EMBARC_APP_FREERTOS_SEC_WOLFSSL_SERVER "wolfSSL server" for details.
      - For example, the wolfssl server node's ip address is **192.168.43.76**.
      - Copy the **certs** dir in middleware/wolfssl into the SD card of secure client node, it will load CA file and key file.
      - Run this example, and when initialization is finished and network is connected, run different commands in ntshell to connect to tls or dtls server.
        - connect to tls server cmd: **main -h 192.168.43.76 -p 1111**
        - connect to dtls server cmd: **main -h 192.168.43.76 -p 1111 -u**
      - To get help, run **main -?**.

    ![ScreenShot of wolfSSL secure client](pic/images/example/emsk/freertos_sec_wolfssl_client.jpg)

### Extra Comments
    Run **main -?** will print all help messages and exit the application.

*/

