.. _example_ble_rn4020:

BLE Moude: iotdk onboard RN4020 module
######################################

Overview
********

 This example is designed to demonstrate how to operate UART-based BLE device RN4020 via UART and how to use RN4020 BLE API defined in rn4020.h.

Detailed Description
====================
 * Extra Required Tools
    BLE APP on SmartPhone:
        - Android: `nRF Connect <https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en_US>`_
        - iOS: `LightBlue <https://itunes.apple.com/us/app/lightblue-bluetooth-low-energy/id557428110?mt=8>`_

 * Design Concept
    This example is designed to demonstrate how to operate UART-based BLE device RN4020 via UART and how to use RN4020 BLE API defined in rn4020.h.

 * Usage Manual
    - How to use this example
        | This example demonstrates how to use the API of RN4020 located in *device/peripheral/ble*. The APIs contain methods to send AT command and receive response, and read or write data to BLE device.
        | The example performs tests on the RN4020 BLE device using different uart baudrate, which take approximately 2 minutes, then the device will enter to receive and send mode.
        | Users can use a BLE-UART APP on their smartphone which must support BLE, for android please use nRF Connect, for iOS please use LightBlue.

 * Extra Comments
 	NO

Buidling and Running
********************

It is supported by iotdk. The cmds to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/ble_rn4020
    $ gmake BOARD=iotdk TOOLCHAIN=mw run

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

	embARC Build Time: Sep 11 2018, 16:17:39
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	n4020 test application
	state: INITIALIZING
	rx:CMD
	state: READY
	state: WAITING_FOR_AOK
	tx: SF,1
	rx:AOK
	state: READY
	state: WAITING_FOR_AOK
	tx: SN,embARC
	rx:AOK
	state: READY
	state: WAITING_FOR_AOK
	tx: SS,C0000001
	rx:AOK
	state: READY
	state: WAITING_FOR_AOK
	tx: PZ
	rx:AOK
	state: READY
	state: WAITING_FOR_AOK
	tx: PS,00112233445566778899AABBCCDDEEFF
	rx:AOK
	state: READY
	state: WAITING_FOR_AOK
	tx: PC,000102030405060708090A0B0C0D0E0F,12,05
	rx:AOK
	state: READY
	state: WAITING_FOR_AOK
	tx: PC,101112131415161718191A1B1C1D1E1F,0A,05
	rx:AOK
	state: READY
	state: WAITING_FOR_AOK
	tx: SR,00002000
	rx:AOK
	state: READY
	state: WAITING_FOR_RESET
	tx: R,1
	rx:Reboot
	state: WAITING_FOR_CMD
	rx:CMD
	state: READY
	state: WAITING_FOR_AOK
	tx: A
	rx:AOK
	state: READY
	state: WAITING_FOR_LS
	tx: LS
	rx:180A
	service uuid, skip it
	rx:  2A25,000E,V
	rx:  2A27,0010,V
	rx:  2A26,0012,V
	rx:  2A28,0014,V
	rx:  2A29,0016,V
	rx:  2A24,0018,V
	rx:180F
	service uuid, skip it
	rx:  2A19,0021,V
	rx:  2A19,0022,C
	rx:00112233445566778899AABBCCDDEEFF
	service uuid, skip it
	rx:  000102030405060708090A0B0C0D0E0F,001B,02,05
	rx:  000102030405060708090A0B0C0D0E0F,001C,10,02
	rx:  101112131415161718191A1B1C1D1E1F,001E,0A,05
	rx:END
	state: READY
	state: WAITING_FOR_AOK
	rx:AOK