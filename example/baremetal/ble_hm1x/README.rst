.. _example_ble_hm1x:

BLE Module: HM1X
################

Overview
********

 This example is designed to demonstrate how to operate UART-based BLE device HM10 via UART and how to use HM-10 BLE API defined in hm1x.h.

Detailed Description
====================
 * Extra Required Tools
    BLE APP on SmartPhone:
        - Android: BLE Android ComAssistant
        - iOS: `LightBlue <https://itunes.apple.com/us/app/lightblue-bluetooth-low-energy/id557428110?mt=8>`_

 * Extra Required Peripherals
    UART Based BLE Device HM-10

 * Design Concept
    This example is designed to demonstrate how to operate UART-based BLE device HM10 via UART and how to use HM-10 BLE API defined in hm1x.h.

 * Usage Manual
    - How to connect HM-10 to "EMSK".
      Connect the Pmod HM-10 to J1.
    - How to use this example
        | This example demonstrates how to use the API of HM-10 located in *device/peripheral/ble*. The APIs contain methods to send AT command and receive response, and read or write data to BLE device.
        | The example performs tests on the HM-10 BLE device using different uart baudrate, which take approximately 2 minutes, then the device will enter to receive and send mode.
        | Users can use a BLE-UART APP on their smartphone which must support BLE, for android please use BLE Android ComAssistant, for iOS please use LightBlue.

 * Extra Comments
 	NO

Buidling and Running
********************

It is supported by EMSK. The cmds to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/ble_hm1x
    $ gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw run

Sample Output
=============

.. code-block:: console

	-----------------------------------------------------------
	 ____                                _ ____
	|  _ \ _____      _____ _ __ ___  __| | __ ) _   _
	| |_) / _ \ \ /\ / / _ \ '__/ _ \/ _` |  _ \| | | |
	|  __/ (_) \ V  V /  __/ | |  __/ (_| | |_) | |_| |
	|_|   \___/ \_/\_/ \___|_|  \___|\__,_|____/ \__, |
	                                             |___/
	                     _       _    ____   ____
	       ___ _ __ ___ | |__   / \  |  _ \ / ___|
	      / _ \ '_ ` _ \| '_ \ / _ \ | |_) | |
	     |  __/ | | | | | |_) / ___ \|  _ <| |___
	      \___|_| |_| |_|_.__/_/   \_\_| \_\\____|
	------------------------------------------------------------

	embARC Build Time: Mar 28 2018, 16:20:03
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	Init HM1X with baudrate 9600bps
	++++Do following test with baudrate 9600bps++++
	test command Successfully
	Restart Successfully
	SET MODE to 0
	SET TYPE to 0
	SET ROLE to 0
	GET ROLE:0
	GET MODE:0
	GET TYPE:0
	GET Battery Level:100
	GET HM1X MAC Address:7CEC79BECBDD
	GET HM1X NAME:HMSoft
	GET HM1X Version:HMSoft V540
	++++Test with baudrate 9600bps finished++++
	Re-Init the HM1X BLE Baudrate to 4800bps, and restart the test
	++++Do following test with baudrate 4800bps++++
	test command Successfully
	Restart Successfully
	SET MODE to 0
	SET TYPE to 0
	SET ROLE to 0
	GET ROLE:0
	GET MODE:0
	GET TYPE:0
	GET Battery Level:100
	GET HM1X MAC Address:7CEC79BECBDD
	GET HM1X NAME:HMSoft
	GET HM1X Version:HMSoft V540
	++++Test with baudrate 4800bps finished++++
	Re-Init the HM1X BLE Baudrate to 9600bps, and restart the test
	++++Do following test with baudrate 9600bps++++
	test command Successfully
	Restart Successfully
	SET MODE to 0
	SET TYPE to 0
	SET ROLE to 0
	GET ROLE:0
	GET MODE:0
	GET TYPE:0
	GET Battery Level:100
	GET HM1X MAC Address:7CEC79BECBDD
	GET HM1X NAME:HMSoft
	GET HM1X Version:HMSoft V540
	++++Test with baudrate 9600bps finished++++
	Re-Init the HM1X BLE Baudrate to 19200bps, and restart the test
	++++Do following test with baudrate 19200bps++++
	test command Successfully
	Restart Successfully
	SET MODE to 0
	SET TYPE to 0
	SET ROLE to 0
	GET ROLE:0
	GET MODE:0
	GET TYPE:0
	GET Battery Level:100
	GET HM1X MAC Address:7CEC79BECBDD
	GET HM1X NAME:HMSoft
	GET HM1X Version:HMSoft V540
	++++Test with baudrate 19200bps finished++++
	Re-Init the HM1X BLE Baudrate to 38400bps, and restart the test
	++++Do following test with baudrate 38400bps++++
	test command Successfully
	Restart Successfully
	SET MODE to 0
	SET TYPE to 0
	SET ROLE to 0
	GET ROLE:0
	GET MODE:0
	GET TYPE:0
	GET Battery Level:100
	GET HM1X MAC Address:7CEC79BECBDD
	GET HM1X NAME:HMSoft
	GET HM1X Version:HMSoft V540
	++++Test with baudrate 38400bps finished++++
	Re-Init the HM1X BLE Baudrate to 57600bps, and restart the test
	++++Do following test with baudrate 57600bps++++
	test command Successfully
	Restart Successfully
	SET MODE to 0
	SET TYPE to 0
	SET ROLE to 0
	GET ROLE:0
	GET MODE:0
	GET TYPE:0
	GET Battery Level:100
	GET HM1X MAC Address:7CEC79BECBDD
	GET HM1X NAME:HMSoft
	GET HM1X Version:HMSoft V540
	++++Test with baudrate 57600bps finished++++
	Re-Init the HM1X BLE Baudrate to 115200bps, and restart the test
	++++Do following test with baudrate 115200bps++++
	test command Successfully
	Restart Successfully
	SET MODE to 0
	SET TYPE to 0
	SET ROLE to 0
	GET ROLE:0
	GET MODE:0
	GET TYPE:0
	GET Battery Level:100
	GET HM1X MAC Address:7CEC79BECBDD
	GET HM1X NAME:HMSoft
	GET HM1X Version:HMSoft V540
	++++Test with baudrate 115200bps finished++++

	Try to reset the baudrate to 9600bps
	Do send and receive test under baudrate:9600bps
	Start HM1X-BLE Send Receive Test, please connect to HM1X-BLE using its app.
	The main loop will receive data from HM1X, and send back what it received.
	.................
