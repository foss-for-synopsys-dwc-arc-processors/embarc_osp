.. _release_2017.03:

Release 2017.03
===============

This is the fifth release of the embARC Open Software Platform, release 2017.03.

Tool Support
############

* Eclipse IDE for GNU Toolchain for ARC Processors, 2016.09
* MetaWare Development Toolkit, 2016.12

Features and Enhancements
#########################

* Main new features

  * Update SecureShield library to version 2
  * Add C++ runtime support
  * Add `OpenThread <https://github.com/openthread/openthread>`_ support
  * Add TCF support in build system
  * Linker script generation using template
  * Add ARC HS support by AXS 103 board and nSIM

* Update ARC HAL

  * Bugs fixed in arc cache and exception driver
  * Update uDMA driver for uDMA version 2
  * Update arc cache and exception driver for SecureShield

* Update board support

  * Remove EMSK 2.0 and 2.1 support
  * Add EMSK 2.3 support
  * EMSK 2.0, 2.1 and 2.2 can upgrade to 2.3
  * Add a new nsim support using TCF, BD_VER is 10
  * Add AXS 103 board basic support

* Update middlewares

  * LwIP updated to 2.0
  * AWS IoT C SDK updated to 2.0
  * Add ihex and openthread middleware

* Update libraries

  * Update c library support
  * Add cpp support
  * Update SecureShield library to version 2

* Update OS

  * FreeRTOS updated to V9.0.0

* Update examples

  * Update and add some SecureShield related examples
  * Add an axs 103 board test example
  * Add a blinky example
  * Add openthread test example
  * Add cpp test example
  * Add an iot demo application
  * Update aws smarthome demo with OLED support
  * Add a spiflash driver test example using spi with dma enabled

* Update build system

  * Object folders are created following the source code directory structure
  * Linker script is generated automatically using template
  * Simplified build rules for source code files
  * TCF support is included

* Documentation updates
* Bug fixes
* FOSS notice update

How to Use
##########

Please refer to the doc folder in the release zip file and `embARC.org <https://embarc.org/>`_.

Known Issues
############

* Make version >=4.0 is not supported. Please use make version 3.81 or 3.82
* For Wifi applications, the Digilent pmod Wifi with Microchip part MRF24WG0MA should be used. Pmods built with MRF24WB0MA have been shown to experience problems and are NOT supported.
* Please don't put embARC source code too deep in your directory, because some examples can't be compiled due to too long command line not supported in Windows.
* Since brand new device hal API is upgraded, your application that use the old device hal API need to be upgraded.
* Secureshield library need MPU hardware module on EMSK, check your hardware configuration of EMSK before using it.
* Please use the recommended tool versions for metaware and arc gnu.
* For ARC GNU 2016.09, an utility called **tcftool** from metaware toolset is required for tcf build support. You can get it from Metaware Lite and copy it to ARC GNU's folder such as *C:/arc_gnu/bin/*.

