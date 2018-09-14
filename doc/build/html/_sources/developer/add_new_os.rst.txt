.. _add_new_os:

Adding support for a new OS
###########################

Follow the steps below to add a new OS to embARC OSP.

* Include **<target-os>.mk** in **os.mk**. Add corresponding definitions such as OS_ID, OS_CSRDIR, OS_ASMSRCDIR, and OS_INCDIR.
* Add necessary definitions in os_hal_inc.h, such as the header-files path of **<target-os>** and the definition of os_hal_exc_init. The **os_hal_exc_init()** function is provided to initialize the exception and interrupt-related features of **<target-os>**.
* Create a sub-folder named **<target-os>** and add **<target_os>.mk** into it.
* In **<target-os>.mk**, add commands to build the **<target-os>** and generate the **lib<target-os>.a**, such as modules and definitions. FreeRTOS.mk can be used as a template.
