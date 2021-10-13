.. _example_spi_slave:

SPI_SLAVE
#########

Overview
********

 This example shows SPI driver polling mode usage for both master and slave.

Detailed Description
====================
 * Extra Required Tools
    jumper wire

 * Extra Required Peripherals
    NO

 * Design Concept
    The master will transfer data to slave. Upon recieving, the slave will loopback the data.

 * Usage Manual
    This example requires one IoTDK board (as master) and one EMSK board (as slave)

 * Extra Comments
    NO

Buidling and Running
********************

Use jumper wire to connect EMSK P1 lower row with IoTDK Pmod_B upper row. For pin map please refer to documents:
`EMSK <https://foss-for-synopsys-dwc-arc-processors.github.io/embarc_osp/doc/build/html/board/emsk.html#pmod-pins-definition>`_
`IoTDK <https://foss-for-synopsys-dwc-arc-processors.github.io/embarc_osp/doc/build/html/board/iotdk.html#extension-interfaces>`_


.. note:: Make sure you have also connect GND of two boards with jumper wire


With two different types of board connected to your computer, an extra command parameter ''DIG_NAME'' is needed to tell which board is which.
Launch ''Digilent Adept'' which should have been installed along with board driver to get the ''DIG_NAME''. In our developing secnario, the ''DIG_NAME'' for EMSK is TE0604-03
And the ''DIG_NAME'' for IoTDK is IoTDK. Please noted that the ''DIG_NAME'' may varies from board to board.

*please run EMSK (SPI slave) program first*, tThe commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/spi_slave
    $ make BOARD=emsk BD_VER=22 CUR_CORE=arcem9d DIG_NAME=TE0604-03 run

If you do not have an EMSK development board, you can use the nSIM simulator
which have been installed in MetaWare IDE.

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/spi_slave
    $ make BOARD=iotdk DIG_NAME=IoTDK run


Sample Output
=============

The output from IoTDK (SPI master) are shown below

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

    embARC Build Time: Oct 13 2021, 10:35:16
    Compiler Version: Metaware, Clang 11.1.0
    PI Master & Slave test Oct 13 2021 / 14:42:48
    IoTDK Pmod B upper row is working as SPI Master
    SPI Master is successfully opened
    SPI Master Buffer
    TX: 0x0 0x0 0x0 0x0
    RX: 0x0 0x0 0x0 0x0
    SPI Master Buffer
    TX: 0x1 0x1 0x1 0x1
    RX: 0x0 0x0 0x0 0x0
    SPI Master Buffer
    TX: 0x2 0x2 0x2 0x2
    RX: 0x1 0x1 0x1 0x1
    SPI Master Buffer
    TX: 0x3 0x3 0x3 0x3
    RX: 0x2 0x2 0x2 0x2
    SPI Master Buffer
    TX: 0x4 0x4 0x4 0x4
    RX: 0x3 0x3 0x3 0x3
    SPI Master Buffer
    TX: 0x5 0x5 0x5 0x5
    RX: 0x4 0x4 0x4 0x4
    SPI Master Buffer
    TX: 0x6 0x6 0x6 0x6
    RX: 0x5 0x5 0x5 0x5
    .................

The output from EMSK (SPI slave) are shown below

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

    embARC Build Time: Oct 11 2021, 10:12:43
    Compiler Version: Metaware, Clang 11.1.0
    SPI Master & Slave test Oct 13 2021 / 14:42:47
    EMSK Pmod 1 (J1) lower row is working as SPI Slave
    SPI Slave is successfully opened
    SPI Slave Buffer
    TX: 0x0 0x0 0x0 0x0
    RX: 0x0 0x0 0x0 0x0
    SPI Slave Buffer
    TX: 0x0 0x0 0x0 0x0
    RX: 0x1 0x1 0x1 0x1
    SPI Slave Buffer
    TX: 0x1 0x1 0x1 0x1
    RX: 0x2 0x2 0x2 0x2
    SPI Slave Buffer
    TX: 0x2 0x2 0x2 0x2
    RX: 0x3 0x3 0x3 0x3
    SPI Slave Buffer
    TX: 0x3 0x3 0x3 0x3
    RX: 0x4 0x4 0x4 0x4
    SPI Slave Buffer
    TX: 0x4 0x4 0x4 0x4
    RX: 0x5 0x5 0x5 0x5
    SPI Slave Buffer
    TX: 0x5 0x5 0x5 0x5
    RX: 0x6 0x6 0x6 0x6
    .................
