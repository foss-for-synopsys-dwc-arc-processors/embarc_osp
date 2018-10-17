.. _example_esp8266_wifi:

ESP8266 WiFi Module
####################

Overview
********

 This example is designed to show basic usage of ESP01/ESP01S esp8266 WiFi Module on embARC board.

Detailed Description
====================

 * Extra Required Peripherals
    ESP01/ESP01S WiFi Module

 * Design Concept
    Without downloading users' image, ESP01/ESP01S can be controlled using AT commands.
    This example is designed to show how to use AT commands to control the Wi-Fi module and exchange data.
    Besides, we use AT command to establish a TCP server and provide simple HTTP service just like our another example :ref:`example_net_httpserver`

 * Usage Manual
 	- Connect ESP01/ESP01S module to your board, see :ref:`peripheral_preparation`.
 	- Set up an AP, the default setting of this example is to connect a WIFI with SSID: `embARC`, pwd: `qazwsxedc`.
 	- Build, download, run the example and get log from COM port console.
 	- When you see a `while` is shown in log, meaning that the server is setup and waiting for input. Then open your web browser (Chrome, Firefox) and visit the ip address shown in the log. You will get a web page right away.

Building and Running
********************

This example outputs to the console. It is supported by all platform. The commands to run this example are as follows:

.. code-block:: console

	$ cd <embarc_root>/example/freertos/esp8266_wifi
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

	embARC Build Time: Oct 17 2018, 15:25:42
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	=========================== Init ============================
	[at_parser_init]56: obj->psio 0x800062c0 -> 0x80000f28
	[at_send_cmd]83: command is NULL, send AT test command
	[at_send_cmd]117: at_out: "AT
	" (4)
	[at_get_reply]137: "AT

	OK" (9)
	============================ Set Mode ============================
	[at_send_cmd]117: at_out: "AT+CWMODE_CUR?
	" (16)
	[at_get_reply]137: "
	AT+CWMODE_CUR?
	+CWMODE_CUR:1

	OK" (38)
	CWMODE_CUR = 1
	[at_send_cmd]117: at_out: "AT+CWMODE_CUR=3
	" (17)
	[at_get_reply]137: "
	AT+CWMODE_CUR=3

	OK" (24)
	============================ Connect WiFi ============================
	[at_send_cmd]117: at_out: "AT+CWLAP
	" (10)
	[at_get_reply]137: "
	AT+CWLAP
	+CWLAP:(3,"embARC",-61,"6a:ec:c5:cc:2f:3b",1,32767,0)
	+CWLAP:(5,"Synopsys",-91,"6c:f3:7f:a5:8a:e3",1,-49,0)
	+CWLAP:(0,"synopsys-guest",-81,"6c:f3:7f:a5:38:21",1,-47,0)
	+CWLAP:(5,"Synopsys",-81,"6c:f3:7f:a5:38:23",1,-47,0)
	+CWLAP:(3,"ClickShare-1871789077",-87,"28:24:ff:bf:2b:06",1,32767,0)
	+CWLAP:(0,"synopsys-guest",-91,"6c:f3:7f:a5:8a:e1",1,-47,0)
	+CWLAP:(5,"Synopsys",-86,"04:bd:88:5a:7c:42",6,-51,0)
	+CWLAP:(0,"synopsys-guest",-86,"04:bd:88:5a:7c:43",6,-51,0)
	+CWLAP:(0,"synopsys-guest",-73,"6c:f3:7f:a2:90:61",11,-54,0)
	+CWLAP:(5,"Synopsys",-71,"6c:f3:7f:a2:90:63",11,-56,0)
	+CWLAP:(3,"XKORAT - Mi",-71,"4c:49:e3:ba:95:80",11,-29,0)

	OK" (731)
	Searching for WIFI "embARC" ......
	WIFI "embARC" found! Try to connect
	[at_send_cmd]117: at_out: "AT+CWMODE_CUR=1
	" (17)
	[at_get_reply]137: "
	AT+CWMODE_CUR=1

	OK" (24)
	[at_send_cmd]117: at_out: "AT+CWJAP_CUR="embARC","qazwsxedc"
	" (35)
	[at_get_reply]137: "
	AT+CWJAP_CUR="embARC","qazwsxedc"
	WIFI DISCONNECT
	WIFI CONNECTED
	WIFI GOT IP

	OK" (88)
	WIFI "embARC" connect succeed
	============================ Connect Server ============================
	[at_send_cmd]117: at_out: "AT+CIPMUX=1
	" (13)
	[at_get_reply]137: "
	AT+CIPMUX=1

	OK" (20)
	[at_send_cmd]117: at_out: "AT+CIPSERVER=1,80
	" (19)
	[at_get_reply]137: "
	AT+CIPSERVER=1,80
	no change

	OK" (37)
	============================ Show IP ============================
	[at_send_cmd]117: at_out: "AT+CIFSR
	" (10)
	[at_get_reply]137: "
	AT+CIFSR
	+CIFSR:STAIP,"192.168.137.96"
	+CIFSR:STAMAC,"5c:cf:7f:0c:27:d6"

	OK" (83)
	============================ while ============================
	============================ send ============================

	The message is:
	0,CONNECT
	1,CONNECT

	+IPD,0,383:GET / HTTP/1.1
	Host: 192.168.137.96
	Connection: keep-alive
	Upgrade-Insecure-Requests: 1
	User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36
	Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8
	Accept-Encoding: gzip, deflate
	Accept-Language: en-US,en;q=0.9


	Send Start
	[at_send_cmd]117: at_out: "AT+CIPSEND=0,44
	" (17)
	[at_get_reply]137: "AT+CIPSEND=0,44

	OK" (22)
	[at_get_reply]137: "
	>
	Recv 44 bytes

	SEND OK" (30)
	[at_send_cmd]117: at_out: "AT+CIPSEND=0,93
	" (17)
	[at_get_reply]137: "
	AT+CIPSEND=0,93

	OK" (24)
	[at_get_reply]137: "
	>
	Recv 93 bytes
	" (21)
	[at_send_cmd]117: at_out: "AT+CIPSEND=0,93
	" (17)
	[at_get_reply]137: "AT+CIPSEND=0,93
	busy s...
	" (29)
	[at_send_cmd]117: at_out: "AT+CIPCLOSE=0
	" (15)
	[at_get_reply]137: "AT+CIPCLOSE=0
	busy s...

	SEND OK" (36)
	Send Finish

