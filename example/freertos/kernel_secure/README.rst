.. _example_kernel_secure:

FreeRTOS Kernel Secure
######################

Overview
========

This example is designed to show how FreeRTOS is integrated with SecureShield.
Here, trap_s is used to trigger a task context switch request. So application
should not use trap exception.

As container switch is not with task switch. So during a container call, task scheduler
should be suspended to avoid a task context.


Detailed Description
====================

* Extra Required Tools
  NO

* Extra Required Peripherals
  NO

* Usage Manual

  - Re-configure the EMSK 2.3 board to EM7D configuration.
  - run the following commands

For nSIM, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/freertos/kernel_secure
    $ gmake BOARD=nsim BD_VER=10 CUR_CORE=arcsem TOOLCHAIN=mw run

For EMSK 2.3, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/freertos/kernel_secure
    $ make BOARD=emsk BD_VER=23 CUR_CORE=arcem7d TOOLCHAIN=gnu run

.. note:: This example is only valid for arc processors with SecureShield feature (Currently, emsk 2.3 arcem7d and nsim arcsem)


Sample Output
=============

.. code-block:: console

	-------------------------------------------------------------
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

	embARC Build Time: Oct 15 2018, 15:56:56
	Compiler Version: ARC GNU, 7.3.1 20180319

	Rounds: 0

	Task2 is running and makes a container call...
	the secret is:i love embarc
	The performance data is:
	        task2->task1:535 cycles
	        task1->int:89 cycles
	        int->nest int:89 cycles
	        nest int->int:80 cycles
	        int->task2:254 cycles

	Task1 is running and makes a container call...
	container 2 is trusted
	container2 is trusted, no need to check password
	container2 provided an incorrect data pointer for storing the secret
	get secret failed - someone tried to hack the system?
	Exit from main function, error code:0....

	Rounds: 1
