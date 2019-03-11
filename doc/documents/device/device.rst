.. _device_hal:

Devices
=======

Overview
########

The ``device`` folder in the root of embARC contains the necessary software APIs
and implementations for common device IP and external peripheral drivers. The
device layer consists of the following components:

- **Device HAL**: a device hardware abstraction layer including WNIC (Wireless
  Network Interface Controller), UART (Universal Asynchronous
  Receiver/Transmitter), SPI (Serial Peripheral Interface), IIC (Inter-
  Integrated Circuit), I2S (Inter-IC Sound), GPIO (General Purpose Input Output) and SDIO (Secure
  Digital Input and Output).

- **IP Driver Implementation**: device driver implementations based on the above device
  HAL, including DesignWare IP implementations of UART, IIC, SPI, GPIO and
  SDIO devices and Designware ARC Subsystem IP implementation.

- **Peripheral Driver Implementation**: peripheral driver implementations such as
  flash, sensors and PMOD modules.

.. image:: /pic/embARC_device_hal.jpg
    :alt: embARC Device HAL


Device HAL
##########

The Device HAL provides abstraction for the following devices:

.. toctree::
   :maxdepth: 1

   gpio.rst
   i2c.rst
   i2s.rst
   spi.rst
   uart.rst
   sdio.rst
   wnic.rst
   trng.rst
   pwm_timer.rst


These \*.h files in ``device/inc`` and ``device/ip/up_hal/inc`` provide abstraction APIs and macros that require corresponding
implementations. embARC OSP provides the device implementations in
``device/ip/designware``, ``device/ip/subsystem``, ``device/peripheral``, and ``board/xxxx/drivers``.

Different device features are abstracted in the device HAL, such as UART, IIC,
and SPI. Object-oriented concepts are adopted to provide common device
operation APIs and device access APIs.

IP Drivers
##########

The IP drivers are placed in ``device/ip`` folder. Currently, the following
Designware IP drivers are supported:

* GPIO
* I2S
* I2C
* PWM_TIMER
* SDIO
* SPI
* TRNG
* UART

and the follong Designware ARC subsystem IP drivers are supported:

* GPIO
* UART
* I2C master
* SPI master
* I2S master

In the future, more Designware IP drivers will be added, e.g., USB, ethernet

Not limited to Designware IP drivers, other IP drivers can be easily supported which
can take Designware IP drivers as a reference.

Driver Instantiation
####################

The device drivers will be instantiated in board BSP based on specific hardware
information. Taking EMSK board as an example, its drivers' instantiation is located in
``board/emsk/drivers/ip/designware``. Different instantiations of same type driver will be
differentiated through ID.

There are 3 UARTs in EMSK, their instantiations are in ``dw_uart_obj.c`` and
ID allocations & other related definitions are in ``dw_uart_obj.h``.

External Peripheral Drivers
###########################

The external peripheral drivers are placed in ``device/peripheral`` folder. embARC OSP already
include some common external peripheral drivers, especially PMOD modules.

* MRF24J40, a 802.15.4 based transceiver.
* AD7991, a I2C interface AD converter.
* HM1X, a UART-BLE module.
* W25QXX, a SPI Flash chip.
* ADT7420, a I2C interface temperature sensor.
* TCN75, a I2C interface temperature sensor.
* MRF24G, a SPI WiFi module.
* RW009, a SPI WiFi module.
* RN4020, a UART-BLE module.
* MPU9250, a 9D IMU module.

