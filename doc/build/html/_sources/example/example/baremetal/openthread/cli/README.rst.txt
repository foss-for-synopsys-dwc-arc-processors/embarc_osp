.. _example_openthread_cli:

OpenThread CLI
##############

Overview
********

 | This example is an OpenThread Command Line Interface (CLI) application on PMOD RF2 (MRF24J40).
 | The mesh network is established, and IPv6 is configured with using bi-/multi-boards as Thread nodes.
 | The node status can be shown on the terminal via UART. There are dozens of commands supported in the CLI example.

Detailed Description
====================

 * Extra Required Tools
    NO

 * Extra Required Peripherals
    - 2 x EMSK
    - 2 x Digilent PMOD RF2 (MRF24J40)
    - 1 x SD card

 * Design Concept
    | This example is an OpenThread Command Line Interface (CLI) application on PMOD RF2 (MRF24J40).
    | The mesh network is established, and IPv6 is configured with using bi-/multi-boards as Thread nodes.
    | The node status can be shown on the terminal via UART. There are dozens of commands supported in the CLI example.
    | The OpenThread CLI reference is in OT_CLI.md.

 * Usage Manual
    - See EMBARC_BOARD_CONNECTION "EMSK" to connect PMOD RF2 (MRF24J40).
    - How to use this example
        * Program the secondary bootloader application into onboard SPI flash of EMSKs.
        * Generate boot.bin of the Openthread CLI example using "make bin".
        * Run Openthread CLI example with boot.bin from SD card. Make sure Bit 4 of the onboard DIP switch is ON to enable the secondary bootloader.
            - Insert SD Card back to one EMSK. Press the reset button to reboot it. Wait for loading boot.bin from SD card.
            - Get SD card from one EMSK and insert it to the other EMSK. Press the reset button to reboot it. Wait for loading boot.bin from SD card.
            - Enter “1” and press Enter button in one Tera Term. Enter “2” and press Enter button in the other one.
              The number will not be shown directly in the Tera Term until pressing Enter button from the keyboard.
            - Enter the following commands in the two terminal windows, "panid 0x1234", "ifconfig up", "thread start", to start
              start Thread process.
            - Wait 20 seconds for completing Thread configuration. Enter “state” to see the state of the node, one leader and one router.
            - Enter other commands in the OpenThread CLI reference (OT_CLI.md) to get more information. For example, “ipaddr” is used to show the IP address of the Thread node. Enter “ipaddr” in one ternimal to get the Thread node IP address, such as **fe80:0:0:0:f065:44eb:ef9f:57c8**. Enter “ping fe80:0:0:0:f065:44eb:ef9f:57c8” in the other ternimal to ping the Thread node. The Internet Control Messages Protocol (ICMP) is implemented in the OpenThread for **ping** command.

 * Extra Comments
    | A few seconds are required to make connections of Thread nodes.
    | Use AC adapter to ensure a steady power supply.
    | Open two Tera Term emulators to connect EMSKs with different COM ports.
    | The self-boot mode preparation is included in the above steps.
    | “make run” is not supported because EMSK boards are both v2.x and it can lead to conflict. See sect_example_usage_HowToDebugMultiBoards "how to debug multiple boards in embARC" for more information.
    | Enter number to generate the pseudo-random number for OpenThread. Recommend enter number in order, such as “1”, “2” and “3”. Using same number in different nodes may lead error.
    | Make sure the compiler configuration (TOOLCHAIN, BD_VER, CUR_CORE) of the secondary bootloader and bin file. For example, the bootloader for EMSK 2.3/ARCEM7D cannot boot the program for EMSK 2.3/ARCEM11D.

Buidling and Running
********************

 See the usage manual.

Sample Output
=============

 See the usage manual.

