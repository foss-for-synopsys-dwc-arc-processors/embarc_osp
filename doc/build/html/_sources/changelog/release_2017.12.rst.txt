.. _release_2017.12:

Release 2017.12
===============

This is the sixth release of the embARC Open Software Platform, release 2017.12.

Tool Support
############

* Eclipse IDE for GNU Toolchain for ARC Processors, 2017.09
* MetaWare Development Toolkit, 2017.09

Features and Enhancements
#########################

* Main new features
  
  * Add support for new ARC HS Development Kit (HSDK) platform
  * Reconstruct the device HAL

* Device HAL updates

  * Add device/peripheral folder for reusable peripheral drivers
  * Add device/ip folder for on-chip ip, e.g. uart, spi, i2c.
  * Add Designware SDIO driver

* ARC HAL updates

  * Add initial support of arc connect for multi core
  * Add the io pinmux api for arduino, pmod and mikro bus
  * Bugs fixed

* Board support updates

  * Update the board support according the changes in device HAL
  * Add ARC HS Development Kit support

* Middleware updates

  * Remove Xively
  * Remove Matrixssl
  * Remove Wolfssl

* OS updates

  * Remove Contiki OS

* Examples update

  * Remove Xively, Matrixssl, Wolfssl and Contiki related examples
  * Make some examples board-independent
  * Bug fixes

* Documentation updates
* Bug fixes
* FOSS notice update

How to Use
##########

Please refer to the doc folder in the release zip file and `Getting Started <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp/wiki/Getting-Started-with-embARC-OSP>`_.

Known Issues
############

* This release has changed a lot in device layer, with the peripheral layer introduced, many examples which rely on old peripheral drivers in emsk will not be compatible with new embARC OSP, please modify it.
* Make version >=4.0 is not supported. Please use make version 3.81 or 3.82
* For Wifi applications, the Digilent pmod Wifi with Microchip part MRF24WG0MA should be used. Pmods built with MRF24WB0MA have been shown to experience problems and are NOT supported.
* Please don't put embARC source code too deep in your directory, because some examples can't be compiled due to too long command line not supported in Windows.
* Since brand new device hal API is upgraded, your application that use the old device hal API need to be upgraded.
* Secureshield library need MPU hardware module on EMSK, check your hardware configuration of EMSK before using it.
* Please use the recommended tool versions for metaware and arc gnu.
* For ARC GNU, an utility called **tcftool** from Metaware toolset is required for tcf build support. You can get it from Metaware Lite and copy it to ARC GNU's folder such as *C:/arc_gnu/bin/*.
