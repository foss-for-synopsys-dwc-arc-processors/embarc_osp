.. _example_openthread_ncp:

OpenThread NCP
##############

Overview
********

 | This example is an OpenThread Network Co-Processor (NCP) application on PMOD RF2 (MRF24J40).
 | This example can serve as a low-power wireless Network Co-Processor (NCP) to join Thread network and communicate with Userspace WPAN Network Daemon (wpantund) on Unix-like operating systems.
 | This example can be used along with OpenThread wpantund to provide Internet connectivity to the Thread network.

.. note:: The NCP status can not be shown on the terminal via UART, it might show messy code.

Detailed Description
====================

 * Extra Required Tools
    NO

 * Extra Required Peripherals
    - 2 x EMSK
    - 2 x Digilent PMOD RF2 (MRF24J40)
    - 2 x SD card

 * Design Concept
    - This example is an OpenThread Network Co-Processor (NCP) application on PMOD RF2 (MRF24J40).
    - This example can serve as a low-power wireless Network Co-Processor (NCP) to join Thread network and communicate with Userspace WPAN Network Daemon (wpantund) on Unix-like operating systems.
    - This example can be used along with OpenThread wpantund to provide Internet connectivity to the Thread network.
    - For more information about NCP and wpantund, see README.md for reference.

 * Usage Manual
    - See \ref EMBARC_BOARD_CONNECTION "EMSK" to connect PMOD RF2 (MRF24J40).
    - How to use this example
        * Program the secondary bootloader application into onboard SPI flash of EMSKs.
        * Generate boot.bin of the Openthread NCP example using "make bin".
        * Run an Openthread CLI example first with boot.bin from SD card, see `cli\README.md` for more information.
            - Insert SD Card back to one EMSK. It will run the CLI example. Press the reset button to reboot it. Wait for loading boot.bin from
            - Enter “1” and press Enter button in the Tera Term. Enter the number here to generate pseudo random number for OpenThread.
            - Enter the following commands in the two terminal windows, "panid 0x1234", "ifconfig up", "thread start", to start Thread process.
            - Wait 20 seconds for completing Thread configuration. Enter “state” to see the state of the node, it will be the leader.
        * Run Openthread NCP example with boot.bin from SD card. Make sure Bit 4 of the onboard DIP switch is ON to enable the secondary bootloader.
            - Insert the other SD card to the other EMSK. It will run the NCP example. Press the reset button to reboot it. Wait for loading bo
            - Enter **2** and press Enter button in the NCP's Tera Term.
            - After entering **2** in the NCP's Tera Term, the NCP will start up automatically. It will show you some messy code in the last line, just ignore it.
            - Install and Run OpenThread wpantund, and join the NCP to the Thread network created by CLI. See README.md for more information.
            - Ping to each other between Linux host and CLI Node.

 * Extra Comments
    | Use AC adapter to ensure a steady power supply.
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

