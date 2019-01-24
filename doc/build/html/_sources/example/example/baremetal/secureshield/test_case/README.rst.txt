.. _example_secureshield_test_case:

SecureShield Test Case
######################

Overview
********

This example is embARC Secureshield Test Cases example.

Detailed Description
====================

 * Extra Required Tools
    NO

 * Extra Required Peripherals
    NO

 * Design Concept
    This example is designed to test the secureshield's API functionality.

 * Usage Manual
    Run this example and you will see the following output.

 * Extra Comments
 	NO

Buidling and Running
********************

It is supported by nSIM. The commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/secureshield/test_case
    $ gmake BOARD=nsim BD_VER=10 CUR_CORE=arcsem TOOLCHAIN=mw run

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

	embARC Build Time: Apr  2 2018, 17:29:42
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	timer0 interrupt prioirty is:-1
	container call from background to 1
	tst_func1 in container 1, arguments are:1
	soft interrupt 1 interrupt handler
	container call from 1 to 2
	tst_func4 in container 3, arguments are:1,2,3,4
	container call from 3 to 1
	tst_func0 in container 1, no arguments
	my container id is:1
	the id of caller is:3
	ret is:0
	soft interrupt 3 interrupt handler
	soft interrupt 1 interrupt handler
	ret is:7
	ret is:1
	tst_func2 in container 1, arguments are:1,2
	container call from background to 2
	ret is:5
	container call from background to 3
	tst_func6 in container 3, arguments are:1,2,3,4,5,6
	ret is:6
	container call from background to 4
	ret is:8
	IRQ_ACT is:0
	test function 7 in background container:7
	soft interrupt 0 interrupt handler