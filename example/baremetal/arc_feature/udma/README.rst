.. _example_arc_feature_udma:

ARC uDMA Example
################

Overview
********

 This example is designed to show how to use uDMA API and to compare uDMA copy with memory copy.

Detailed Description
====================

 * Extra Required Tools
    NO

 * Extra Required Peripherals
    NO

 * Design Concept
    This example is designed to show how to use uDMA API and to compare uDMA copy with memory copy.

.. note::
    Hardware support: all |arc| based platform with uDMA configured

Buidling and Running
********************

Taking |emsk| as exmaple, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/arc_feature/udma
    $ gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw run

.. note:: Make sure you have selected the correct configuration of |emsk| via dip switches and that you have reset the board (button above “R”).

You can uncomment the *APPL_DEFINES* line in this application makefile to overwrite the linker script settings.
If you uncomment the *APPL_DEFINES* line, then you can modify the *REGION_ROM* and *REGION_RAM* in ``appl_mem_config.h`` to proper values.

If you want to run in DDR, you can set this two values to *EXT_RAM*. When you
change this values defined in ``appl_mem_config.h`` in this example, you need to
clean the project first then rebuilt it.

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

	embARC Build Time: Mar 28 2018, 10:50:49
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	Performance benchmark timer overhead is 70 cycles
	Performance benchmark for both dmacpy and memcpy including overhead
	XFER_SIZE DESC_CONF CACHE_FLUSH XFER_START XFER_WAIT CACHE_INV DMACPY MEMCPY
	256 211 980 774 9543 722 12604 408
	512 95 1191 318 17646 944 20344 690
	768 95 1703 318 26332 1280 29840 1010
	1024 95 2215 318 34959 1577 39276 1330
	1280 95 2727 318 43652 1915 48819 1650
	1536 95 3239 318 52310 2213 58287 1970
	1792 95 3751 318 60993 2548 67817 2290
	2048 95 4263 318 69623 2885 77296 2610
	2304 95 4775 318 78277 3386 86963 2930
	2560 95 5287 318 86899 3557 96268 3250
	2816 95 5964 318 95603 3893 105985 3570
	3072 95 6311 318 104278 4229 115343 3890
	3328 95 6988 318 112936 4729 125178 4210
	3584 95 7500 318 121637 4901 134563 4530
	3840 95 7847 318 130277 5402 144051 4850
	4096 95 8359 318 138939 5573 153396 5208
	4352 95 8871 318 147644 6074 163114 5904
	4608 95 9383 318 156250 6245 172403 6270
	4864 95 9895 318 164924 6746 182090 6379
	5120 95 10400 318 173597 6901 191423 6944
	5376 95 11219 318 182252 7234 201230 7264
	5632 95 11729 318 190865 7571 210690 7326
	5888 95 12242 318 199630 7906 220303 7805
	6144 95 12702 318 208294 8242 229763 8002
	6400 95 12988 600 216958 8578 239331 8557
	6656 242 13775 392 225585 9062 249168 9132
	6912 241 14243 392 234310 9233 258531 9592
	7168 241 14440 392 242885 9734 267804 10070
	7424 241 14952 393 251532 9905 277135 9862
	7680 242 15785 392 260220 10406 287157 10235
	7936 242 16261 392 268880 10571 296458 10866
	8192 241 16868 393 277548 10892 306189 10719
	DMA only support 8K cell transfer