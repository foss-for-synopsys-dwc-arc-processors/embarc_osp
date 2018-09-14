.. _getting_started_with_hs_development_kit:

Getting Started with the ARC HS Development Kit
===============================================

Supported Hardware
##################

The ARC HS Development Kit (HSDK) uses an SoC from Synopsys featuring a quad-core ARC HS Processor (HS38x4) and integrated support for a rich set of peripherals including Ethernet, WiFi, Bluetooth, USB, I2C, SPI and UART. It is supported with tools and software necessary for Linux, RTOS and bare metal applications development. Linux distributions can be built for the platform using standard build systems (buildroot, Yocto/OE, OpenWrt) while RTOS & bare metal applications can be developed using the embARC Open Software Platform (OSP). I/O functionality can be extended using Digilent® Peripheral Modules (Pmods™), as well as Arduino Shields and mikrobus modules.

Setting Up HSDK
###############

#. The HSDK boot switches are shown below. There are multiple configurations, such as boot mirror, boot image location, boot start mode, core selection and multi-core mode. For example, the configurations in the following picture are boot image location: 0x1 (internal ROM), boot start mode: generate cpu_start autonomously after reset, core selection: 0x0 (HS38X4_0), and multi-core mode: 0x0 (signal-core). For complete HSDK information, see the **HS Development Kit User's Guide** provided with the HSDK.

.. figure:: /pic/images/getting_started/hsdk_dip_settings.png
   :scale: 80 %

   ARC HSDK DIP switch settings

#. Connect the USB cable to the HSDK mini-USB connector and to the USB port on your development host. If using the Ashling Opella-XD for ARC Debug Probe or Digilent JTAG-HS2 Probe, connect it to the right connector. Connect the power supply to the board and to the power outlet.

.. figure:: /pic/images/getting_started/hsdk_debug_interface.png
   :scale: 50 %

   ARC HSDK debug interface

#. You can use any terminal emulation program to view UART output from the HSDK. The USB connection provides both the debug channel and RS232 transport. Use Tera Term in Windows as an example.

   * Determine which COM port the HSDK is using.

      * Open the **Windows Control Panel** and select **Device Manager** in a Windows environment.
      * Expand **Ports (COM and LPT)** in the list. Select **USB Serial Port (COM x)** and Note the COM port number for the ARC board.

   * Configure serial terminal connection.

      * Launch Tera Term. Select **Serial** in **Setup**. Choose the appropriate COM port string in **Port** and 115200 in **Baud rate**, 8 bits data, 1 stop bit, no parity (115200-8-N-1). Click **OK** to set the port configuration.
      * You can optionally save your settings so they can be easily retrieved.
      * Select **New connection** in **File** and choose the appropriate COM in **Serial**.

#. Test serial output with HSDK.

      * Press the "RESET" button on the HSDK board to reset the board.
      * Confirm that you can see the header and self-test message printed to the console.

      .. code-block:: console

         ********************************
         **       Synopsys, Inc.       **
         **   ARC HS Development Kit   **
         ********************************
         ** IC revision: Rev 1.0
         Bootloader revision: Jun 12 2016, 08:54:06
         Bootloader location: 0x1 (internal ROM)
         Bootloader aux: 0x9 (sg15E,4Gb,x8,2r)
         Bootloader speedmode: Slow
         Bootloader multicore: 0x0 (Single-core)


#. (Optional) Connect the PmodWiFi and PmodRF2 to the HSDK as shown.

.. figure:: /pic/images/getting_started/hsdk_pmod_connection.png
   :scale: 80 %
