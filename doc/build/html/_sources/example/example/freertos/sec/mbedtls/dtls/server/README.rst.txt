.. _example_sec_mbedtls_dtls_server:

FreeRTOS mbed TLS Example: DTLS Server
######################################

Overview
********

This example takes reference from mbedtls **dtls_server** to test dtls client functionality.


Detailed Description
====================

* Extra Required Tools

* Extra Required Peripherals

    - Digilent PMOD WIFI(MRF24WG0MA)

.. note::
   EMSDP has board WiFi module, no need to use PMOD WiFI.

.. note::
   The memory footprint of this example is larger then
   the default configuration of |iotdk|. Without optimization ,this example doesnot support |iotdk|


Buidling and Running
********************

1. Before building this example, it's required to get the mbedtls code and apply specific patch through the ``apply_embarc_patch.sh`` by the following commands

.. code-block:: console

   cd <embarc_root>/middleware/mbedtls
   sh apply_embarc_patch.sh

2. When initialization finished and network connected, run **main** in ntshell, it will try to do dtls communication.

3. In pc node or mbedtls dtls_client node, run the client application to connect to the server node.

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

	embARC Build Time: Jan 23 2019, 11:21:59
	Compiler Version: Metaware, 4.2.1 Compatible Clang 6.0.1 (branches/release_60)
	FatFS initialized successfully!
	Start Init LWIP
	ntshell command info(Information) was registered!
	ntshell command main(Call main function) was registered!
	ntshell command ls(List information about files) was registered!
	ntshell command cd(Change the working directory) was registered!
	ntshell command rm(Remove the file or the folder) was registered!
	ntshell command mkdir(Create the directory) was registered!
	ntshell command cat(Output file contents) was registered!
	ntshell command cp(Copy source to destination) was registered!
	ntshell command mv(Rename file or move file) was registered!
	ntshell command pwd(Print the current working directory) was registered!
	ntshell command cls(Clear the console output) was registered!
	ntshell command touch(Create a file) was registered!
	ntshell command ymodem(Ymodem file transfer protocol) was registered!
	ntshell command disk(Operate the storge device) was registered!
	ntshell command load(Load .bin/.hex file from SD card to ram at specified address) was registered!
	ntshell command go(Run the program at the specified address) was registered!
	ntshell command spirw(Read/Write the image in spi flash for updating bootloader/application image) was registered!
	ntshell command mload(Update firmware, read or write MCS file in the SPI flash) was registered!
	NTShell Task StartUp
	COM1>
	Now trying to connect to WIFI hotspot, please wait about 30s!
	MRF24G Device Information As Follows:
	Device Type:2, ROM Ver:31, Patch Ver:7
	Connection Profile ID:1
	WF INIT SUCCESSFULL!
	MRF24G MAC ADDRESS:00-1e-c0-0e-71-ac
	WF_EVENT_CONNECTION_SUCCESSFUL
	Link is UP!

	Now start get ip address using DHCP, Please wait about 30s!

	-----WIFI IP ADDRESS INFO-----
	ipaddr 192.168.137.39 netmask 255.255.255.0 gw 192.168.137.1
	dns server 0 :192.168.137.1
	dns server 1 :0.0.0.0
	-----------------------------------
	WiFi connected
	Please run NT-Shell command(main) to start your application.
	main command may required some arguments, please refer to example's document.
	main

	. Loading the server cert. and key... ok
	. Bind on udp/*/4433 ... ok
	. Seeding the random number generator... ok
	. Setting up the DTLS data... ok
	. Waiting for a remote connection ... ok
	. Performing the DTLS handshake... hello verification requested
	. Waiting for a remote connection ... ok
	. Performing the DTLS handshake... ok
	< Read from client: 9 bytes read

	Echo this

	> Write to client: 9 bytes written

	Echo this

	. Closing the connection... done
	. Waiting for a remote connection ... ok
	. Performing the DTLS handshake... failed
	! mbedtls_ssl_handshake returned -0x7900

	Last error was: -30976 - SSL - Processing of the ClientHello handshake message failed

	. Waiting for a remote connection ...