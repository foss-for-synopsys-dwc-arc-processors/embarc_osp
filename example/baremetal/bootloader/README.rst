.. _example_bootloader:

Simple Bootloader
#################

Overview
********

 This example is designed to work as a secondary bootloader for embARC, it will load boot.hex or boot.bin on SDCard and run that program. By default, this example will validate the binary image based on MCUBoot to do secure boot. And this example itself can be used as ntshell application.

Detailed Description
====================

 * Extra Required Tools
    - The requirements of MCUBoot image tools

 * Extra Required Peripherals
    - SDCard with boot binary(boot.hex or boot.bin)

 * Design Concept
    This example is designed to work as a secondary bootloader for embARC, it will load boot.hex or boot.bin on SDCard and run that program. And this example itself can be used as ntshell application.

    This example combines multiple usage cases, including the image is in SD or Flash, and whether or not to use MCUboot. The program flow of this example as following.

    .. image:: /pic/images/example/baremetal_bootloader/boot_loader_flow.jpg

 * Usage Manual
    As shown in the following picture, when the EMSK configuration in SPI flash is loaded into the FPGA, a simple primary bootloader is also loaded in ICCM. Through the primary bootloader, the application or secondary bootloader can be loaded into external memory (DDR memory), bootloader start address is 0x17F00004, ram address is 0x17F00000.

    For EMSK 1.x, bootloader core configuration must be arcem6, for EMSK2.x, bootloader core configuration must be arcem7d.
    For EMSK 1.0, it can be upgraded to 1.1 by 1.1's firmware. For EMSK 2.0/2.1/2.2, it can be upgraded to 2.3 by 2.3's firmware.

    .. image:: /pic/bootloader.jpg

    Here are steps for how to program the secondary bootloader application into onboard SPIFlash (Take EMSK2.3 - ARC EM7D as example) and automatically load and run *boot.hex* or *boot.bin* in SDCard.

    - Generate a secondary bootloader binary file

    - Program generated secondary bootloader binary file into SPIFlash
        + Insert SDCard to your PC, and copy the binary file *obj_emsk_23/gnu_arcem7d/emsk_bootloader_gnu_arcem7d.bin* to SDCard Root, and rename it to *em7d_2bt.bin*
        + Insert the SDCard to EMSK Board, please choose the right core configuration, build and run the *<embARC>/example/baremetal/bootloader* example, then press any button to stop auto boot process, and enter to ntshell command mode.
        + Then use ntshell command *spirw* to program the *em7d_2bt.bin* into spiflash.
            - Firstly, run *spirw* to show help
            - Secondly, run *spirw -i* to check SPIFlash ID, it should be **Device ID = ef4018**
            - Thirdly, run *spirw -w em7d_2bt.bin ram_addr ram_start* to program spiflash, the *ram_addr* and *ram_start* is the load address and program start address of this bootloader, and they are different in each board, please check they in the table below. Usually, developer creates *appl_mem_config.h* and set **APPL_DEFINES += -DUSE_APPL_MEM_CONFIG** in *makefile* to define the addresses.
            - Check the output message to see if it was programmed successfully.

            .. table:: Examples in embARC OSP
                :widths: auto

                =======================  ======================  ======================  =======================
                Board version            Use MCUBoot             ram_addr                ram_start
                =======================  ======================  ======================  =======================
                axs                      not support             user definition         user definition
                emsdp                    not support             user definition         user definition
                emsk                     true                    0x15000000              0x15000004
                emsk                     false                   user definition         user definition
                iotdk                    true                    0x00000000              0x00000004
                iotdk                    false                   user definition         user definition
                nsim                     not support             user definition         user definition
                =======================  ======================  ======================  =======================

            .. image:: /pic/images/example/baremetal_bootloader/emsk_bootloader_program2splflash.jpg

        + If programmed successfully, when the board is reset, make sure Bit 4 of the onboard DIP switch is ON to enable secondary bootloader run.
        + If the sdcard already contains the *boot.bin* in it, the bootloader will automatically load it from sdcard, if not, it will enter to ntshell mode.
        + You can goto the next step to generate the *boot.bin* for proper application you want to be auto-loaded in sdcard.

        .. image:: /pic/images/example/baremetal_bootloader/emsk_bootloader_onspiflash.jpg

    - When not using MCUBoot, generate *boot.bin* using any embARC example which ram start address should be 0x10000000 and use bootloader to run it

    - Know Issues
        + Bootrom of EMSK1.x is not able to load secondary bootloader on SPIFlash, you need a modified EMSK1.x mcs file to enable this function, please send request in forum about this mcs file.

    **The secondary bootloader is a complement of the primary bootloader, and provides the following functions:**
        - File operations on SD card
        - Operations on the EMSK, GPIO, I2C, SPI flash
        - Operations on ARC processors
        - Automatic boot from SD card, using following instructions:
            + program the bin file of bootloader into EMSK spiflash using spirw command **spirw -w bootloader.bin ram_addr ram_start** with the help of JTAG
            + the primary bootloader should be able to load the secondary bootloader
            + put the file you want to boot in the root directory of SD card, name it boot.bin
            + plug in SD card

        - Optional secure boot based on MCUBoot:
            + validate the binary image by SHA-256 hash function and asymmetric encryption RSA algorithm for the integrity check and signature verification
            + validate the new binary image for upgrade and do firmware upgrade

        - LED Status of loading application(boot.bin)
            + Start to load application: LED on board -> 0x0F
            + Load application finished: LED on board -> 0xFF, if application is running normally, LED will quickly change to 0x0
            + Load application failed: LED on board -> 0xAA
            + Skip loading application, and enter to NTShell runtime: LED on board -> 0x0

        - Type *help* command in ntshell to show the list of supported commands.

        .. image:: /pic/images/example/baremetal_bootloader/emsk_bootloader.jpg

 * Extra Comments
    - Bootrom of EMSK1.x is not able to load secondary bootloader on SPIFlash, you need a modified EMSK1.x mcs file to enable this function, please send
    - Make sure that the bootloader can only load application for extract the same core you built for.
    - Currently the 2nd bootloader ran in a different place(end of DDR) in DDR which has no conflict with other normal examples placed at start of DDR.
    - Bootloader example only support arc core configuration which has DDR with cache enabled.
    - Bootloader example runs in DDR ram, not in CCM.
    - Bootloader example itself can be treated as a ntshell example, you can run some ntshell commands using this example.
    - The bootloader example is also built optimized for the ARC core you selected, so the example built for one specified core may not run successfully
    - Supported core configurations
        + EMSK 1.1: EM6, EM6GP
        + EMSK 2.2/2.3: EM7D, EM11D

