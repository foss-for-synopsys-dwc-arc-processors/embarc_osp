.. _release_2015.01:

Release 2015.01
===============

This is the first release of the embARC Open Software Platform, release 2015.01.

Tool Support
############

* Eclipse IDE for GNU Toolchain for ARC Processors, v2014.12
* MetaWare Development Toolkit, v2014.12

Features and Enhancements
#########################

* Based on the latest source code of embARC

      * Contiki v2.7
      * FreeRTOS v8.1.2
      * LWIP and uIP stacks
      * MQTT and COAP middleware support

* Support for EM Starter Kit development platform (v1.1 and v2.0)
* Example IDE Projects for EMSK v1.1 and v2.0 for both MetaWare IDE and ARC GNU IDE

How to Use
##########

You can refer to the doc folder in the release zip file and README.md in example folder.

Known Issues
############

* Xively Cloud demo instability. The examples are functional but reliability of the Xively Cloud service may vary.
* Make version >=4.0 is not supported. Please use make version 3.81 or 3.82
* For Wifi applications, the Digilent pmod Wifi with Microchip part MRF24WG0MA should be used. Pmods built with MRF24WB0MA have been shown to experience problems.
