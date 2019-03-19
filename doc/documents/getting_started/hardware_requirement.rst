.. _hardware_requirement:

Hardware Requirement
====================

Board Requirement
#################
* ARC Board

  * ARC Software Development Platform
  * `ARC EM Starter Kit <https://www.synopsys.com/dw/ipdir.php?ds=arc_em_starter_kit>`_
  * `ARC HS Development Kit <https://www.synopsys.com/dw/ipdir.php?ds=arc-hs-development-kit>`_
  * `ARC IoT Development Kit <https://www.synopsys.com/dw/ipdir.php?ds=arc_iot_development_kit>`_
  * `ARC AXS <https://www.synopsys.com/dw/ipdir.php?ds=arc-software-development-platform>`_
  * `ARC Virtual Board based on nSIM <https://www.synopsys.com/dw/ipdir.php?ds=sim_nSIM>`_

See :ref:`board_bsp` for more board information. You can also find instructions on how to add your own board in :ref:`porting_guides`.

* Peripherals

  * Wireless Communications

    * `Digilent Pmod WiFi with Microchip MRF24WG0MA <https://store.digilentinc.com/pmodwifi-wifi-interface-802-11g/>`_
    * `Digilent Pmod RF2 with Microchip MRF24J40MA <https://store.digilentinc.com/pmod-rf2-ieee-802-15-rf-transceiver/>`_
    * `BLE Microchip RN4020 <https://www.microchip.com/wwwproducts/en/RN4020>`_
    * `BLE Huamao HM-10 <http://www.huamaosoft.cn/bluetooth.asp?id=1>`_
    * WiFi RW009

  * Sensor

    * `Digilent Pmod AD2 with Analog Devices AD7991 <https://store.digilentinc.com/pmod-ad2-4-channel-12-bit-a-d-converter/>`_
    *  `Digilent Pmod TMP2 with Analog Devices ADT7420 <https://store.digilentinc.com/pmod-tmp2-temperature-sensor/>`_
    *  `Digilent Pmod TMP3 with Microchip TCN75A <https://store.digilentinc.com/pmod-tmp3-digital-temperature-sensor/>`_

  * Audio & Display

    * `Digilent Pmod I2S with Cirrus Logic CS4344 <https://store.digilentinc.com/pmod-i2s-stereo-audio-output-retired/>`_
    * `SSD1306 Monochrome OLED Graphic Display <http://www.solomon-systech.com/en/product/display-ic/oled-driver-controller/ssd1306/>`_

  * Storage

    * SD Card, SDHC (8G preferred)

  * Wireless router or smart-phone hotspot (Android preferred)

    * WiFi supports IEEE 802.11b/g/n and can be connected to the Internet website AWS IoT
    * Encryption with WPA/WPA2 - AES

Not every peripheral listed above is required for by embARC OSP examples. Choose peripherals according to your application's requirements. You can also add your own peripherals in the embARC OSP.

Board Connections
###################
* EMSK

  * Pmod WiFi is connected to Pmod J5 connector.
  * Pmod RF2 is connected to Pmod J6 connector.
  * BLE HM-10 is connected to the Pmod J1 connector.
  * Pmod AD2 is connected to the Pmod J2 connector.
  * Pmod TMP2/TMP3 is connected to the Pmod J2 connector.
  * SSD1306 OLED module is connected to the Pmod J2 or J4 connector, depending on your application.
  * SD Card plugged into card slot J9.

.. _emsk_board_settings:
.. figure:: /pic/images/getting_started/emsk_board_settings.jpg
   :scale: 20 %
   :alt: emsk board setting

   EMSK board connections

All versions of EMSK have the same board connections.

.. note:: The above connections are set as default in the embARC OSP. You can set your board connections in *<embARC>/board*. The hardware may be damaged by wrong connections.

Setting Board Environment
##########################
* Set serial terminal

  * The default UART baudrate is 115200 bps.
  * For example, in the Tera Term menu, select **Setup -> Serial Port** and change the settings for port and baudrate.

* If using WiFi modules in the embARC OSP, a WiFi hotspot with **WPA/WPA2 PSK** encrypted is necessary. Some changes in *<embARC>/board* should be made before running embARC examples.

  * Check the *<embARC>/board/board.h*. And go to the target board header file, For example, go to *<embARC>/board/emsk/emsk.h* for EMSK.
  * Set the WiFi hotspot name to **embARC**.
  * Set the WiFi hotspot IP address range from **192.168.43.1** to **192.168.43.254**.
  * Set the WiFi password to **qazwsxedc**.

If the WiFi hotspot IP address settings cannot be changed, the settings in the board setting header file must be modified.

.. code-block:: c

  #define WF_IPADDR_1                   (192)
  #define WF_IPADDR_2                   (168)
  #define WF_IPADDR_3                   (43)
  #define WF_IPADDR_4                   (102)

  #define WF_NETMASK_1                  (255)
  #define WF_NETMASK_2                  (255)
  #define WF_NETMASK_3                  (255)
  #define WF_NETMASK_4                  (0)

  #define WF_GATEWAY_1                  (192)
  #define WF_GATEWAY_2                  (168)
  #define WF_GATEWAY_3                  (43)
  #define WF_GATEWAY_4                  (1)

  #define WF_HOTSPOT_IS_OPEN            (0)

  #define WF_IPADDR_DHCP                (1)

  #define WF_HOTSPOT_NAME               "embARC"
  #define WF_HOTSPOT_PASSWD             "qazwsxedc"

The IP address settings should be adjusted according to your hotspot settings.
  * For example, if your hotspot IP address is 192.168.1.1, your WiFi hotspot name is "myhotspot", and password is "12345678", change the settings as shown below. Make sure the IP address (WF_IPADDR_x) doesn't conflict with other WiFi devices in the same network.

  .. code-block:: c

    #define WF_IPADDR_1                   (192)
    #define WF_IPADDR_2                   (168)
    #define WF_IPADDR_3                   (1)
    #define WF_IPADDR_4                   (102)

    #define WF_NETMASK_1                  (255)
    #define WF_NETMASK_2                  (255)
    #define WF_NETMASK_3                  (255)
    #define WF_NETMASK_4                  (0)

    #define WF_GATEWAY_1                  (192)
    #define WF_GATEWAY_2                  (168)
    #define WF_GATEWAY_3                  (1)
    #define WF_GATEWAY_4                  (1)

    #define WF_HOTSPOT_IS_OPEN            (0)

    #define WF_IPADDR_DHCP                (1)

    #define WF_HOTSPOT_NAME               "myhotspot"
    #define WF_HOTSPOT_PASSWD             "12345678"

.. note:: If the hotspot IP address range is not similar to **192.168.43.xxx**, the IP address settings of the EMSK board should be changed in the target board header file. The baremetal examples don't support DHCP. A static IP address is required.