Building and Running
********************

- Generate a secondary bootloader binary file

.. note:: If you want to disable MCUBoot, you should set **USE_MCUBOOT = 0** in your makefile

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/bootloader
    $ gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw bin

- Generate a secondary bootloader binary file for iotdk use eflash

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/bootloader
    $ gmake BOARD=iotdk BD_VER=10 CUR_CORE=arcem9d TOOLCHAIN=mw LOCATION=eflash bin

If the binary file is generated successfully, you will output as follows:

.. code-block:: console

	"Compiling       : " ../../../middleware/ntshell/cmds/cmds_fs/ymodem/ymodemio.c
	"Compiling       : " ../../../middleware/ntshell/port/ntshell_usrcmd.c
	"Compiling       : " ../../../middleware/ntshell/port/ntshell_task.c
	"Archiving       : " obj_emsk_23/gnu_arcem7d/libmidntshell.a
	"Compiling       : " ../../../middleware/parson/parson.c
	"Archiving       : " obj_emsk_23/gnu_arcem7d/libmidparson.a
	"Compiling       : " ../../../arc/arc_timer.c
	"Compiling       : " ../../../arc/arc_cache.c
	"Compiling       : " ../../../arc/arc_exception.c
	"Compiling       : " ../../../arc/arc_udma.c
	"Assembling      : " ../../../arc/arc_exc_asm.s
	"Archiving       : " obj_emsk_23/gnu_arcem7d/libcpuarc.a
	"Compiling       : " ../../../library/clib/fatfs_dirent.c
	"Compiling       : " ../../../library/clib/malloc.c
	"Compiling       : " ../../../library/clib/embARC_sbrk.c
	"Compiling       : " ../../../library/clib/embARC_misc.c
	"Compiling       : " ../../../library/clib/embARC_syscalls.c
	"Compiling       : " ../../../library/clib/ya_getopt.c
	"Compiling       : " ../../../library/clib/embARC_target.c
	"Archiving       : " obj_emsk_23/gnu_arcem7d/liblibclib.a
	"Archiving       : " obj_emsk_23/gnu_arcem7d/libembarc.a
	"Linking         : " obj_emsk_23/gnu_arcem7d/emsk_bootloader_gnu_arcem7d.elf
	"Generating Binary obj_emsk_23/gnu_arcem7d/emsk_bootloader_gnu_arcem7d.bin"

- Generate *boot.bin* using any embARC example
    + Here take *<embarc_root>/example/freertos/kernel* for example

    + When using MCUBoot, you should modify **LIB_SEL += mcuboot** and **MID_SEL += mbedtls** in your makefile to enable MCUBoot. And you should use the imgtool to sign the generated binary

    .. code-block:: console

        $ cd <embarc_root>/example/freertos/kernel
        $ gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw bin
        $ gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw sign

      Or you can use sign command directly, which will generate binary firstly and sign it

    + When not using MCUBoot, the example ram start address should be 0x10000000

    .. code-block:: console

        $ cd <embarc_root>/example/freertos/kernel
        $ gmake BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=mw bin

    + Insert SDCard to PC, and copy generated binary file *obj_emsk_22/mw_arcem7d/freertos_kernel_mw_arcem7d.bin* to SDCard Root, and rename it to boot.bin
      Please note that when using MCUBoot, the origin binary name is *obj_emsk_22/mw_arcem7d/signed_freertos_kernel.bin*
    + Insert SDCard back to EMSK, make sure bit 4 of DIP Switch is ON, and press re-configure button above letter **C**, and wait for autoload.

.. note:: Make sure you have selected the correct configuration of EMSK via dipswitches and that you have reset the board (button above "R") to confirm its configuration

Sample Output
=============

The output depends on the *boot.bin*. This sample is using *<embarc_root>/example/freertos/kernel*.

.. image:: /pic/images/example/baremetal_bootloader/emsk_bootloader_loadbootbin.jpg

.. image:: /pic/images/example/baremetal_bootloader/emsk_bootloader_mcuboot.jpg
