.. _example_kernel:

FreeRTOS Kernel
###############

Overview
********

 This example is designed to show the functionality of FreeRTOS.

 All the possible cases of task switch are included in this example and the corresponding time is measured:

 - the time of task switch caused suspend and reusme operation
 - the interrupt latency: response time and recover time
 - the time of mutex operation
 - the time of semaphore operation
 - the time of event operation
 - the time of data queue operation

Buidling and Running
********************

This example outputs to the console. It is supported by all platform. The commands to run this example are as follows:

.. code-block:: console

	$ cd <embarc_root>/example/freertos/kernel
	$ make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu run


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

	embARC Build Time: Mar  9 2018, 15:48:26
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	Benchmark CPU Frequency: 25000000 Hz
	Benchmark will run 200 times, please wait about 400 ms

	Average benchmark result list as follows:
	extra measurement cost : 26 cycles
	task2     ->  task1    : 235 cycles
	task1     ->  int      : 64 cycles
	int       ->  nest int : 82 cycles
	nest int  ->  int      : 76 cycles
	int       ->  task1    : 50 cycles
	task1     ->  task2    : 194 cycles
	mux: tsk1 ->  tsk2     : 369 cycles
	sem: tsk1 ->  tsk2     : 296 cycles
	evt: tsk1 ->  tsk2     : 281 cycles
	dtq: tsk1 ->  tsk2     : 344 cycles