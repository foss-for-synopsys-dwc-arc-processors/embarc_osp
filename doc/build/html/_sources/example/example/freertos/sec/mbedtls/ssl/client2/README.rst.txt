.. _example_sec_mbedtls_ssl_client2:

FreeRTOS mbed SSL Example: SSL Client
#####################################


Overview
********

This example takes reference from mbedtls **ssL_client2** to test ssl client functionality.


Detailed Description
====================

* Extra Required Tools

* Extra Required Peripherals

    - Digilent PMOD WIFI(MRF24WG0MA)

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

3. Here the mbedtls ssl server node's ip address is 192.168.43.76, we can get
it from console output of uart. When initialization finished and network
connected, run **main server_addr=192.168.43.76 auth_mode=optional** in
ntshell, it will try to do ssl communication.

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

	embARC Build Time: Jan 23 2019, 11:38:25
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
	NTShell Task StartUp
	COM1>RS9113 MAC ADDRESS:00-00-00-00-00-00

	Now trying to connect to WIFI hotspot, please wait about 30s!
	RS9113 MAC ADDRESS:88-da-1a-0b-5e-4c
	Link is UP!

	Now start get ip address using DHCP, Please wait about 30s!

	-----WIFI IP ADDRESS INFO-----
	ipaddr 192.168.137.46 netmask 255.255.255.0 gw 192.168.137.1
	dns server 0 :192.168.137.1
	dns server 1 :0.0.0.0
	-----------------------------------
	WiFi connected
	Please run NT-Shell command(main) to start your application.
	main command may required some arguments, please refer to example's document.
	main server_addr=192.168.137.39 auth_mode=optional

	. Seeding the random number generator... ok
	. Loading the CA root certificate ... ok (0 skipped)
	. Loading the client cert. and key... ok
	. Connecting to tcp/192.168.137.39/4433... ok
	. Setting up the SSL/TLS structure... ok
	. Performing the SSL/TLS handshake... ok
		[ Protocol is TLSv1.2 ]
		[ Ciphersuite is TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384 ]
		[ Record expansion is 29 ]
		[ Maximum fragment length is 16384 ]
	. Verifying peer X.509 certificate... failed
	! The certificate validity has expired
	! The certificate Common Name (CN) does not match with the expected CN
	! The certificate validity starts in the future

	. Peer certificate information    ...
		cert. version     : 3
		serial number     : 09
		issuer name       : C=NL, O=PolarSSL, CN=Polarssl Test EC CA
		subject name      : C=NL, O=PolarSSL, CN=localhost
		issued  on        : 2013-09-24 15:52:04
		expires on        : 2023-09-22 15:52:04
		signed using      : ECDSA with SHA256
		EC key size       : 256 bits
		basic constraints : CA=false

	> Write to server: 34 bytes written in 1 fragments

	GET / HTTP/1.0
	Extra-header:


	< Read from server: 152 bytes read

	HTTP/1.0 200 OK
	Content-Type: text/html

	<h2>mbed TLS Test Server</h2>
	<p>Successful connection using: TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384</p>
	. Closing the connection... done
