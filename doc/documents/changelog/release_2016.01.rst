.. _release_2016.01:

Release 2016.01
===============

This is the third release of the embARC Open Software Platform, release 2016.01.

Tool Support
############

* Eclipse IDE for GNU Toolchain for ARC Processors, v2015.06
* MetaWare Development Toolkit, J-2015.06

Features and Enhancements
#########################

* Brand-new device hardware abstraction layer(Device HAL)

    * Redefined UART/GPIO/SPI/IIC Device HAL
    * Add interrupt method support for Device HAL

        * Interrupt send and receive supported
        * Callback functions(TX/RX/ERROR) for interrupt supported

    * Add examples for how to use UART/GPIO/SPI/IIC Device HAL

        * Include both poll and interrupt usage

    * Device HAL API is not compatiable with old embARC releases

* Add support for EMSK 2.2

    * Support ARC EM7D, ARC EM9D, ARC EM11D configurations

* Add basic support for nSIM virtual board

    * nSIM

        * Use hostlink IO to simulate UART
        * No available simulated pheripherals

* Add U8glib support for SSD1306

    * Add gui solution for embARC using U8glib
    * Provide support for IIC based OLED Screen for U8glib

* JSON and HTTP Parser Support

    * Add parson as JSON support
    * Add http_parser as HTTP Parser support
    * Add an example that using parson and http parser to communicate with xively cloud

* Enhanced NT-Shell Commands

    * Add MCS file update command called *mload* to update EMSK firmware without Xilinx tools
    * Add YModem file transfer command called *ymodem* to send file to PC or receive file from PC using UART interface
    * Remove old file system test command *fs*, and implement some linux-like commands for file system testing like *ls*, *rm*
    * Restructure NT-Shell commands help message and source code structure

* IoT Cloud Application

    * Add a demo for smart home using MQTT technology

        * Located in *<embARC>/example/emsk/lwip/freertos/smarthome*
        * Provided a `dashboard website <http://fanghuaqi.github.io/xively-js/sample-app/>`_ to view and control LEDs on board, and monitor current temperature

* Middleware Update

    * LwIP updated to `commit:affc6d6 <http://git.savannah.gnu.org/cgit/lwip.git/tree/?id=affc6d6>`_
    * lwIP Contrib updated to `commit:167c3e7 <http://git.savannah.gnu.org/cgit/lwip/lwip-contrib.git/tree/?id=167c3e7>`_
    * Wakaama updated to `commit:69a32cf <https://github.com/eclipse/wakaama/tree/69a32cfae39f66fe4eec4cc8d1cd48ced7ad447c>`_
    * WolfSSL updated to V3.6.8
    * Some enhancements to xively, mqtt, coap, fatfs, ntshell middlewares

* OS Support Update

    * FreeRTOS updated to V8.2.3
    * Contiki updated to V3.0

* Update separated MQX RTOS support for embARC

    * Add xively, json, http_parser middlewares into MQX
    * Update LwIP and LwIP Contrib middlewares
    * Enhancements to mqtt middleware

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
* For ARC GNU with EMSK 2.2, you need to change the JTAG adapter frequency (adapter_khz) in its openocd config file to 5000 in *<ARC_GNU>/share/openocd/scripts/board/snps_em_sk.cfg*.
