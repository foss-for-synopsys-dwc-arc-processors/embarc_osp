.. _board_bsp:

Boards
======

Overview
########

The **Board Support Package (BSP) Layer** provides support for different
boards based on |arc|. The layer contains the following functions:

- **Drivers instantiation**   In embARC OSP, an object-oriented design is applied
  for device drivers. The peripheral driver implementations are similar to the
  definition of class in the device abstraction layer. So the peripheral drivers
  should be instantiated for every board.   For example, ``dw_gpio_obj.c`` /
  ``dw_gpio_obj.h`` is the instantiation of the DesignWare GPIO driver
  implementation in device abstraction layer, which contains the information of
  register address, interrupt number, etc.

- **Common board initialization process** A standard bootup process is
  defined for each board. The entry of this process is ``board_main`` defined
  in ``board/board.c``, it will cover board initialization, os environment
  setup, fatfs and ntshell initialization, finally it will enter to ``main``
  function if required.

- **Board resource unified definition**  Most boards share common peripherals.
  Unified definitions are provided such as UART used to print messages, SD card
  interface, buttons, LEDs.

- **Linker script for boards** Linker script is auto-generated from
  template using the ICCM/DCCM and DDR memory information. embARC OSP provides templates
  for both ARC GNU and MetaWare toolchains, located in ``board`` folder, called
  ``linker_template_gnu.ld`` and ``linker_template_mw.ld``.

Common Board Initialization Process
###################################

The common ``board_main`` function in ``board.c`` will do the following operations:

- Board initialization steps - defined in ``board_init`` function.

  - Install required device driver objects, such as UART/SPI/IIC device objects.
  - Clear and disable all interrupts.
  - Initialize 1 ms timer interrupt.
  - Initialize some peripherals if used.
  - Initialize other required board resources such as mux controller.

- xprintf library setup to provide printf functionality
- Initialization of FatFs if FatFs middleware is selected
- Initialization of FreeRTOS tasks if defined
- Go to ``main`` function

Supported Boards
################

The BSP Layer currently supports the following boards.

.. toctree::
   :maxdepth: 1

   axs.rst
   emsk.rst
   emsdp.rst
   hsdk.rst
   iotdk.rst
   nsim.rst


* AXS board support can be found in ``board/axs``, detailed in ARC AXS Software
  Development Platform (AXS). It includes the following sub-folders:

  * :file:`common`: AXS board initialization functions and 1 ms interrupt timer function
  * :file:`drivers`: Various peripheral drivers for various examples
  * :file:`configs`: Different versions of axs, and various core configurations for different version

* EM Starter Kit (EMSK) support can be found in ``board/emsk``. It includes the following sub-folders.

  * :file:`common`: EMSK board initialization functions and 1 ms interrupt timer function
  * :file:`drivers`: Various peripheral drivers for various examples
  * :file:`configs`: Different versions of emsk, and various core configurations for different version

* EM Software Development Package (EM SDP) support can be found in ``board/emsdp``. It includes the following sub-folders.

  * :file:`common`: EMSK board initialization functions and 1 ms interrupt timer function
  * :file:`drivers`: Various peripheral drivers for various examples
  * :file:`configs`: Different versions of emsdp, and various core configurations for different version

* HS Development Kit (HSDK) board support can be found in ``board/hsdk``. It includes the following sub-folders.

  * :file:`common`: HSDK board initialization functions and 1 ms interrupt timer function
  * :file:`drivers`: Various peripheral drivers for various examples
  * :file:`configs`: Support for different board versions (if applicable), and support for different core configurations for each board version (if applicable)

* IoT Development Kit (IoTDK) board support can be found in ``board/iotdk``. It includes the following sub-folders.

  * :file:`common`: IoTDK board initialization functions and 1 ms interrupt timer function
  * :file:`drivers`: Various peripheral drivers for various examples
  * :file:`configs`: Support for different board versions (if applicable), and support for different core configurations for each board version (if applicable)

* nSIM virtual board support can be found in ``board/nsim``. It includes the following sub-folders.

  * :file:`common`: nSIM virtual board initialization functions and 1 ms interrupt timer function
  * :file:`drivers`: Basic hostlink UART driver for message input and output
  * :file:`configs`: Different versions of nsim, and various core configurations for different version

.. note:: MetaWare license is required for nSIM virtual board.

Board Resource Unified Definitions
##################################

The board resource definitions are defined in ``board.h``, where the hardware header files of each board are included. The following are the common board resources definitions. For each board, it can implement these definitions according to its own configurations. Through these common definitions, the code re-usability is improved.

