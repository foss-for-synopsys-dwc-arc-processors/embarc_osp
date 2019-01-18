.. _application_development:

Application Development
=======================

General Build and Run Steps
###########################

There are a lot of examples and demos in embARC OSP which are located in
``<embarc>/example`` folder. General steps are shown below to build the
provded examples and demos, e.g. the example in
``<embARC>/example/baremetal/bootloader`` for the EMSK 2.2 board and ARC EM7D
core configuration.

    1. Go to the example directory, ``cd <embARC>/example/baremetal/bootloader`` . ``<embARC>`` should be replaced by
    your actual path such as ``c:/embARC`` . Use ``\`` instead of ``/`` in the
    Windows environment.

    2. Run the ``make`` commands
       * Commands for using MetaWare toolchain

            * Command common part: ``gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw``
            * Build: ``gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw``
            * Run: ``gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw JTAG=usb run``
            * Debug: ``gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw JTAG=usb gui``
            * Generate binary: ``gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw JTAG=usb bin``
            * Show build configuration: ``gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw JTAG=usb cfg``
            * Show build options: ``gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw JTAG=usb opt``
            * Clean current build configuration: ``gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw clean``
            * Clean current all configurations for the same board: ``gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw boardclean``
            * Clean current all configurations in this example: ``gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw distclean``

       * Commands for using ARC GNU toolchain

            * Command common part: ``make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu``
            * Build: ``make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu``
            * Run: ``make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu JTAG=usb run``
            * Debug: ``make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu JTAG=usb gui``
            * Generate binary: ``make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu JTAG=usb bin``
            * Show build configuration: ``make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu JTAG=usb cfg``
            * Show build options: ``make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu JTAG=usb opt``
            * Clean current build configuration: ``make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu clean``
            * Clean current all configurations for the same board: ``make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu boardclean``
            * Clean current all configurations in this example: ``make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu distclean``

Visit :ref:`build_system_documentation` for more build system information.

Create a New Application
########################

Here, we will show how to create a new application in embARC OSP,  taking a simple ``hello world`` as an example.

* Goals

    * Baremetal application based on embARC OSP
    * Hardware: EMSK 2.2 - ARC EM7D Configuration
    * Print "Hello world from embARC" through UART at 115200 bps
    * Use GNU toolchain to running and debugging in the command line

1. Creat a folder named ``hello_world`` under ``embarc/example``.

2. Copy the makefile template ``example/example.makefile`` and ``main.c.tmpl``
   into ``hello_world`` folder and rename ``example.makefile`` to ``makefile``,
   rename ``main.c.tmpl`` to ``main.c``.

3. Change the configurations in makefile according to your hardware configuration and application.

    * Change the application name: change the value of ``APPL`` to ``helloworld``.

    * Change the board name: change the value of ``BOARD`` to ``emsk``. This
      option can also be given in cmd line       If not specified, the default value
      will be ``emsk``

    * Change the board version: change the value of ``BD_VER`` to ``22``.This
      option can also be given in cmd line       If not specified, the default value
      will be ``22`` for board ``emsk``.

    * Change the core configuration: change the value of ``CUR_CORE`` to
      ``arcem7d``. This option can also be given in cmd line. If not specified,
      the default value will ``arcem7d`` for board ``emsk`` and version ``22``.

    * Change the embARC OSP root: change the value of ``EMBARC_ROOT`` to
      ``../..``. ``EMBARC_ROOT`` can be relative path or an absolute path.

    * Add the middleware that you need for this application: Change the value
      of MID_SEL.

        * The value of ``MID_SEL`` must be the folder name in
          ``<embARC>/middleware``, such as ``common`` or ``lwip``.

        * If using ``lwip``, ``ntshell``, ``fatfs``, and ``common``, set
          ``MID_SEL`` to ``lwip ntshell fatfs common``.

        * Set it to ``common`` in the "HelloWorld" application.

    * Change your toolchain: change the value of ``TOOLCHAIN`` to ``gnu``.

    * Update source folders and include folder settings.

        * Update the C code folder settings: change the value of
          ``APPL_CSRC_DIR`` to ``.``. ``APPL_CSRC_DIR`` is the C code relative
          path to the application folder

        * Update the assembly source-folder settings: change the value of ``APPL_ASMSRC_DIR``.

        * Update the include-folders settings: change the value of
          ``APPL_INC_DIR`` which is the application include path to the
          application folder (-I).

        * If more than one directory is needed, use whitespace between the folder paths.

    * Set your application defined macros: Change the value of ``APPL_DEFINES``.

        * For example, if define ``APPLICATION=1``, set ``APPL_DEFINES`` to ``-DAPPLICATION=1``.

   Then ``hello_world`` application's makefile will be like this

    .. code-block:: makefile

            ## embARC application makefile template ##
            ### You can copy this file to your application folder
            ### and rename it to makefile.
            ##

            ##
            # Application name
            ##
            APPL ?= helloworld

            ##
            # Extended device list
            ##
            EXT_DEV_LIST +=

            # Optimization level
            # Please refer to toolchain_xxx.mk for this option
            OLEVEL ?= O2

            ##
            # Current board and core
            ##
            BOARD ?= emsk
            BD_VER ?= 22
            CUR_CORE ?= arcem7d

            ##
            # Debugging JTAG
            ##
            JTAG ?= usb

            ##
            # Toolchain
            ##
            TOOLCHAIN ?= gnu

            ##
            # Uncomment following options
            # if you want to set your own heap and stack size
            # Default settings see options.mk
            ##
            #HEAPSZ ?= 8192
            #STACKSZ ?= 8192

            ##
            # Uncomment following options
            # if you want to add your own library into link process
            # For example:
            # If you want link math lib for gnu toolchain,
            # you need to set the option to -lm
            ##
            #APPL_LIBS ?=

            ##
            # Root path of embARC
            ##
            EMBARC_ROOT = ../..

            ##
            # Middleware
            ##
            MID_SEL = common

            ##
            # Application source path
            ##
            APPL_CSRC_DIR = .
            APPL_ASMSRC_DIR = .

            ##
            # Application include path
            ##
            APPL_INC_DIR = .

            ##
            # Application defines
            ##
            APPL_DEFINES =

            ##
            # Include current project makefile
            ##
            COMMON_COMPILE_PREREQUISITES += makefile

            ### Options above must be added before include options.mk ###
            # Include key embARC build system makefile
            override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
            include $(EMBARC_ROOT)/options/options.mk


 4.  run and debug

    * Set your EMSK 2.2 hardware configuration to ARC EM7D, and connect it to
      your PC. Open ``PuTTY`` or ``Tera-term``, and connect to the right COM
      port. Set the baudrate to **115200 bps**.

    * Enter ``make run`` in the command line to run this application.

    * Enter ``make gui`` in the command line to debug this application. Press **CTRL + C** to stop the program. Enter ``q`` to exit the debugger. See the `GDB manual <https://sourceware.org/gdb/current/onlinedocs/gdb/>`_ for how to debug an application using arc-elf32-gdb.

