.. _example_freertos_blinky:

FreeRTOS Demo
#############

Overview
********

 This example demostrates basic FreeRTOS task usage.

Detailed Description
====================
 * Extra Required Tools

    NO

 * Extra Required Peripherals

    NO

 * Design Concept

    This example is designed to test board without any extra peripheral

 * Usage Manual

    Toggle all leds on board in 1s period

 * Extra Comments

    NO

Buidling and Running
********************

This example outputs to the console. It is supported by all platform. The commands
to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/freertos/freertos_blinky
    $ make BOARD=emsk run

If you do not have an EMSK development board, you can use the nSIM simulator
which have been installed in MetaWare IDE.

.. code-block:: console

    $ cd <embarc_root>/example/freertos/freertos_blinky
    $ gmake BOARD=nsim run

Sample Output
=============

If you are using the **nSIM**, one console msg wil be used to simulate the
bink of LEDs, you will see the following output in the CMD window when the
download is successful.

.. code-block:: console

    C:\embarc_bsp\example\freertos/freertos_blinky> gmake BOARD=nsim run
    "Download & Run obj_nsim_10/mw_arcem/freertos_blinky_mw_arcem.elf"
    mdb -nooptions -nogoifmain -toggle=include_local_symbols=1 -nsim -off=binary_stdin -off=binary_stdout  -on=load_at_paddr -on=reset_upon_restart -off=flush_pipe -off=cr_for_more -OKN  -prop=nsim_mem-dev=uart0,kind=dwuart,base=0xf0000000,irq=24 @obj_nsim_10/mw_arcem/embARC_generated/mdb.arg -run obj_nsim_10/mw_arcem/freertos_blinky_mw_arcem.elf
    11:19:04.575058 WARN:[IO-DEVMGR] SYS device uart0: Unhandled property: kind
    Console now belongs to UART, hit CRTL-] to return to simulator.
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

    embARC Build Time: Sep 14 2021, 10:54:36
    Compiler Version: Metaware, Clang 11.1.0
    [main] Helloworld! This is a freertos demo!
    [task_blinky] Helloworld! This is a freertos demo!
    led out: ff, ff
    Exit from main function, return code: 0
    led out: ff00, ff
    led out: ff, ff
    led out: ff00, ff
    led out: ff, ff
    led out: ff00, ff
    led out: ff, ff
    led out: ff00, ff
    led out: ff, ff
    led out: ff00, ff
    .................

If you are using the **ARC EMSK** development board, you will see the leds
start to blink when the download is successful.
