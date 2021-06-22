.. _example_arc_feature_memory_protection:

ARC Memory Protection Example
#############################

Overview
********

 This example is designed to show how to use |arc| Memory Protection Unit(MPU) and the
 switch between user mode and kernel mode.

Detailed Description
====================

 * Extra Required Tools
      NO

 * Extra Required Peripherals
      NO

 * Design Concept

   This example is designed to show how to use |arc| Memory Protection Unit(MPU) and the
   switch between user mode and kernel mode. The violations of read, write and execute are raised
   and caught by MPU. The privilege violations are also caught.

.. note::
    Hardware support: all |arc| based platform with MPU configured

Building and Running
********************

Taking |emsk| as example (only em7d configuration has MPU), the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/arc_feature/memory_protection
    $ make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu run

.. note:: Make sure you have selected the correct configuration of |emsk| via dip switches and that you have reset the board (button above “R”).

Sample Output
=============

If you are using the ``nSIM``, you will see the following outputs in the CMD window when download is successful.
If you are using the |emsk|, you will see the same msg through UART

.. note:: Make sure you have chosen correct COM port and set the baud rate to 115200 before you start.

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

	embARC Build Time: Dec 26 2019, 15:03:14
	Compiler Version: Metaware, 4.2.1 Compatible Clang 6.0.1 (branches/release_60)
	raise violation of write to ROM
	Memory write protection violation: MPU
	raise violation of execute in RAM
	Instruction fetch violation: MPU
	go to user mode
	Memory write protection violation: MPU
	Instruction fetch violation: MPU
	Memory read protection violation: MPU
	Privilege violation
	end of program
