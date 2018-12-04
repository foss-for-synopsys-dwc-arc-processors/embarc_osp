.. _board_emsdp:

ARC EM Software Development Platform
====================================

Introduction
------------

The DesignWare® ARC® EM Software Development Platform (EM SDP, emsdp)
is an FPGA based development platform intended to support ARC licenses in developing their software for the ARC EM processor family and ARC EM Subsystems. It has the support for ARC
EM4, EM5D, EM6, EM7D, EM9D and EM11D processors. ARC EM Enhanced Security Package (ESP) and ARC EM Subsystems (DFSS, SCSS, DSS) are also supported.

The EM SDP board includes the following features:

- Support for selected ARC EM and ARC EM Subsystem configurations
- Drag-and-drop configuration of FPGA device through USB
- Audio PLL capable of generating the most common audio reference clocks
- Analog audio input and output
- MEMs digital microphone stereo input
- 16MB Quad-SPI flash memory supporting execute-in-place (XiP)
- 16MB Pseudo-Static RAM
- Bluetooth 4.0, Wi-Fi a,b,g,n and 802.15.4 (e.g ZigBee) wireless connectivity
- On board 9-axis motion sensors
- Expansion connectors

  - 1x Arduino UNO revision 3, over two connectors
  - 3x PMOD connector (2x6 header)
  - 1x MikroBUS
  - 1x generic pin header

- Micro-SD card interface for user data
- Console output via a micro USB® connector
- Multiple debug ports

  - Micro-USB (Digilent® Adept Xilinx® programming)
  - Cortex 10-pin Debug Interface (adapter to Ashling® Opella-XD for ARC debug)
  - Mictor 38 pin (Ashling Ultra-XD)

- Buttons, LEDs and switches


The DesignWare ARC EM Software Development Platform contains the following components:

- FPGA

  - Xilinx Kintex-7 XC7325T-2

- Memory

  - PSRAM (16 MB)
  - SPI Flash (16 MB quad-mode XiP)
  - Configuration memory (16 MB SPI flash)

- Interfaces

  - Audio line in/out
  - USB Data port (JTAG/UART/access to configuration memory)
  - Micro-SD Card
  - WIFI/BT/Zigbee module
  - ADC (eight channels)
  - Motion Sensor
  - Digital MEMs microphone (2x)
  - RTT Nexus, JTAG

- Extensions

  - Arduino Interface headers (UNO R3 compatible)
  - mikroBUS headers
  - Pmod Interfaces (3x)
  - Generic Pin Header (52 pins)


.. image:: /pic/EMSDP_Block_Diagram.png
    :alt: EMSDP Block Diagram


FPGA
^^^^

The FPGA on the ARC EM SDP board is configured by storing an FPGA bitstream into the configuration memory of the ARC EM SDP.
Drag-and-drop feature enable users to copy an FPGA bitstream from the PC to the configuration memory and programmed into the FPGA.

Memory
^^^^^^

Please refer to `ARC_EM_SDP_UserGuide` for memory map and more details.

Interfaces
^^^^^^^^^^

ARC EM SDP board provides a variety set of peripherals for evaluate and development.
There are many peripheral devices available, such as SPI, IIC, UART, GPIO.
Upon these, the ARC EM SDP offers multiple interfaces for audio, USB, micro-SD card, wireless, ADC, motion sensor, etc.
External hardware modules can also be easily connected to the board though extension interfaces using Pmod Connectors, Arduino, mikro BUS.

Extensions
^^^^^^^^^^

To bring your application context around the ARC EM SDP, Pmod Connectors, Arduino, mikro BUS are supported.
All the sets of connections are controlled by mux controller and can be switched by user.
There are many peripheral devices available, such as SPI, IIC, UART, GPIO.


Usage
-----

Setting Up
^^^^^^^^^^
You can use either USB-JTAG or USB-UART interface to communicate with ARC EM SDP board.
Before the interfaces are used, you must install the required drivers on the computer where you intend to run
the MetaWare debugger or other serial debug consoles (such as PuTTY or other hyper-terminals).

The driver is a part of the Digilent Adept tool. You can download the most recent version of
the Digilent Adept tool from the Digilent website at http://www.digilentinc.com, and follow the installation instructions provided by Digilent.

You may also skip this step if you are using default settings.

.. note:: Learn more about programming FPGA device in `ARC_EM_SDP_UserGuide`


Run Your First Program
^^^^^^^^^^^^^^^^^^^^^^
By setting up the board, you are now ready to run your first program on ARC EM SDP.
please refer to example `example_blinky` for further details. Build and download the example using either MetaWare or GNU. The command is shown below:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/blinky
    $ gmake BOARD=emsdp BD_VER=rev1 CORE=em11d_dfss TOOLCHAIN=mw run

.. note:: Notice that for CORE option there are multiple core choices, please check `<embarc_root>/board/emsdp/rev1` folder for available cores.

Development Guide
^^^^^^^^^^^^^^^^^
Please refer to `Developer Guides`.
You may need to specify peripheral driver (`<embarc_root>/device/peripheral`) for your own code.

For example, if wifi rs9113 driver is needed, add this line in makefile:

.. code-block:: console

    EXT_DEV_LIST += wifi/rs9113

Another example is to add both wifi driver and audio codec driver for an online audio application:

.. code-block:: console

    EXT_DEV_LIST += wifi/rs9113 audio/max9880a


Contributing
^^^^^^^^^^^^
ARC EM SDP is welcome to your contribution. If you found any bug or issue or have suggestions,
please feel free to raise issues or pull requests at [embARC Open Software Platform](https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp) repository on GitHub.





