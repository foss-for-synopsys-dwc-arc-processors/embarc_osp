.. _example_sec_mbedtls_ssl_client2:

FreeRTOS mbed SSL Example: SSL Client
#####################################


Overview
********

This example takes reference from mbedtls **ssL_client2 ** to test ssl client functionality.


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

2. In pc node or mbedtls ssl server node, run the server application to listen.

3. Copy the ``tests`` dir in ``middleware/mbedtls`` into the SD card of secure
server node, it will load CA file and key file.

4. Here the mbedtls ssl server node's ip address is 192.168.43.76, we can get
it from console output of uart. When initialization finished and network
connected, run **main server_addr=192.168.43.76 auth_mode=optional** in
ntshell, it will try to do ssl communication.

Sample Output
=============

.. code-block:: console