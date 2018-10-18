.. _getting_started_with_iot_development_kit:

Getting Started with IoT Development Kit
========================================

Supported Hardware
##################

The DesignWare® ARC® IoT Development Kit (IoTDK) is a versatile platform that includes the necessary hardware and software to accelerate software development and debugging of sensor fusion, voice recognition and face detection designs. RTOS & bare metal applications can be developed using the embARC Open Software Platform (OSP). I/O functionality can be extended using Digilent® Peripheral Modules (Pmods™), as well as Arduino Shields and mikrobus modules.

For further information, see :ref:`board_iotdk`

Setting Up IoTDK
################

#. Switch DIP switches to the OFF position (closest to the edge of the board).

#. Connect your PC and IoTDK board with USB cable to micro USB port next to 5V DC power connector on board.

#. (Optional) Connect 5V DC power connector with 5V AC/DC adapter.

#. You can use any terminal emulation program to view UART output from the IoTDK. The USB connection provides both the debug channel and RS232 transport. Use PuTTY application on Windows as an example.

   * Determine which COM port the IoTDK is using.

      * Open the **Windows Control Panel** and select **Device Manager** in a Windows environment.
      * Expand **Ports (COM and LPT)** in the list. Select **USB Serial Port (COM x)** and note the COM port number for the ARC board.

   * Configure serial terminal connection.

      * Launch PuTTY. Select **Serial** under **Connection type** and enter the appropriate COM port string under **Serial line**.
      * Choose 115200 baud, 8 bits, 1 stop bit, no parity (115200-8-N-1) in settings.
      * You can optionally save your settings so they can be easily retrieved every time PuTTY is launched.
      * Click Open to open the console.

#. Test serial output with IoTDK.

      * Press the reset button on the IoTDK board to reset the board and run the uboot.
      * Confirm that you can see the message printed to the console as shown below.

      .. code-block:: console

         U-Boot 2018.03-rc3-00028-g161e5eeaa4a4-dirty (Mar 05 2018 - 12:56:14 +0300)

         DRAM:  128 KiB
         MMC:   Synopsys Mobile storage: 0
         Loading Environment from FAT... Card did not respond to voltage select!
         ** Bad device mmc 0 **
         Failed (-5)
         In:    serial0@80014000
         Out:   serial0@80014000
         Err:   serial0@80014000
         =>


#. (Optional) Connect peripheral modules to the IoTDK as per :ref:`peripheral_preparation`.

