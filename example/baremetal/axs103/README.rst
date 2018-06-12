.. _example_axs103:

AXS103 Example
##############

Overview
********

 This example is designed to test AXS 103 board function .

Detailed Description
====================
 embARC example for testing AXS 103 board function

 * Extra Required Tools
      NO

 * Extra Required Peripherals
      NO

 * Design Concept
    This example is designed to show how to use ARC timer and interrupt.

 * Usage Manual
    Configure your axs 103 hardware according the hardware user manual, then run it, and see the uart console output.

 * Extra Comments
    Tested and supported on v1.1 firmware version of AXS103.

Sample Output
=============

You will see the following output in theuart console window when the download is successful.

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

    embARC Build Time: Mar 16 2018, 09:58:46
    Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
    ARC timer and interrupt
    interrupt number:32, extern interrupts:12, interrupt priorities:1, fiq disabled
    timer 0 is present
    cnt:37294
    timer 0 interrupt:1
    button:0
    timer 0 interrupt:2
    button:0
    timer 0 interrupt:3
    button:0
    timer 0 interrupt:4
    button:0
    timer 0 interrupt:5
    button:0
    timer 0 interrupt:6
    button:1
    timer 0 interrupt:7
    button:8
    timer 0 interrupt:8
    button:0
    timer 0 interrupt:9
    button:2

.. note:: Make sure you have chosen correct COM port and set the Baud rate to 115200 before you start.