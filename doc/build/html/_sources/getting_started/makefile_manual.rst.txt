.. _makefile_manual:

Makefile Manual
===============

This is an introduction to embARC makefile system, which including a set of tools that you could build, run and debug your code with.
This document also contains a quick manual helping you to find the right command for your project.

Here are quick in-page links of some mostly used commands:

* help_
* cfg_
* build_
* gui_
* run_
* distclean_

Commands Details
################

This make system is based on make/gmake. Please make sure your environment meets the :ref:`software_requirement`.

.. _help:

help
****
This is one of the most useful commands, you could use it whenever you have problem finding the right command.
Help information will be printed to your console.

.. code-block:: console

	make help

or:

.. code-block:: console

	gmake help

In the following part of this manual, we will only use ``make`` in example, you could change it to ``gmake`` anytime.

.. note:: make/gmake is provided in the MetaWare toolkit (gmake) and ARC GNU toolchain (make).

Some text

.. _cfg:

cfg
***

The basic way of this command is to enter ``make cfg``, the makefile system will check and print out current settings as well as supported configurations, showing below.

.. code-block:: console

	make cfg
	"=======Current Configuration======="
	"Host OS            : Windows"
	"Board              : emsk"
	"Hardware Version   : 22"
	"Core Configuration : arcem7d"
	"CPU Clock HZ       : "
	"Peripheral Clock HZ: "
	"Build Toolchain    : mw"
	"Optimization Level : O2"
	"Debug Jtag         : usb"
	"======Supported Configurations of emsk-22======"
	"Boards (BOARD)                 : axs emsk hsdk iotdk nsim"
	"Core Configurations (CUR_CORE) : arcem11d arcem7d arcem9d"
	"Build Toolchains (TOOLCHAIN)   : gnu mw"
	"Debug Jtags (JTAG)             : usb opella"

At supported Configurations, you can find different choices for your command, the uppercase words in parentheses are paramater names to be used in your command.
For example, the command below will execute your command for an *EMSK* board with *arcem7d* core, toolchain would be *gnu* and JTAG connection is via *USB*.

.. code-block:: console

	make BOARD=emsk CUR_CORE=arcem7d TOOLCHAIN=gnu JTAG=usb [your command]

For the details of command parameters, see :ref:`Useful_Options`

A handy application to use ``cfg``:
	Suppose we need to build the project on hsdk, but we are not sure the exact core name.
	Then we could use cfg command with BOARD=hsdk so makefile system could find avaiable cores for us.
	In this case there are *archs38_c0 archs38_c1 archs38_c2 archs38_c3* available.

.. code-block:: console

	make BOARD=hsdk cfg
	"=======Current Configuration======="
	"Host OS            : Windows"
	"Board              : hsdk"
	"Hardware Version   : 10"
	"Core Configuration : archs38_c0"
	"CPU Clock HZ       : "
	"Peripheral Clock HZ: "
	"Build Toolchain    : mw"
	"Optimization Level : O2"
	"Debug Jtag         : usb"
	"======Supported Configurations of hsdk-10======"
	"Boards (BOARD)                 : axs emsk hsdk iotdk nsim"
	"Core Configurations (CUR_CORE) : archs38_c0 archs38_c1 archs38_c2 archs38_c3"
	"Build Toolchains (TOOLCHAIN)   : gnu mw"
	"Debug Jtags (JTAG)             : usb opella"



.. _build:

build
*****

This command will clean and build the project with selected paramaters.

Here is a example using EMSK (version 1.1, arcem6 core), compiled with gnu.

.. code-block:: console

	make BOARD=emsk BD_VER=11 CUR_CORE=arcem6 TOOLCHAIN=gnu build

.. _gui:

gui
***

Debug command, will first compile and link the project, then load the project with debugger and start debugging.

Here is a example using EMSK (version 2.2, arcem9d core), compiled with Metaware and debug via usb connection.

.. code-block:: console

	make BOARD=emsk BD_VER=22 CUR_CORE=arcem9d TOOLCHAIN=mw JTAG=usb [your command]

.. _run:

run
***

When this command is used, makefile system will first compile and link the project.
Then it will try to download the project to the board and start running it. If no target board is found, it will return error.

Here is a example using IoTDK (version 1.0, arcem9d core), compiled with Metaware and debug via usb connection.

.. code-block:: console

	make BOARD=iotdk BD_VER=10 CUR_CORE=arcem9d TOOLCHAIN=mw JTAG=usb run

You may also use it wiout BD_VER and CUR_CORE options since they are default options.

.. code-block:: console

	make BOARD=iotdk TOOLCHAIN=mw JTAG=usb run

.. _distclean:

distclean
*********

This command will clean the directory, removing all object files of all boards.

.. code-block:: console

	make distclean


.. _Useful_Options:

Useful Options
##############

#. Boards (**BOARD**): Use this option to check config for different boards. Usually he default BOARD setting is EMSK.
#. Core Configurations (**CUR_CORE**): shows available core choices.
#. Build Toolchains (**TOOLCHAIN**):To choose from different toolchain.
#. Debug Jtags (**JTAG**): In most case the micro-usb interface on board supports JTAG debugging. Ashling's Opella-XD-ARC is also support by some boards.
#. Board version (**BD_VER**): Often used by EMSK which has many versions.

