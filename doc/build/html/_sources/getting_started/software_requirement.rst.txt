.. _software_requirement:

Software Requirement
====================

Development Environment
#######################
* Windows 10 / Windows 7
* Ubuntu 16.04

.. note:: IF you need assistance with a different host environment, please file a GitHub Issue to get support from the embARC development team.

Software Requirement
####################
* `ARC Development Tools <https://www.synopsys.com/designware-ip/processor-solutions/arc-processors/arc-development-tools.html>`_
    Choose **MetaWare Toolkit** and/or **ARC GNU Toolchain** from the following list according to your requirement:

  * MetaWare Toolkit

    * `Premium MetaWare Development Toolkit (2018.12) <https://www.synopsys.com/dw/ipdir.php?ds=sw_metaware>`_
      The DesignWare ARC MetaWare Development Toolkit builds upon a long legacy of industry-leading compiler and debugger products. It is a complete solution that contains all the components needed to support the development, debugging and tuning of embedded applications for the DesignWare ARC processors.
    * `DesignWare ARC MetaWare Toolkit Lite (2018.12) <https://www.synopsys.com/dw/ipdir.php?ds=sw_metaware>`_
      A demonstration/evaluation version of the MetaWare Development Toolkit is available for free from the Synopsys website. MetaWare Lite is a functioning demonstration of the MetaWare Development Toolkit, but has a number of restrictions, including a code-size limit of 32 Kilobytes and no runtime library sources. It is available for Microsoft Windows only.

  * ARC GNU Toolchain

    * `Open Source ARC GNU IDE (2018.09) <https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases>`_
      The ARC GNU Toolchain offers all of the benefits of open source tools, including complete source code and a large install base. The ARC GNU IDE Installer consists of Eclipse IDE with `ARC GNU plugin for Eclipse <https://github.com/foss-for-synopsys-dwc-arc-processors/arc_gnu_eclipse/releases>`_, `ARC GNU prebuilt toolchain <https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases>`_ and `OpenOCD for ARC <https://github.com/foss-for-synopsys-dwc-arc-processors/openocd>`_

* `Digilent Adept Software <https://store.digilentinc.com/digilent-adept-2-download-only/>`_ for Digilent JTAG-USB cable driver
* `Tera Term <https://ttssh2.osdn.jp/>`_ or `PuTTY <https://www.putty.org/>`_ for serial terminal connection, 115200 baud, 8 bits data, 1 stop bit and no parity (115200-8-N-1) by default.

.. note:: If using embARC OSP with the ARC GNU toolchain on Windows, install `Zadig <https://zadig.akeo.ie>`_ to replace FTDI driver with WinUSB driver. See `How to Use OpenOCD on Windows <https://github.com/foss-for-synopsys-dwc-arc-processors/arc_gnu_eclipse/wiki/How-to-Use-OpenOCD-on-Windows>`_ for more information.

Check the following items and set your development environment

* Make sure the paths of the above required tools for the MetaWare toolkit and ARC GNU toolchain are added to the system variable **PATH** in your environment variables.

* We recommend users install the ARC GNU IDE in the default location. If installing to a different location, you will need to make additional changes as described below.

  * If running and debugging embARC applications using **arc-elf32-gdb** and `OpenOCD for ARC <https://github.com/foss-for-synopsys-dwc-arc-processors/openocd>`_, make sure 1) the path of **OpenOCD** is added to the **PATH** in your environment variables, and 2) modify **OPENOCD_SCRIPT_ROOT variable** in *<embARC>/options/toolchain/toolchain_gnu.mk* according to your **OpenOCD** root path.
  * If running GNU program with using the GNU toolchain on Linux, modify the **OpenOCD** configuration file as Linux format with LF line terminators. **dos2unix** can be used to convert it.

.. note:: Check the version of your toolchain. The embARC OSP build system is purely makefile-based. make/gmake is provided in the MetaWare toolkit (gmake) and ARC GNU toolchain (make).

* Download extra third-party source code and apply embARC OSP patches.

    * Install required tools

      * Linux: **bash**, **git**, **dos2unix**
      * Windowns: **Cygwin**/**MSYS2**, **git**, **dos2unix**

    * Set up network environment

        * Connection to `GitHub <https://github.com>`_ and `Eclipse Git repositories <https://git.eclipse.org/c/>`_ to clone source code from them

    * Run scripts to apply patches

        * Make sure the *<embARC>/apply_embARC_patch.sh* file is Unix format. If not, use **dos2unix** to convert it first.

      Apply patches with Unix format error

      * Run ``bash <embARC>/apply_embARC_patch.sh`` in the shell environment to download required source code and patch them.

        * You can also run the separate patch script to patch the required source code, e.g., *<embARC>/middleware/mbedtls/apply_embARC_patch.sh*.

.. note:: <embARC> represents the location where you put the embARC source code in the example usage guide, e.g., *C:/embARC*.
