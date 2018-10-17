.. _example_blinky:

Blinky
######

Overview
********

 This example is designed to test board without any extra peripheral.

Detailed Description
====================
 * Extra Required Tools
    NO

 * Extra Required Peripherals
    NO

 * Design Concept
    This example is designed to test board without any extra peripheral

 * Usage Manual
    Toggle all leds on board in 1s period

 * Extra Comments
    NO

Buidling and Running
********************

This example outputs to the console. It is supported by all platform. The commands
to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/blinky
    $ make BOARD=emsk BD_VER=11 CUR_CORE=arcem6 TOOLCHAIN=gnu run

If you do not have an EMSK development board, you can use the nSIM simulator
which have been installed in MetaWare IDE.

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/blinky
    $ gmake BOARD=nsim BD_VER=1506 CUR_CORE=arcemfull TOOLCHAIN=mw run

.. note:: Make sure you have selected the correct configuration of EMSK via dipswitches and that you have reset the board (button above “R”) to confirme its configuration

Sample Output
=============

If you are using the **nSIM**, one console msg wil be used to simulate the
bink of LEDs, you will see the following output in the CMD window when the
download is successful.

.. code-block:: console

    Microsoft Windows [Version 10.0.15063]
    (c) 2017 Microsoft Corporation. All rights reserved.

    C:\embarc_osp-master\example\baremetal\blinky>gmake BOARD=nsim TOOLCHAIN=mw -j4 run
    "Creating Directory    : " obj_nsim_1506/mw_arcemfull/application/.
    "Creating Directory    : " obj_nsim_1506/mw_arcemfull/arc
    "Creating Directory    : " obj_nsim_1506/mw_arcemfull/board
    "Creating Directory    : " obj_nsim_1506/mw_arcemfull/board/nsim/drivers/uart
    "Creating Directory    : " obj_nsim_1506/mw_arcemfull/library/clib
               .........................
    "Linking               : " obj_nsim_1506/mw_arcemfull/blinky_mw_arcemfull.elf
    "Download & Run obj_nsim_1506/mw_arcemfull/blinky_mw_arcemfull.elf"
    mdb -nooptions -nogoifmain -toggle=include_local_symbols=1 -nsim -off=binary_stdin -off=binary_stdout  -on=load_at_paddr -on=reset_upon_restart -off=flush_pipe -off=cr_for_more -OKN  -arcv2em -core2 -Xtimer0 -Xtimer1 -Xlib -Xmpy_option=6 -Xdiv_rem=radix2 -Xcode_density -Xatomic -Xbitscan -Xbarrel_shifter -mpu -mpu_regions=16 -Xdsp2 -Xdsp_complex -Xdsp_divsqrt=radix2 -Xdsp_itu -Xdsp_accshift=full -Xagu_large -Xxy -Xbitstream -Xfpus_div -Xfpu_mac -Xfpus_mpy_slow -Xfpus_div_slow -firq -interrupts=8 -ext_interrupts=6 -interrupt_priorities=16 -icache_feature=2 -iccm_base=0x00000000 -iccm_size=0x00080000 -dcache_feature=2 -dccm_base=0x80000000 -dccm_size=0x00080000 -rgf_num_banks=2 -rgf_banked_regs=32 -pc_width=32 -dmac -dmac_channels=2 -dmac_registers=0 -dmac_fifo_depth=2 -dmac_int_config=single_internal -run obj_nsim_1506/mw_arcemfull/blinky_mw_arcemfull.elf
    led out: ff, ff
    led out: ff00, ff
    led out: ff, ff
    led out: ff00, ff
    led out: ff, ff
    led out: ff00, ff
    led out: ff, ff
    led out: ff00, ff
    led out: ff, ff
    led out: ff00, ff
    led out: ff, ff
    led out: ff00, ff
    led out: ff, ff
    led out: ff00, ff
    led out: ff, ff
    .................

If you are using the **ARC EMSK** development board, you will see the leds
start to blink when the download is successful.
