.. _example_net_httpserver:

FreeRTOS Net HTTP Server
########################

Overview
********

 This example is a small-scale http server, designed to show the how LwIP applications is run on FreeRTOS on ARC.

Detailed Description
====================

 * Extra Required Tools
    NO

 * Extra Required Peripherals
    Digilent PMOD WIFI (MRF24WG0MA)

 * Design Concept
    This example is designed to show how to use LwIP netconn API to serve as a httpserver.

 * Usage Manual
    The Pmod wifi modules should be connected to ARC. This is an example using TCP/IP connection to work as a small web httpserver.
    When wifi is connected, there will be IP address showing on terminal. Use a web browser and you will recieve a web page showing success.

 * Extra Comments
    NO

Buidling and Running
********************

 This example outputs to the console. It is supported by all platform. The commands to run this example are as follows:

.. code-block:: console

	$ cd <embarc_root>/example/freertos/net/httpserver
	$ make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu run


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

	embARC Build Time: Apr 12 2018, 16:36:48
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	Start Init LWIP
	Enter to main function....
	Wait until WiFi connected...

	Now trying to connect to WIFI hotspot, please wait about 30s!
	MRF24G Device Information As Follows:
	Device Type:2, ROM Ver:31, Patch Ver:7
	Connection Profile ID:1
	WF INIT SUCCESSFULL!
	MRF24G MAC ADDRESS:00-1e-c0-0e-6e-0a
	WF_EVENT_CONNECTION_SUCCESSFUL
	Link is UP!

	Now start get ip address using DHCP, Please wait about 30s!

	-----PMOD WIFI IP ADDRESS INFO-----
	 ipaddr 192.168.137.157 netmask 255.255.255.0 gw 192.168.137.1
	-----------------------------------
	WiFi connected
	Now Open Your Browser Enter The IPAddr Show On Terminal, you will get an webpage servered on EMSK