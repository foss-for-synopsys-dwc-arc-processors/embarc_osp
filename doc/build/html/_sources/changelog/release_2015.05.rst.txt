.. _release_2015.05:

Release 2015.05
===============

This is the second release of the embARC Open Software Platform, release 2015.05.

Tool Support
############

- Eclipse IDE for GNU Toolchain for ARC Processors, v2014.12
- MetaWare Development Toolkit, J-2015.03

Features and Enhancements
#########################

* LwM2M support: provided by Eclipse wakaama project
* TLS/DTLS support: 2 implementations are supported  - wolfssl and matrixssl
* MQX RTOS support: standard embARC middleware (LwIP/NTShell/Fatfs) has been ported for using with MQX
* New UART-based bluetooth4.0 module (HM-10) driver, and RW009 wifi driver
* Added support for EMSK v2.1 (in addition to existing support for v1.1 and v2.0)
* API updates

      * _arc_lr -> _arc_lr_reg in inc/arc/arc_builtin.h
      * _arc_sr -> _arc_sr_reg in inc/arc/arc_builtin.h

* Device HAL updates

      * add ioctl command: UART_CMD_SET_READ_MODE into uart device

* Source code updates

      * FreeRTOS version updated to V8.2.1 (latest)
      * LwIP updated to the latest upstream sources
      * Contiki updated to the latest upstream sources

* embARC examples updates

    * New examples for LwM2M server and client
    * New examples for HM-10 BLE device
    * New examples for wolfssl and matrixssl

* Build System update

    * Update build system to support source code dependency file generation correctly
    * Change the example build object folder from *obj_tool_core* to *obj_board_bd_ver/tool_core*
    * Add support for *run* and *gui* make command for just GNU toolchain, now use openocd and arc-elf32-gdb for running and debugging

* Pre-installed ide projects update

    * Add ide projects for EMSK2.1
    * Add include paths and macros in ide project configurations

* Documentation updates
* Bug fixes
* FOSS notice update

How to Use
------------------

Please refer to the doc folder in the release zip file and getting started instructions on `embarc.org <https://embarc.org/>`_

Known Issues
-------------------

* Xively Cloud demo instability. The examples are functional but reliability of the Xively Cloud service may vary.
* Make version >=4.0 is not supported. Please use make version 3.81 or 3.82
* For WiFi applications, the Digilent pmod Wifi with Microchip part MRF24WG0MA should be used. Pmods built with MRF24WB0MA have been shown to experience problems and are NOT supported.
