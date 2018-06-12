.. _release_2016.05:

Release 2016.05
===============

This is the fourth release of the embARC Open Software Platform, release 2016.05.

Tool Support
############

* Eclipse IDE for GNU Toolchain for ARC Processors, 2015.12
* MetaWare Development Toolkit, 2015.12

Features and Enhancements
#########################

* Update support for EMSK and nSIM boards

  * uDMA and MPU supported
  * Update core configurations files

* Add new middlewares

  * `mbed TLS: yotta-2.3.0 <https://github.com/ARMmbed/mbedtls/tree/yotta-2.3.0>`_
  * `AWS IoT C SDK: 1.0.1 <https://github.com/aws/aws-iot-device-sdk-embedded-C/tree/v1.0.1>`_

* Add library support

  * `dlmalloc <http://g.oswego.edu/dl/html/malloc.html>`_
  * MetaWare standard c library for MetaWare toolchain
  * newlib c library for ARC GNU toolchain

* Improvement to ARC HAL

  * Bug fixes in ARC cache driver
  * Bug fixes and improvements in ARC exception driver

* Update middlewares

  * wolfSSL updated to `3.8.0 <https://github.com/wolfSSL/wolfssl/tree/v3.8.0>`_
  * matrixssl updated to `3.8.3 <https://github.com/matrixssl/matrixssl/tree/3-8-3-open>`_

* Add TinyDTLS support to Contiki
* Add SecureShield library
* Rearrange embARC examples

  * Divided examples by different runtimes such as baremetal, FreeRTOS, Contiki
  * Remove some simple examples
  * Update example documentation
  * Make enhancements to all the remained examples

* Add new examples

  * AWS IoT Smarthome demo for single node and multiple nodes
  * Secureshield examples for baremetal and Contiki runtime
  * Tinydtls examples for contiki
  * mbedTLS examples for FreeRTOS
  * ARC uDMA examples

* Upgrade embARC application bring-up process

  * Board initialization is done before main function
  * NT-Shell, LwIP, and FatFs middleware are initialized before main function if enabled
  * FreeRTOS excution environment is initialized before main function
  * Details refer to board/board.c
  * Default core configuration for embARC application changed to EMSK2.2 - ARC EM7D

* Documentation updates
* Bug fixes
* FOSS notice update

How to Use
##########

Please refer to the doc folder in the release zip file and getting started instructions on `embarc.org <https://embarc.org/>`_

Known Issues
############

* Xively Cloud demo instability. The examples are functional but reliability of the Xively Cloud service may vary.
* Make version >=4.0 is not supported. Please use make version 3.81 or 3.82
* For Wifi applications, the Digilent pmod Wifi with Microchip part MRF24WG0MA should be used. Pmods built with MRF24WB0MA have been shown to experience problems and are NOT supported.
* Please don't put embARC source code too deep in your directory, because some examples can't be compiled due to too long command line not supported in Windows.
* Since brand new device hal API is upgraded, your application that use the old device hal API need to be upgraded.
* For ARC GNU with EMSK 2.2, you need to change the JTAG adapter frequency(adapter_khz) in its openocd config file(<ARC_GNU>/share/openocd/scripts/board/snps_em_sk.cfg) to 5000, simply change the adapter_khz to 5000.
* Secureshield library need MPU hardware module on EMSK, check your hardware configuration of EMSK before using it.
* Compile and run secureshield example using ARC GNU, you need to install ARC GNU 2016.03.
