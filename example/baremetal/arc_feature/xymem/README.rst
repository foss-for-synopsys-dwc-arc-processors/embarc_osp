.. _example_arc_feature_xymem:

ARC XY Memory Example
#####################

Overview
********

This example is designed to show how to use |arc| XY memory and Address Generation Unit (AGU)
which can accerlate the DSP application.

Detailed Description
====================

 * Extra Required Tools
      NO

 * Extra Required Peripherals
      NO

 * Design Concept
    This example is designed to show how to use |arc| XY memory and Address Generation Unit (AGU). The job is to add *DAT_SIZE* uin32_t number, i.e. c = a + b. Two approaches are adopted
    to do the job, one tradition way and one accelrated by xy memory

.. note::
    Hardware support: all |arc| based platform with XY memory and AGU configured. If not configured, the consumed cycles of two approaches are the same.

Buidling and Running
********************

Taking |emsk| as exmaple, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/arc_feature/xymem
    $ make BOARD=emsk BD_VER=22 CUR_CORE=arcem9d TOOLCHAIN=gnu run

.. note:: Make sure you have selected the correct configuration of |emsk| via dip switches and that you have reset the board (button above “R”).

Sample Output
=============

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

	embARC Build Time: Jan  7 2019, 14:22:51
	Compiler Version: Metaware, 4.2.1 Compatible Clang 6.0.1 (branches/release_60)
	add with xy ccm for 1024 numbers : 2093 cycles
	add for 1024 numbers : 5153 cycles

.. note:: Make sure you have chosen correct COM port and set the baud rate to 115200 before you start.
