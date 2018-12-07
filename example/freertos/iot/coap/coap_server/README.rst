.. _example_iot_coap_coap_server:

FreeRTOS IoT CoAP Server
########################

Overview
********

 embARC Example for CoAP Protocol on FreeRTOS

Detailed Description
====================

 * Extra Required Tools
    Python with `CoAPthon <https://github.com/Tanganelli/CoAPthon>`_ packet installed

 * Extra Required Peripherals
    - Digilent PMOD WIFI (MRF24WG0MA)
    - Digilent PMOD TMP2

 * Design Concept
    This example is designed to show how CoAP server running on FreeRTOS. We prepared three different resources which are helloworld string, temperature readings, and LEDs. To demonstrate the function of CoAP server, we need to use CoAPthon as a CoAP client.

 * Usage Manual
    This is an example running coap server on LwIP with FreeRTOS. The Pmod modules should be connected to ARC board.

 * Extra Comments
    NO

Building and Running
********************

 This example outputs to the console. It is supported by all platform. You might need to change MUX configurations or connect wires in order to use peripheral modules, see :ref:`peripheral_preparation`.
 The commands to run this example are as follows:

.. code-block:: console

	$ cd <embarc_root>/example/freertos/iot/coap/coap_server
	$ make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu run


When the WiFi is connected and CoAP server is running, you may run python code shown below.
The python code uses CoAPthon packet as a CoAP Client and try to get and post resources from server. Please noted that the host IP in python code should be replaced to the ipaddr shown on board uart console before running.

.. code-block:: python

	from coapthon.client.helperclient import HelperClient
	host = "192.168.137.65"
	port = 5683
	path ="hello"

	client = HelperClient(server=(host, port))
	response = client.get(path)
	print(response.pretty_print())

	path ="temperature"
	response = client.get(path)
	print(response.pretty_print())

	path ="leds"
	payload = "0xAA"
	response = client.post(path, payload)
	print(response.pretty_print())

	client.stop()


You may save this code as a .py file (e.g. ``CoAP_client.py``) and run it from a new cmd console. Here is an example of running the python code at a new cmd console:

.. code-block:: console

	$ cd <py_file_location>
	$ CoAP_client.py


Sample Output
=============

 Output from embARC

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

	embARC Build Time: Apr 13 2018, 10:08:43
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
	 ipaddr 192.168.137.65 netmask 255.255.255.0 gw 192.168.137.1
	 dns server 0 :192.168.137.1
	 dns server 1 :0.0.0.0
	-----------------------------------
	WiFi connected
	temperature update running
	Exit from main function, error code:0....


Output from CoAPthon CoAP client

.. code-block:: console

	2018-04-13 10:10:35,208 - MainThread - coapthon.layers.messagelayer - DEBUG - send_request - From None, To ('192.168.137.65', 5683), None-None, GET-bG, [Uri-Path: hello, ] No payload
	2018-04-13 10:10:35,208 - MainThread - coapthon.client.coap - DEBUG - send_datagram - From None, To ('192.168.137.65', 5683), CON-10095, GET-bG, [Uri-Path: hello, ] No payload
	2018-04-13 10:10:35,209 - Thread-1   - coapthon.client.coap - DEBUG - Start receiver Thread
	2018-04-13 10:10:35,210 - MainThread-Retry-10095 - coapthon.client.coap - DEBUG - retransmit loop ... enter
	2018-04-13 10:10:35,385 - Thread-1   - coapthon.client.coap - DEBUG - receive_datagram - From ('192.168.137.65', 5683), To None, ACK-10095, CONTENT-bG, [Content-Type: 0, Max-Age: 196607, ] hello world...11 bytes
	2018-04-13 10:10:35,386 - Thread-1   - coapthon.layers.messagelayer - DEBUG - receive_response - From ('192.168.137.65', 5683), To None, ACK-10095, CONTENT-bG, [Content-Type: 0, Max-Age: 196607, ] hello world...11 bytes
	2018-04-13 10:10:35,386 - Thread-1   - coapthon.client.coap - DEBUG - Waiting for retransmit thread to finish ...
	2018-04-13 10:10:35,387 - MainThread-Retry-10095 - coapthon.client.coap - DEBUG - retransmit loop ... exit
	Source: ('192.168.137.65', 5683)
	Destination: None
	Type: ACK
	MID: 10095
	Code: CONTENT
	Token: bG
	Content-Type: 0
	Max-Age: 196607
	Payload:
	hello world

	2018-04-13 10:10:35,401 - MainThread - coapthon.layers.messagelayer - DEBUG - send_request - From None, To ('192.168.137.65', 5683), None-None, GET-VK, [Uri-Path: temperature, ] No payload
	2018-04-13 10:10:35,402 - MainThread - coapthon.client.coap - DEBUG - send_datagram - From None, To ('192.168.137.65', 5683), CON-10096, GET-VK, [Uri-Path: temperature, ] No payload
	2018-04-13 10:10:35,406 - MainThread-Retry-10096 - coapthon.client.coap - DEBUG - retransmit loop ... enter
	2018-04-13 10:10:35,553 - Thread-1   - coapthon.client.coap - DEBUG - receive_datagram - From ('192.168.137.65', 5683), To None, ACK-10096, CONTENT-VK, [Content-Type: 0, Max-Age: 60, ] 28.187500...9 bytes
	2018-04-13 10:10:35,554 - Thread-1   - coapthon.layers.messagelayer - DEBUG - receive_response - From ('192.168.137.65', 5683), To None, ACK-10096, CONTENT-VK, [Content-Type: 0, Max-Age: 60, ] 28.187500...9 bytes
	2018-04-13 10:10:35,557 - Thread-1   - coapthon.client.coap - DEBUG - Waiting for retransmit thread to finish ...
	2018-04-13 10:10:35,557 - MainThread-Retry-10096 - coapthon.client.coap - DEBUG - retransmit loop ... exit
	Source: ('192.168.137.65', 5683)
	Destination: None
	Type: ACK
	MID: 10096
	Code: CONTENT
	Token: VK
	Content-Type: 0
	Max-Age: 60
	Payload:
	28.187500

	2018-04-13 10:10:35,572 - MainThread - coapthon.layers.messagelayer - DEBUG - send_request - From None, To ('192.168.137.65', 5683), None-None, POST-wR, [Uri-Path: leds, ] 0xAA...4 bytes
	2018-04-13 10:10:35,573 - MainThread - coapthon.client.coap - DEBUG - send_datagram - From None, To ('192.168.137.65', 5683), CON-10097, POST-wR, [Uri-Path: leds, ] 0xAA...4 bytes
	2018-04-13 10:10:35,577 - MainThread-Retry-10097 - coapthon.client.coap - DEBUG - retransmit loop ... enter
	2018-04-13 10:10:35,655 - Thread-1   - coapthon.client.coap - DEBUG - receive_datagram - From ('192.168.137.65', 5683), To None, ACK-10097, CONTENT-wR, [] No payload
	2018-04-13 10:10:35,656 - Thread-1   - coapthon.layers.messagelayer - DEBUG - receive_response - From ('192.168.137.65', 5683), To None, ACK-10097, CONTENT-wR, [] No payload
	2018-04-13 10:10:35,659 - Thread-1   - coapthon.client.coap - DEBUG - Waiting for retransmit thread to finish ...
	2018-04-13 10:10:35,659 - MainThread-Retry-10097 - coapthon.client.coap - DEBUG - retransmit loop ... exit
	Source: ('192.168.137.65', 5683)
	Destination: None
	Type: ACK
	MID: 10097
	Code: CONTENT
	Token: wR
	Payload:
	None
