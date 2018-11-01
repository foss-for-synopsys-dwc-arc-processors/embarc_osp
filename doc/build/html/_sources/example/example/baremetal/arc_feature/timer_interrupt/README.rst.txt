.. _example_arc_feature_timer_interrupt:

ARC Timer Interrupt Example
###########################

Overview
********

 This example is designed to show how to use |arc| internal timers and timer interrupt.

Detailed Description
====================

 * Extra Required Tools
      NO

 * Extra Required Peripherals
      NO

 * Design Concept
    This example is designed to show how to use ARC timer and interrupt.
    During initialization, the timer 0 and timer 1 are enabled and interrupt is configured. The timer 0 Interrupt Service Routine (ISR) is set as Fast Interrupt Request Queue (FIQ) using ``int_pri_set()``. The timer 0 interrupt is responsed with the highest priority.

.. note::
    Hardware support: all |arc| based platform with internal timers configured

Buidling and Running
********************

Taking |emsk| as exmaple, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/arc_feature/timer_interrupt
    $ make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu run

.. note:: Make sure you have selected the correct configuration of |emsk| via dip switches and that you have reset the board (button above “R”).

Sample Output
=============

If you are using the ``nSIM``, you will see the following outputs in the CMD window when download is successful.

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
    interrupt number:8, extern interrupts:6, interrupt priorities:16, fiq enabled
    timer 0 is present
    cnt:2719
    timer 1 is present
    cnt:106634
    timer0 interrupt with the highest priority(fiq if fiq enabled):1
    timer0 interrupt with the highest priority(fiq if fiq enabled):2
    stop timer0, cnt:20
    timer1 interrupt:1
    timer1 interrupt:2
    stop timer1, cnt:19
    enable timer 0 watchdog
    ---end----

If you are using the |emsk|, you will see the same msg through UART

.. note:: Make sure you have chosen correct COM port and set the baud rate to 115200 before you start.
