.. _release_2018.09:

Release 2018.09
===============

This is the seventh release of the embARC Open Software Platform, release 2018.09.

Tool Support
############

* GNU Toolchain for ARC Processors, 2018.03
* MetaWare Development Toolkit, 2018.06

Features and Enhancements
#########################

* Main new features
  * Support for new ARC IoT Development Kit (IoTDK) platform
* Device HAL updates
  * Support for ARC Data Fusion and Sensor and Control Subsystem drivers and corresponding wrappers (under device/ip/subsystem)
  * Support for DesignWare I2S driver
  * Support for UART-BLE module RN4020
  * Support for IMU sensor module MPU9250
* ARC HAL updates
  * Bugs fixes
* Board support updates
  * Support for new ARC IoT Development Kit (IoTDK platform)
* Middleware updates
  * No update
* OS updates
  * No update
* Examples update
  * Add new examples for RN420 and MPU9250 modules
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
