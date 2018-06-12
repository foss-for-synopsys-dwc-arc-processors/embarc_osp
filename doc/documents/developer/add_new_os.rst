.. _add_new_os:

Add New OS
##########

Follow the steps below to add a new OS.

* Include **<target-os>.mk** in **os.mk**. Add corresponding definitions such as OS_ID, OS_CSRDIR, OS_ASMSRCDIR, and OS_INCDIR.
* Add necessary definitions in os_hal_inc.h, such as the header-files path of **<target-os>** and the definition of os_hal_exc_init. The **os_hal_exc_init()** function is provided to initialize the exceptionand interrupt-related features of **<target-os>**.
* Create a sub-folder named **<target-os>** and add **<target_os>.mk** into it.
* In **<target-os>.mk**, add how to build the **<target-os>** and generate the **lib<target-os>.a**, such as modules and definitions. FreeRTOS.mk is a simple template.
* Refer to other embARC supported OSes as reference.