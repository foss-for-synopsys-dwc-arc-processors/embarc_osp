.. _example_graphic_u8glib:

Graphic u8glib Example
######################


Overview
********

This example is a graphic test based on OLED ssd1306 and U8glib.


Detailed Description
====================
 * Extra Required Tools
    NO

 * Extra Required Peripherals
    PMOD OLED based on SSD1306 128x64 monochrome

 * Design Concept
    It demonstrates how to use U8glib functions of drawing box, line and ASCII characters. Dynamic graphics are shown in SSD1306.
    And display width & height information is shown in Tera Term

 * Usage Manual
    Outputs pitch, roll, yaw get from MPU9250 DMP to the console.

 * Extra Comments
    NO

Building and Running
********************

The commands to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/graphic_u8glib
    $ make BOARD=emsk BD_VER=22 CUR_CORE=arcem7d TOOLCHAIN=gnu run

.. note:: As IoTDK's pmod interface don't support I2C, please use the i2c pins of arduino interface 	mikro bus to connect pmod OLED.
