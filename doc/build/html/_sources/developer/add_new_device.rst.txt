.. _add_new_device :

Adding a New Device
###################

Adding support for On-Chip IP
-----------------------------

If the HAL for the new device is already defined in embARC OSP, follow the steps below.

* Create a sub-folder in *device/ip*.
* Implement the driver according to the device HAL definition and existing DesignWare IP implementations.
* Add the corresponding makefile.

If the HAL of the new device is not defined, follow the steps below.

* Create a header file in *device/ip/ip_hal*.
* Add the HAL definition for the new device.
* Implement the driver according to the device HAL definition.
* Add the corresponding makefile.

Adding support for an On-Board or External Peripheral
-----------------------------------------------------

* Add the driver in device/peripheral folder according to the class of peripheral. For example, a flash device xxx should be in the folder *device/peripheral/flash/xxx* folder.
* Add the corresponding makefile.
