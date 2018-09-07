.. _usage_guide_faq:

Usage Guide FAQ
===============

* **Here are some tips when running some examples unsuccessfully.**

	* Check the board version, core configuration and toolchain version.
	* Check the WiFi hotspot name and password settings in *board/board.h*.
	* Make sure that peripherals are connected to the right ports, such as Pmod WiFi, Pmod TMP2, Pmod RF2, and SD card.
	* The MetaWare toolchain works better than the ARC GNU toolchain.
	* Check if your PC or laptop is connected to the same WiFi hotspot as EMSK.
	* If you are running a Telnet commands on your PC to test embARC examples, shut down the Telnet process in the Task Manager.
	* If **WF_EVENT_CONNECTION_FAILED** is printed in the terminal when embARC startup, wait at least two minutes for reconnection. The EMSK can reconnect to the WiFi hotspot if your WiFi hotspot setting is right.
	* Using the make/gmake in the MetaWare and ARC GNU toolchain is highly recommended in the embARC OSP.
	* Make sure the source code file is not in UTF-16 format. If the UTF-16 format is used, compiling may fail with the error message **cc1.exe: out of memory allocating 838860800 bytes**.
	* When building and downloading a program with the MetaWare toolchain, it is recommended to disconnect PC/EMSK from the WiFi hotspot. Connecting to the WiFi hotspot may cause a compile failure when checking the license of the compiler. It depends on your network environment.

* **Here are some tips when you are using the ARC GNU toolchain.**

	* Install EMSK Digilent JTAG drivers on Windows for the ARC GNU toolchain. See `How to Use OpenOCD on Windows <https://github.com/foss-for-synopsys-dwc-arc-processors/arc_gnu_eclipse/wiki/How-to-Use-OpenOCD-on-Windows>`_ for more information.
	* Installing the Digilent JTAG drivers for the ARC GNU toolchain affects the usage of MetaWare debugger. If you want to switch to using the MetaWare debugger, uninstall the Digilent JTAG drivers for the ARC GNU toolchain. Then install the drivers provided by Digilent.
	* Different board has different driver name. There are two drivers for the onboard FTDI chip, one for JTAG and the other for USB. Choose and update the JTAG driver only.
	* System reboot might be needed.

* **Why cannot build the embARC after changing the version of embARC OSP, or pulling source code from Github?**

	* Check the embARC OSP version and ARC toolchain version carefully in :ref:`software_requirement`. The embARC OSP doesn't check the toolchain version compatibility.

* **Why cannot run an example with minor changes in the source code/configuration again?**

	* The generated files are not cleaned automatically when changing some code or configurations.
	* If you can run any of the examples provided in the first time. But for the second time, no response is sent back. Try to clean the project with `make clean`, set the board configuration and build the example again.

* **Here are some tips when you are using WiFi in the embARC OSP?**

	* Make sure your Pmod WiFi connects to the right Pmod connector, J5. Then Check the hotspot settings in <board_name>/<board_name>.h, e.g., emsk/emsk.h.
	* Make sure your WiFi setting is correct.

		* **WF_HOTSPOT_IS_OPEN**

			* 1: WiFi hotspot is non-encrypted
			* 0: WiFi hotspot is encrypted, you must set the right WF_HOTSPOT_NAME and WF_HOTSPOT_PASSWD

		* **WF_HOTSPOT_NAME**: the SSID name of your WiFi hotspot
		* **WF_HOTSPOT_PASSWD**: the password of your WIFI hotspot
		* If your WIFI hotspot is encrypted, you must set it to WPA/WPA2 PSK encrypted.

	* Running the WiFi examples on EMSK is recommended in the current version.

* **Why are the IP addresses different when running in bare metal and RTOS?**

	* Running examples under an RTOS (FreeRTOS) enables DHCP to get the IP address from the DHCP server. So the IP address under an RTOS is automatically assigned.
	* Set the right IP address, gateway, and netmask in *board/board.h* when using baremetal.

* **How can I check whether the Pmod WiFi is connected to a hotspot?**

	* When the WIFI Pmod is connected to a hotspot, EMSK prints a message on the terminal.

	.. code-block:: console

		WF_EVENT_CONNECTION_SUCCESSFUL.
		When connecting fails, the message is WF_EVENT_CONNECTION_FAILED. The common message for lwIP examples is shown below.
		Now trying to connect to WIFI Hotspot, Please wait about 30s!
		MRF24G Device Information As Follows:
		Device Type:2, ROM Ver:31, Patch Ver:7
		Connection Profile ID:1
		WF INIT SUCCESSFULL!
		PMWIFI_0 MAC ADDRESS:00-1e-c0-0e-71-ac
		WF_EVENT_CONNECTION_FAILED     --> sometimes happens
		WF_EVENT_CONNECTION_SUCCESSFUL
		Link is UP!

	* Sometimes the board may hang when using the WiFi module. The board can prints a message **WF_EVENT_ERROR:XXXXXX** or gives no output. If the board hangs, recommend to power down the board, download and run your program again.

* **How to run multi-node examples in the embARC OSP**

	* For the EMSK boards, a recommended way is generating binary file for your applications, for example, `make TOOLCHAIN=gnu BD_VER=22 CUR_CORE=arcem7d bin`. Rename it to *boot.bin* and copy it to the SDCard. Then use **emsk_bootloader** as a secondary bootloader to load *boot.bin* on the SDCard, where the EMSK SPI Flash is programmed.

* **Why do the make TOOLCHAIN=gnu run or make TOOLCHAIN=gnu gui commands fail on Linux host?**

	* **OpenOCD** on Linux requires the configuration files in a UNIX file format with LF line terminators. Convert the configuration files to UNIX file format.

* **How to set the JTAG frequency to match the target CPU frequency?**

	* Sometimes the CPU frequency and JTAG frequency don't match in the ARC core configurations, e.g., when you add a new ARC board. It is necessary to set the JTAG frequency to match the CPU frequency of the selected core configuration.
	* For the MetaWare toolchain, pass a make option called **DIG_SPEED** (in Hz) to set the JTAG frequency, like ``DIG_SPEED=5000000``. For the EMSK 2.2, the **DIG_SPEED** is hard-coded in *board/emsk/configs/22/emsk_22.mk*.
	* For the ARC GNU toolchain, change the value of adapter_khz(in KHz) in the openocd file of EMSK to set the JTAG frequency.

		* For EMSK 1.x, the openocd config file is located in *<ARC_GNU>/share/openocd/scripts/board/snps_em_sk_v1.cfg*.
		* For EMSK2.x, the openocd config file is located in *<ARC_GNU>/share/openocd/scripts/board/snps_em_sk.cfg*.
		* *<ARC_GNU>* is the installation directory of the ARC GNU toolchain.
