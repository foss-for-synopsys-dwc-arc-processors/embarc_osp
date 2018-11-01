.. _example_arc_feature_cache:

ARC Cache Example
#################

Overview
********

 This example is designed to test the cache module of |arc|. Besides, it shows how to use ARC cache API defined in |embarc|.

Detailed Description
====================

 * Extra Required Tools
    NO

 * Extra Required Peripherals
    NO

 * Design Concept
    This example is designed to test the cache module of |arc|. Besides, it shows how to use ARC Cache API defined in ``arc_cache.h``.

.. note::
    Hardware support: Only works for |arc| that contain cache, please check your |arc| configuration before running this example

Buidling and Running
********************

Taking |emsk| as exmaple, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/arc_feature/cache
    $ make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu run

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

	embARC Build Time: Mar 26 2018, 17:07:17
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	Data cache, ver:4, capacity:16384, ways:2, line len:32
	Instruction cache, ver:4, capacity:16384, ways:2, line len:32
	fill the data cache with data
	cache data:0,  mem data:0
	lock the data in data cache
	invalidate data cache, even it's locked
	cache data:3c3c3c3c,  mem data:3c3c3c3c
	invalidate one cache line
	cache the data into one cache line with 0xff
	cache data:ffffffff,  mem data:0
	flush the changed cache line
	cache data:ffffffff,  mem data:0
	cached the data into one cache line again with 0x55
	cache data:55555555,  mem data:0
	lock the changed cache line
	flush the whole data chache, the locked line won't be flushed
	cache data:55555555,  mem data:0
	refill the cache with data
	cache data:3020100,  mem data:3c3c3c3c
	flush multiple cache lines
	cache data:3020100,  mem data:3c3c3c3c
	Cache Test Done