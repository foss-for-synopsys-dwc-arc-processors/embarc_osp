.. _example_secureshield_secret_normal:

SecureShield Secret Normal
##########################

Overview
********

This example is embARC Secureshield secret normal example.

Detailed Description
====================

 * Extra Required Tools
    NO

 * Extra Required Peripherals
    NO

 * Design Concept
    This example is designed to show how data could be protected with SecureShield. The application demonstrates a secure keystore component. It's similar to the 'secret' example, but protects the secrets differently:
        - all containers are normal containers, no secure privileges.
        - to initialize the secret data in container 1, a private const data section is used; others cannot access this data. In an application with secure persistent storage like EEPROM or flash, it would be stored there.
        - a shared memory region is created for container 1 and container 2 to exchange data; others cannot access this region.
        - an application specific linker script is created to show how shared memory is created and how the initialization data is protected.
        - for further information on the application, see the 'secret' application

 * Usage Manual
    When this example start to run, please try the following ntshell commands:
    	- Run ntshell command `main -h` to see how to run this example.
    	- Run ntshell command `main -p embarc` or `main -t` to get the secret.

 * Extra Comments
 	NO

Buidling and Running
********************

It is supported by nSIM, EMSK 2.2 and EMSK 2.3.

For nSIM, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/secureshield/secret_normal
    $ gmake BOARD=nsim BD_VER=10 CUR_CORE=arcsem TOOLCHAIN=mw run

For EMSK 2.2, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/secureshield/secret_normal
    $ gmake LIB_SECURESHIELD_VERSION=1 BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw run

For EMSK 2.3, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/secureshield/secret_normal
    $ gmake BOARD=emsk BD_VER=23 CUR_CORE=arcem7d TOOLCHAIN=mw run

Sample Output
=============

.. code-block:: console

	------------------------------------------------------------
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

	embARC Build Time: Apr  2 2018, 16:52:36
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	ntshell command info(Information) was registered!
	ntshell command main(Call main function) was registered!
	NTShell Task StartUp
	COM1>