.. note::

     See :ref:`make_options_for_embARC` for make options in detail. You can
     also refer to other embARC examples, e.g.,
     ``example/baremetal/openthread``, ``example/freertos/net/ntshell``.

IDE Project Usage
#################

Run and debug an embARC application in the ARC GNU IDE
------------------------------------------------------
A embARC "helloworld" application has been created and tested. This section provides steps for debugging the application in the ARC GNU IDE.

* Open the ARC GNU IDE and set the workspace location to *ide_projects/emsk_22/gnu* in the embARC root. Set the workspace location in a right way.

.. _embARC_helloworld_createideproject_gnu_pre:
.. figure:: /pic/images/developer/application_development/embARC_helloworld_createideproject_gnu_pre.jpg

   Create helloworld project in ARC GNU IDE

* When the GNU IDE opens, click **File->New->Makefile Project with Existing Code**. Set **Existing Code Location** to your "helloworld" application. Set **Toolchain for Indexer Settings** to **GNU Toolchain for ARC EM**, and click Finish.

.. _embARC_helloworld_createideproject_gnu:
.. figure:: /pic/images/developer/application_development/embARC_helloworld_createideproject_gnu.jpg

   Set helloworld project in ARC GNU IDE

* Build the projec in the IDE. When the build process is finished without any errors, open **Debug Configurations**, and create debug configuration for this project. Remember to choose the correct ELF file, and set the right **Gdbserver Settings** in the **Debugger** tab.

.. _embARC_helloworld_buildideproject_gnu:
.. figure:: /pic/images/developer/application_development/embARC_helloworld_buildideproject_gnu.jpg

   Create debug configuration helloworld project in ARC GNU IDE

* Click **Debug** and debug the project in the IDE. For more information, see `Debugging with OpenOCD in ARC GNU IDE <https://github.com/foss-for-synopsys-dwc-arc-processors/arc_gnu_eclipse/wiki/Debugging-with-OpenOCD>`_

.. _embARC_helloworld_buildideproject_gnu_result:
.. figure:: /pic/images/developer/application_development/embARC_helloworld_buildideproject_gnu_result.jpg

   Debug helloworld project in ARC GNU IDE

.. note:: There are only application source code files added in the IDE project. The makefile is provided by embARC, not generated by the ARC GNU IDE itself.

.. _application_development_debug_multiple_boards:

Debug Multiple Boards
#####################
Multiple EMSK boards are required to run or debug application code on a single PC for some examples in embARC, especially the Pmod RF examples. Here are alternative ways for two EMSK boards running and debugging with one PC.

* MetaWare Toolchain

  * In command-line mode, Ashling JTAG and USB-JTAG are necessary rather than multi-USB JTAGs. One node runs ``make TOOLCHAIN=mw xxxx JTAG=opella xx``, the other node runs ``make TOOLCHAIN=mw xxxx JTAG=usb xx``.
  * In IDE mode, change the debugger configuration in one of the two nodes for Ashling JTAG. Because the Digilent JTAG Cable is the default option.
  * Another way for command-line mode is to modify the DIG_NAME option to choose different boards. DIG_NAME is a MetaWare debugger property corresponding to ``-prop=dig_device=name``.

* ARC GNU Toolchain

  * In command-line mode, all nodes run ``make TOOLCHAIN=gnu xxx JTAG=usb xx``. OpenOCD selects the right board automatically. OpenOCD supports multi-USB-JTAG.
  * In IDE mode, all projects share the same port for the OpenOCD configuration. The same GNU debugger port (49105) is used. To avoid a conflict with GNU debugger ports, set the different GNU debugger ports in the debug configuration.

.. note:: The above methods only support two boards, e.g., one EMSK 1.x and one EMSK 2.x. Two EMSK 1.x boards or two EMSK 2.x boards are not supported.
