.. _example_dma_spiflash:

uDMA Flash Driver
#################

Overview
********

 Compare SPIFlash working frequency using poll spi and spi with uDMA.

Detailed Description
====================

 - Extra Required Tools
      NO

 - Extra Required Peripherals
      NO

 - Design Concept
    This example is designed to test SPIFlash driver with uDMA enhanced.

 - Usage Manual
    Here we take EMSK 2.3 EM9D for example, you can run the program using Metaware toolset.

    - Re-configure the EMSK 2.3 board to EM9D configuration.
    - Command: ``gmake BD_VER=23 CUR_CORE=arcem9d run``.

 - Extra Comments
    - Tested in EMSK 2.3 EM9D, for drivers with DMA, spiflash working frequency can achieve 12Mhz.
    - For drivers without DMA, spiflash working frequency can achieve 2Mhz.
    - This example will stuck at the max frequency that SPI can achieve at that situation.
    - This example required that the ARC core configuration should has uDMA option and DW SPI also enabled DMA interface with uDMA

.. note::
   this example only support |emsdp| board.

Sample Output
=============

You will see the following output in the uart console window when the download is successful.

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

    embARC Build Time: Mar 16 2018, 09:58:46
    Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1
    DO SPIFlash DMA Test @1000000Hz
    Random value:4
    SPI FLASH ID:0xef4018
    SPI Flash Erase Finshed in 2882761 cycles
    SPI Flash Erase Finshed in 2630832 cycles
    SPI Flash Erase Finshed in 3804791 cycles
    SPI Flash Erase Finshed in 2630827 cycles
    SPI FLASH Write Read Verification successfully
    Do SPIFlash DMA Test @2000000Hz
    Random value:110
    SPI FLASH ID:0xef4018
    SPI Flash Erase Finshed in 2836835 cycles
    SPI Flash Erase Finshed in 1266410 cycles
    SPI Flash Erase Finshed in 2410582 cycles
    SPI Flash Erase Finshed in 1266409 cycles
    SPI FLASH Write Read Verification successfully
    Do SPIFlash DMA Test @3000000Hz
    Random value:8
    SPI FLASH ID:0xef4018
    SPI Flash Erase Finshed in 2882761 cycles
    SPI Flash Erase Finshed in 2882761 cycles
    SPI Flash Erase Finshed in 2882761 cycles
    SPI Flash Erase Finshed in 2882761 cycles
    SPI FLASH Write Read Verification successfully
    Do SPIFlash DMA Test @4000000Hz
        ............