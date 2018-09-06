.. _add_new_device :

Add New Device
##############

Add On-Chip IP
--------------

If the HAL of the new device is defined, follow the steps below.

* Create a sub-folder in *device/ip*.
* Implement the driver according to the device HAL definition and existing DesignWare IP implementations.
* Add corresponding makefile.

If the HAL of the new device is not defined, follow the steps below.

* Create a header file in *device/ip/ip_hal*.
* Add the HAL definition for the new device.
* Implement the driver according to the device HAL definition.
* Add corresponding makefile.

Add On-Board or External Peripheral
-----------------------------------

* Add the driver in device/peripheral folder according to the class of peripheral. For example, a flash device xxx should be in the folder *device/peripheral/flash/xxx* folder.
* Add corresponding makefile.