.. table:: Board resource definition
   :widths: auto

   ====================================  ==========================================================
    Resource Definitions                  Description
   ====================================  ==========================================================
    BOARD_CONSOLE_UART_ID                 Console UART ID
    BOARD_ADC_IIC_ID                      AD converter IIC interface ID
    BOARD_TEMP_SENSOR_IIC_ID              Temperature sensor IIC interface ID
    BOARD_TEMP_IIC_SLVADDR                Temperature sensor IIC slave address
    BOARD_SDCARD_SPI_ID                   SD card SPI interface ID
    BOARD_WIFI_SPI_ID                     WiFi SPI interface ID
    BOARD_SFLASH_SPI_ID                   SPI flash SPI interface ID
    BOARD_SDCARD_SPI_LINE                 SD card SPI interface CS line
    BOARD_WIFI_SPI_LINE                   WiFI SPI interface CS line
    BOARD_SFLASH_SPI_LINE                 SPI flash SPI interface CS line
    BOARD_SYS_TIMER_ID                    System 1 ms interrupt timer ID
    BOARD_SYS_TIMER_INTNO                 System 1 ms timer interrupt vector no
    BOARD_SYS_TIMER_HZ                    System timer count freq. (1KHz as default)
    BOARD_SPI_FREQ                        Board SPI interface common freq.
    BOARD_SYS_TIMER_MS_HZ                 Board timer count freq.
    BOARD_SYS_TIMER_MS_CONV               Converter of board to system timer count freq.
    BOARD_OS_TIMER_ID                     OS kernel timer ID
    BOARD_OS_TIMER_INTNO                  OS kernel timer interrupt no
    BOARD_CPU_CLOCK                       CPU clock of the board
    BOARD_DEV_CLOCK                       Device clock of the board
    BOARD_LED_MASK                        LED Mask, if 8 LED, mask is 0xff
    BOARD_LED_CNT                         LED count
    BOARD_BTN_MASK                        Button mask
    BOARD_BTN_CNT                         Button count
    BOARD_SWT_MASK                        Switch mask
    BOARD_SWT_CNT                         Switch count
    BOARD_ONBOARD_NTSHELL_ID              Default NtShell ID
    OSP_DELAY_OS_COMPAT_ENABLE            Delay function is OS compact
    OSP_DELAY_OS_COMPAT_DISABLE           Delay function isn't OS compact
    WF_IPADDR_1                           WiFi device static IP address 1
    WF_IPADDR_2                           WiFi device static IP address 2
    WF_IPADDR_3                           WiFi device static IP address 3
    WF_IPADDR_4                           WiFi device static IP address 4
    WF_NETMASK_1                          WiFi device static netmask 1
    WF_NETMASK_2                          WiFi device static netmask 2
    WF_NETMASK_3                          WiFi device static netmask 3
    WF_NETMASK_4                          WiFi device static netmask 4
    WF_GATEWAY_1                          WiFi device static gateway 1
    WF_GATEWAY_2                          WiFi device static gateway 2
    WF_GATEWAY_3                          WiFi device static gateway 3
    WF_GATEWAY_4                          WiFi device static gateway 4
    WF_HOTSPOT_IS_OPEN                    Specify whether WiFi hot spot is open or not
    WF_IPADDR_DHCP                        Use DHCP to get ip or not
    WF_HOTSPOT_NAME                       WiFi hot spot name
    WF_HOTSPOT_PASSWD                     WiFi hot spot password when not open
    WF_ENABLE_MANUAL_SET_MAC              Enable to set MAC manually or not
    WF_MAC_ADDR0                          WiFi MAC address 0 (manual set)
    WF_MAC_ADDR1                          WiFi MAC address 1 (manual set)
    WF_MAC_ADDR2                          WiFi MAC address 2 (manual set)
    WF_MAC_ADDR3                          WiFi MAC address 3 (manual set)
    WF_MAC_ADDR4                          WiFi MAC address 4 (manual set)
    WF_MAC_ADDR5                          WiFi MAC address 5 (manual set)
    BOARD_PMWIFI_0_ID                     Pmod WiFi 0 ID
    BOARD_PMWIFI_ID_MAX                   Max Pmod WiFi ID
    OSP_GET_CUR_SYSHZ                     Macro to get current syshz
    OSP_GET_CUR_MS                        Macro to get current ms count
    OSP_GET_CUR_US                        Macro to get current us count
    OSP_GET_CUR_HWTICKS                   Macro to get current hardware ticks count
    board_init                            Function for common board initialization
    board_timer_update                    Call in period interrupt when using timer interrupt
    board_delay_ms                        MS delay for OS support
   ====================================  ==========================================================
