.. _example_iot_lwm2m_lwm2m_server:

FreeRTOS IoT LwM2M Server
#########################

Overview
********

 embARC Example for LwM2M Server on LwIP and FreeRTOS

Detailed Description
====================

 * Extra Required Tools
    - Java environment
    - Eclipse Leshan

 * Extra Required Peripherals
    Digilent PMOD WIFI (MRF24WG0MA)

 * Design Concept
    This example is designed to show how to use LwM2M server in FreeRTOS

 * Usage Manual
    This is an example of OMA LightweightM2M(LwM2M) server. It is not recommended to run lwm2m server on a resource-constrained board. It provides two ntshell commands to show this demo, one is ``lwm2m_server`` to set up a LwM2M server, and another one is ``lwm2m_op`` to operate at connected clients.

    Like previous :ref:`example_iot_lwm2m_lwm2m_client` example, you'll need a LwM2M client to test the server. Here we take Eclipse Leshan client for demonstrate use.

        - Eclipse provide a sandbox `leshan client <https://github.com/eclipse/leshan>`_
        - You can download a standalone `leshan client demo <https://hudson.eclipse.org/leshan/job/leshan/lastSuccessfulBuild/artifact/leshan-client-demo.jar>`_ binary from leshan website, and java environment(version >= 1.7.0_75) is required to run this binary

 * Extra Comments
    NO

Buidling and Running
********************

 This example outputs to the console. It is supported by all platform. The commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/freertos/iot/lwm2m/lwm2m_server
    $ make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu run

Once download the program to the board, you can run command ``main`` and ``lwm2m_server`` to start up a LwM2M server at default port 5683. You should also take note of server's IP address which will be used in next step. Once the server is up, run leshan client demo on your PC.

And here is command to run leshan client demo, you need to change the ip address in command according to your lwm2m_server:

.. code-block:: console

    java -jar leshan-client-demo.jar -u 192.168.137.237

Then, you could try out ``lwm2m_op`` command from server side, and also try to use 'w, a, s, d' keys to change the data from leshan client side

Sample Output
=============

Here is sample output from LwM2M server, highlight lines are user input.

.. code-block:: console
	:emphasize-lines: 72,76

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

	embARC Build Time: Apr 16 2018, 14:14:50
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	FatFS initialized successfully!
	Start Init LWIP
	ntshell command info(Information) was registered!
	ntshell command main(Call main function) was registered!
	ntshell command swape(Swap endianness of input variable) was registered!
	ntshell command map(Show the memory map of bootloader) was registered!
	ntshell command bcr(Dump processor build configuration) was registered!
	ntshell command lr(Read auxiliary register) was registered!
	ntshell command sr(Write auxiliary register) was registered!
	ntshell command mem(Operate the memory) was registered!
	ntshell command ledflash(Run led flash example) was registered!
	ntshell command led(Write LED) was registered!
	ntshell command btn(Read button value) was registered!
	ntshell command swt(Read DIP switch value) was registered!
	ntshell command ledswbtn(Operate the LED through button and switch) was registered!
	ntshell command adc(PMOD ADC sensor demo command) was registered!
	ntshell command temp(Show current temperature) was registered!
	ntshell command wifi(wifi operations) was registered!
	ntshell command ls(List information about files) was registered!
	ntshell command cd(Change the working directory) was registered!
	ntshell command rm(Remove the file or the folder) was registered!
	ntshell command mkdir(Create the directory) was registered!
	ntshell command cat(Output file contents) was registered!
	ntshell command cp(Copy source to destination) was registered!
	ntshell command mv(Rename file or move file) was registered!
	ntshell command pwd(Print the current working directory) was registered!
	ntshell command touch(Create a file) was registered!
	ntshell command ymodem(Ymodem file transfer protocol) was registered!
	ntshell command disk(Operate the storge device) was registered!
	ntshell command load(Load .bin file from SD card to ram at specified address) was registered!
	ntshell command boot(Load application specified in a boot.cfg file from sdcard and run) was registered!
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
	MRF24G MAC ADDRESS:00-1e-c0-0e-6e-0a
	WF_EVENT_CONNECTION_SUCCESSFUL
	Link is UP!

	Now start get ip address using DHCP, Please wait about 30s!

	-----PMOD WIFI IP ADDRESS INFO-----
	 ipaddr 192.168.137.237 netmask 255.255.255.0 gw 192.168.137.1
	 dns server 0 :192.168.137.1
	 dns server 1 :0.0.0.0
	-----------------------------------
	WiFi connected
	Please run NT-Shell command(main) to start your application.
	main command may required some arguments, please refer to example's document.
	main
	Start FreeRTOS LwM2M Client Demo by run lwm2m_server and lwm2m_op command
	ntshell command lwm2m_server(Start LwM2M server) was registered!
	ntshell command lwm2m_op(do some operations when LwM2M server is online) was registered!
	COM1>lwm2m_server
	Start LwM2M server.Try to start LwM2M server using port 5683.
	COM1>107 bytes received from [192.168.137.1]:55130
	  48 02 49 22 82 65 BD 10 CB FB 20 10 B2 72 64 11   H . I " . e . . . .   . . r d .
	  28 33 62 3D 55 09 6C 77 6D 32 6D 3D 31 2E 30 05   ( 3 b = U . l w m 2 m = 1 . 0 .
	  6C 74 3D 33 30 0D 01 65 70 3D 65 6D 62 41 52 43   l t = 3 0 . . e p = e m b A R C
	  2D 74 65 73 74 FF 3C 2F 3E 3B 72 74 3D 22 6F 6D   - t e s t . < / > ; r t = " o m
	  61 2E 6C 77 6D 32 6D 22 2C 20 3C 2F 31 2F 30 3E   a . l w m 2 m " ,   < / 1 / 0 >
	  2C 20 3C 2F 33 2F 30 3E 2C 20 3C 2F 36 2F 30 3E   ,   < / 3 / 0 > ,   < / 6 / 0 >
	  2C 20 3C 2F 33 33 30 33 2F 30 3E                  ,   < / 3 3 0 3 / 0 >

	New client #0 registered.
	Client #0:
		name: "embARC-test"
		binding: "UDP"
		lifetime: 30 sec
		objects: /1/0, /3/0, /6/0, /3303/0,

	17 bytes received from [192.168.137.1]:55130
	  48 02 49 23 F0 E4 4F 93 1E CE F0 06 B2 72 64 01   H . I # . . O . . . . . . r d .
	  30                                                0

	Client #0 updated.
	Client #0:
		name: "embARC-test"
		binding: "UDP"
		lifetime: 30 sec
		objects: /1/0, /3/0, /6/0, /3303/0,

