.. _getting_started_with_em_starter_kit:

Getting Started with EM Starter Kit
===================================

Supported Hardware
##################

The ARC EM Starter Kit (EMSK) is an FPGA-based solution for rapid software development, code porting, software debugging, and profiling, preloaded with images for the ARC EM Family of processors. It can be extended using Digilent® Peripheral Modules (Pmods™).

EMSK v1.1 and v2.3 are supported in embARC OSP.

Setting Up EMSK
###############

#. The EMSK has a bank of DIP switches labeled SW1 that are used to select a configuration. For example, to select ARC EM11D for EMSK 2.3, set bit 2 to the ON position (down), and the others to OFF (up) as shown.

.. figure:: /pic/images/getting_started/emsk23_dipswitch.jpg
   :scale: 40 %

   DIP switch settings for ARC EM11D on EMSK 2.3

For exact DIP switch settings for various ARC EM Starter Kit versions, refer to EM Starter Kit section in provided embARC documentation. For complete ARC EM Starter Kit information, see the **EM Starter Kit User's Guide** provided with the ARC EM Starter Kit.

#. Connect the USB cable to connector J7 on the EMSK to USB port on your development host. If using the Ashling Opella-XD for ARC Debug Probe, connect it to the J15 connector on EMSK.

.. note:: Despite the fact that EMSK 2.3 can be powered by connector J7, it is recommended to always use an AC adapter to supply power of the board.

#. You can use any terminal emulation program to view UART output from the EMSK. The USB connection provides both the debug channel and RS232 transport. Use PuTTY application on Windows as an example.

   * Determine which COM port the EMSK is using.

      * Open the **Windows Control Panel** and select **Device Manager** in a Windows environment.
      * Expand **Ports (COM and LPT)** in the list. Select **USB Serial Port (COM x)** and note the COM port number for the ARC board.

   * Configure serial terminal connection.

      * Launch PuTTY. Select **Serial** under **Connection type** and enter the appropriate COM port string under **Serial line**.
      * Choose 115200 baud, 8 bits, 1 stop bit, no parity (115200-8-N-1) in settings.
      * You can optionally save your settings so they can be easily retrieved every time PuTTY is launched.
      * Click Open to open the console.

#. Test serial output with EMSK.

      * Press the middle ("R"eset) button above the **ARC** label on the EMSK board to reset the board and run the self-test.
      * Use EMSK 2.3 as an example. Confirm that you can see the header and self-test message printed to the console.

      .. code-block:: console

         ***********************************
         **       Synopsys, Inc.          **
         **     ARC EM Starter kit        **
         **                               **
         ** Comprehensive software stacks **
         **   available from embARC.org   **
         **                               **
         ***********************************
         Firmware   Feb 22 2017, v2.3
         Bootloader Feb 22 2017, v1.1
         ARC EM11D, core configuration #3

         ARC IDENTITY = 0x43
         RF_BUILD = 0xc902
         TIMER_BUILD = 0x10304
         ICCM_BUILD = 0x804
         DCCM_BUILD = 0x10804
         I_CACHE_BUILD = 0x135104
         D_CACHE_BUILD = 0x215104

         SelfTest PASSED

         Info: No boot image found

#. (Optional) Connect the PmodWiFi, PmodRF2, and PmodTMP2 modules to the EMSK as shown.

.. figure:: /pic/images/getting_started/emsk_board_settings.jpg
   :scale: 15 %
   :alt: emsk board setting

   EMSK board connection
