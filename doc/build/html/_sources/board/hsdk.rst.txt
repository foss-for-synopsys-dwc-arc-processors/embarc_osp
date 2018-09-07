.. _board_hsdk:

ARC HS Development Kit
======================

The ARC HS Development Kit (HSDK) Platform supports the ARC HS34, HS36 and
HS38x4 quad core processors running at 1GHz. The ARC HSDK features 256 kByte
of on-chip SRAM and 4 GByte of DDR3-SDRAM. The software available from
Synopsys for the ARC HSDK includes pre-built SMP Linux image (plus the U-boot
bootloader) and the embARC OSP distribution for embedded systems. embARC OSP
source code includes bare metal and FreeRTOS device drivers and example
applications. Code development is made easy using the MetaWare Development
Toolkit, MetaWare Lite tools or the ARC GNU Tool Chain.

.. image:: /pic/hsdk_board.jpg
    :alt: ARC HS Development Kit


The ARC HS Development Kit features the following components:

- ARC HS SoC:
  - Quad Core HS38x4 Processor
  - DDR3 memory controller
  - GPU
  - USB, Ethernet, SDIO
  - DesignWare APB Peripherals (consult HSDK documentation for complete details)
- Memory / Storage:
  - DDR3-1333 (4 GB)
  - 2x SPI Flash (2 MB)
  - I2C EEPROM (3 KB)
- Interfaces:
  - USB2 (2x)
  - Ethernet (10/100/1000)
  - Audio  - line in/out
  - USB Data port (JTAG/UART)
  - Micro-SD Card
  - WIFI/BT module
  - ADC (6 channels)
  - RTT Nexus and JTAG connectors
- Extensions:
  - AXI Tunnel (32-bit, 150 MHz)
  - Arduino  - Interface headers (UNO R3 compatible)
  - mikroBUS headers
  - Pmod Interfaces (3x), PMOD A and PMOD B (12 pins), PMOD C (6 pins)

.. image:: /pic/hsdk_block_diagram.jpg
    :alt: ARC HSDK Block Diagram

.. image:: /pic/hsdk_chip.jpg
    :alt: ARC HS System on Chip (SoC) Block Diagram

The ARC HS SoC provides the following main features:

- Flexible, customizable IC architecture

  - Configurable / programmable boot scenarios
  - Configurable / programmable memory map

- DesignWare HS38x4 quad-core @ 1GHz

  - 64kByte instruction cache
  - 64kByte data cache
  - 256kByte ICCM (2 cores)
  - 256kByte DCCM (2 cores)
  - Memory Management Unit
  - Physical Address Extension (PAE)
  - 512kByte system level cache
  - Support for IO coherency
  - Support for Real-Time Trace

- Vivante GC7000 NanoUltra3T GPU Processing Unit @ 400Mhz
- Flexible clock generation

  - system clocks

    - 33MHz system reference clock input
    - PLL for DDR clock
    - PLL for TUNNEL clock
    - PLL for ARC clock
    - PLL for all other system clocks

  - audio

    - audio reference clock input (24.576 MHz)
    - integer divider(s) for audio serial clock

- 256kByte SRAM
- DDR3 interface

  - max speed grade DDR-1600 (800MHz)
  - data width 32bit
  - max row address width 16bit
  - max bank address width 3bit
  - max 2 memory ranks
  - max supported DDR memory size is 4GByte

- I2S TX / RX interface
- USB 2.0 Host interface
- SDIO interfaces
- 10/100/1000Mbps Ethernet RGMII interface
- AXI tunnel interface

  - source-synchronous
  - max freq 150MHz
  - max data throughput 600MByte/s

- UART interfaces
- I2C interfaces
- SPI interfaces
- PWM interfaces
- JTAG interface
