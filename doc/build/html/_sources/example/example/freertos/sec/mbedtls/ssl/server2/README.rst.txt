.. _example_sec_mbedtls_ssl_server2:

FreeRTOS mbed SSL Example: SSL Server
#####################################


Overview
********

This example takes reference from mbedtls **ssl_server2 ** to test ssl server
functionality.


Detailed Description
====================

* Extra Required Tools

* Extra Required Peripherals

    - Digilent PMOD WIFI(MRF24WG0MA)
    - SD Card to store certification file, key file

.. note::
   EMSDP has board WiFi module, no need to use PMOD WiFI.

.. note::
   The memory footprint of this example is larger then
   the default configuration of |iotdk|. Without optimization ,
   this example doesnot support |iotdk|


Buidling and Running
********************

1. Before building this example, it's required to get the mbedtls code and
apply specific patch through the ``apply_embarc_patch.sh`` by the following
commands

.. code-block:: console

   cd <embarc_root>/middleware/mbedtls
   sh apply_embarc_patch.sh

2. Copy the ``tests`` dir in ``middleware/mbedtls`` into the SD card of secure
server node, it will load CA file and key file.

3. When initialization finished and network connected, run **main auth_mode=optional** in ntshell, it will wait for ssl client to connect to it.

Sample Output
=============

.. code-block:: console