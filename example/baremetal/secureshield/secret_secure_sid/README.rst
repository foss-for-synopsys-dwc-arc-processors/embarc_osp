.. _example_secureshield_secret_secure_sid:

SecureShield Secret Secure SID
##############################

Overview
********

This example is embARC Secureshield secret with SID example.

Detailed Description
====================

 * Extra Required Tools
    NO

 * Extra Required Peripherals
    NO

 * Design Concept
    This example is designed to show how data could be protected with SecureShield. The application demonstrates a secure keystore component.
        - The secret and password are protected in the context of container 1 which is a container with secure privileges.
        - You need to input the right password to get the secret from the non-trusted background container; container 2 is trusted and can access the secret without password.
        - The init value of secret and password are stored in the data section of secure container 1. Non-secure containers can not access them directly.
        - Secure container 2 can use the container 1 secret internally for implementing other services. The background application can request such a service, without knowing or obtaining the secret itself.
        - For demonstration purpose, the background container can obtain the secret as well (using the password). Since the background container stores the result in global, public memory, this is normally not secure and not recommended. Instead see Container 2 for how to securely use the secret.
        - The difference between this example and secret secure example is MPU reprogramming is avoidded through SID enabled. All containers MPU configuration will be loaded into MPU hardware.

 * Usage Manual
    When this example start to run, please try the following ntshell commands:
    	- Run ntshell command main -h to see how to run this example.
    	- Run ntshell command main -p embarc or main -t to get the secret.

 * Extra Comments
 	NO

Buidling and Running
********************

It is supported by nSIM and EMSK 2.3.

For nSIM, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/secureshield/secret_secure_sid
    $ gmake BOARD=nsim BD_VER=10 CUR_CORE=arcsem TOOLCHAIN=mw run

For EMSK 2.3, the commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/secureshield/secret_secure_sid
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

	embARC Build Time: Apr  2 2018, 17:16:58
	Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
	ntshell command info(Information) was registered!
	ntshell command main(Call main function) was registered!
	NTShell Task StartUp
	COM1>