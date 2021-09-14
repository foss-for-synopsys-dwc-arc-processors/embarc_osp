.. _example_arc_feature_smp:

ARC SMP Example
###############

Overview
********

This example shows a paradigm of multi-core programming in |embarc|.

Detailed Description
====================

 * Extra Required Tools
    NO

 * Extra Required Peripherals
    NO

 * Design Concept
    This example is designed to show the basics of |arc| multi-core features, including spinlock, inter-core interrupts (ici). This example works on nsim and hsdk.

.. note::
    In makefile we provide extra command work for multi-core secnarios. Please use mp_run or mp_gui for running or debugging the example.

Buidling and Running
********************
This example outputs to the console. It is supported by all platform. The commands
to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/arc_feature/multicore
    $ make BOARD=nsim mp_run

Sample Output
=============

.. code-block:: console

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

    embARC Build Time: Sep 13 2021, 17:28:58
    Compiler Version: Metaware, Clang 11.1.0
    core 1 starts to run
    core 1 in arc connect is 1
    core 0 starts to run
    core 0 in arc connect is 0
    core 1 delay 9 ticks
    core 0 receives int from core 1
    core 0 delay 5 ticks
    core 1 receives int from core 0
    core 0 delay 5 ticks
    core 1 receives int from core 0
    core 1 delay 9 ticks
    core 0 receives int from core 1
    core 0 delay 5 ticks
    core 1 receives int from core 0
    core 0 delay 5 ticks
    core 1 receives int from core 0
    core 1 delay 9 ticks
    core 0 receives int from core 1
    core 0 delay 5 ticks
    core 1 receives int from core 0
    core 0 delay 5 ticks
    core 1 receives int from core 0
    core 1 delay 9 ticks
    core 0 receives int from core 1
    core 0 delay 5 ticks
    core 1 receives int from core 0
    core 0 delay 5 ticks
    core 1 receives int from core 0
    core 1 delay 9 ticks
    core 0 receives int from core 1
    core 0 delay 5 ticks
    core 1 receives int from core 0
    core 1 delay 9 ticks
    core 0 receives int from core 1
    core 0 delay 5 ticks
    core 1 receives int from core 0
    core 0 delay 5 ticks
    core 1 receives int from core 0
    .................
