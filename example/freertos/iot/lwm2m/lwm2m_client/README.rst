.. _example_iot_lwm2m_lwm2m_client:

FreeRTOS IoT LwM2M Client
#########################

Overview
********

 embARC Example for LwM2M Client on LwIP and FreeRTOS

Detailed Description
====================

 * Extra Required Tools
    - Java environment
    - Eclipse Leshan

 * Extra Required Peripherals
    Digilent PMOD WIFI (MRF24WG0MA)

 * Design Concept
    This example is designed to show how to use LwM2M client in FreeRTOS.

 * Usage Manual
    This is an example running OMA LightweightM2M(LwM2M) client, provides core device management features(device, connectivity, etc). It provides an extra ntshell command called ``lwm2m_client`` to show this demo.
    - About LwM2M provided in embARC

        + LwM2M features supported by eclipse wakaama project
        + We only support FreeRTOS+LwIP
        + LwM2M use UDP connections to exchange data

    - First, you need to provide a LwM2M Server, here we take Eclipse Leshan server for demonstrate use.

        + Eclipse provide a sandbox `leshan server <https://github.com/eclipse/leshan>`_
        + You can download a standalone `leshan server demo <https://hudson.eclipse.org/leshan/job/leshan/lastSuccessfulBuild/artifact/leshan-server-demo.jar>`_ binary from leshan website, and java environment(version >= 1.7.0_75) is required to run this binary

    - When leshan server is running on your local PC (connected to the same wifi network which your board will connect), then download program to the board, and run lwm2m_client ntshell command.

        + If you local PC IP is 192.168.43.4, then you need to run command ``lwm2m_client -s 192.168.43.4`` to connect to local leshan server.
        + If leshan reports any port being occupied, you could uses its detailed options to change port
        + If connected to local leshan server, you will see the connected LwM2M client in ``http://127.0.0.1:8080/#/clients``.
        + Click on the client, then you will see many objects which the connected client provided.
        + On this website, you can do many operations on this objects.
        + Here you can read onboard switch and button value, and control onboard LEDs using Object 1024.

 * Extra Comments
    - Sometimes the server may not respond to client, this may caused by the wifi connection is bad.
    - In the case above, you need to disconnect all the power of your board, reconnect PMOD WIFI module and wait for a while, then restart this application.

Buidling and Running
********************

 This example outputs to the console. It is supported by all platform. The commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/freertos/iot/lwm2m/lwm2m_client
    $ make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu run

And here is command to run leshan server demo:

.. code-block:: console

    java -jar leshan-server-demo.jar

Sample Output
=============

.. image:: /pic/images/example/freertos_lwm2m_client/freertos_lwm2m_client.jpg