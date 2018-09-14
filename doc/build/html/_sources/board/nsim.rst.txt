.. _board_nsim:

Virtual Board based on ARC nSIM
===============================

Introduction
------------

The DesignWare® ARC® nSIM Instruction Set Simulator provides an instruction
accurate processor model for the DesignWare ARC processor families. Such
processor models take the software development out of your products’ critical
path by enabling an early start as well as increased efficiency through
enhanced visibility and control. A basic test environment for nSIM is
implemented in embARC, where hostlink IO is used for message input and output.
ARC related functions can be tested using nSIM such as internal timer, cache
module, interrupt and exception module.

.. note::
	Metaware license is required to install nSIM or Metaware IDE for nSIM simulation.

How to Run in embARC
--------------------

For example, go to ``example/baremetal/arc_feature/timer_interrupt``, then build
this application for ``nSIM - 10 - arcem`` using MetaWare or ARC GNU
toolchain. Here are the steps for running or debugging this application in
nSIM.

Using the MetaWare toolchain
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here are the steps for MetaWare toolchain, running in nSIM environment.

- Step 1: Open one command terminal in this application folder.

- Step 2: Build and run this application using this command: ``gmake BOARD=nsim
  BD_VER=10 CUR_CORE=arcem TOOLCHAIN=mw run``

The following output will be printed on the console:

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

        embARC Build Time: Apr  3 2018, 14:13:50
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

Using the ARC GNU toolchain
^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you want to run or debug embARC application using the ARC GNU toolchain, you also need to install nSIM tool.

- Make sure that you have installed nSIM tool to your OS, and you can run
  ``nsimdrv`` command in command line.

- For running or debugging embARC application using the ARC GNU toolchain, you
  need to run two commands in two command terminals, here take nsim-10-arcem
  core configuration for application ``example/baremetal/arc_feature/timer_interrupt``.

  + Step 1: Open two command terminals in this application folder.

  + Step 2: Start nsim debug server using this command:``make BOARD=nsim
    BD_VER=10 CUR_CORE=arcem TOOLCHAIN=gnu nsim``

  + Step 3: Run or debug this application using this command: ``make
    BOARD=nsim BD_VER=10 CUR_CORE=arcem TOOLCHAIN=gnu run``

  + Step 4: Exit the debug process by exiting these two commands.

The following output will be printed on the console:

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

    embARC Build Time: Apr  3 2018, 14:18:28
    Compiler Version: ARC GNU, 7.1.1 20170710
    ARC timer and interrupt
    interrupt number:8, extern interrupts:6, interrupt priorities:16, fiq enabled
    timer 0 is present
    cnt:13357
    timer 1 is present
    cnt:195664
    timer0 interrupt with the highest priority(fiq if fiq enabled):1
    timer0 interrupt with the highest priority(fiq if fiq enabled):2
    stop timer0, cnt:10
    timer1 interrupt:1
    timer1 interrupt:2
    stop timer1, cnt:9
    enable timer 0 watchdog
    ---end----

.. note::
	For the ARC GNU toolchain, you need to run two commands to debug or run application in nSIM.
	Most middleware packages in embARC are not supported with nSIM since there is no external peripheral for nSIM.
