.. _example_freertos_demo:

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

    $ cd <embarc_root>/example/freertos_demo
    $ make BOARD=emsk BD_VER=11 CUR_CORE=arcem6 TOOLCHAIN=gnu run

If you do not have an EMSK development board, you can use the nSIM simulator
which have been installed in MetaWare IDE.

.. code-block:: console

    $ cd <embarc_root>/example/freertos_demo
    $ gmake BOARD=nsim BD_VER=1506 CUR_CORE=arcemfull TOOLCHAIN=mw run

.. note:: Make sure you have selected the correct configuration of EMSK via dipswitches and that you have reset the board (button above “R”) to confirme its configuration

If you wish to run ARC v1 series cores on nSIM simulator
which have been installed in MetaWare IDE.
Core here is specified to arc610d, other choices are arc605, arc710d

.. code-block:: console

    $ cd <embarc_root>/example/freertos_demo
    $ gmake BOARD=nsim CUR_CORE=arc610d TOOLCHAIN=mw_mcc run

Sample Output
=============

If you are using the **nSIM**, one console msg wil be used to simulate the
bink of LEDs, you will see the following output in the CMD window when the
download is successful.

.. code-block:: console

    C:\embarc_bsp\example\freertos_demo> gmake BOARD=nsim CUR_CORE=arc610d TOOLCHAIN=mw_mcc run
    "Download & Run obj_nsim_10/mw_mcc_arc610d/freertos_demo_mw_mcc_arc610d.elf"
    mdb -nooptions -nogoifmain -toggle=include_local_symbols=1 -nsim -off=binary_stdin -off=binary_stdout  -on=load_at_paddr -on=reset_upon_restart -off=flush_pipe -off=cr_for_more -OKN  @obj_nsim_10/mw_mcc_arc610d/embARC_generated/mdb.arg -run obj_nsim_10/mw_mcc_arc610d/freertos_demo_mw_mcc_arc610d.elf
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

    embARC Build Time: Aug  4 2020, 15:43:58
    Compiler Version: Metaware, mcc

    [main] Helloworld! This is a freertos demo!
    [task_blinky] Helloworld! This is a freertos demo!
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
