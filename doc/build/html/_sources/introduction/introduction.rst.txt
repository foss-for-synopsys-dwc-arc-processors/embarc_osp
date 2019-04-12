.. _introduction_embarc_osp:

Overview
========

Introduction
############

The embARC Open Software Platform (OSP) is an open software platform to
facilitate the development of embedded systems based on |arc|.

It is designed to provide a unified platform for |arc| users by defining
consistent and simple software interfaces to the processor and peripherals,
together with ports of several well known FOSS embedded software stacks to
|arc|.

`embARC OSP <https://embarc.org>`__ provides a starting foundation for
embedded developers, simplifying development and improving time-to-market
through software reuse.

Purpose and Scope
#################

embARC OSP is an open software platform, providing useful components with a
consistent software layering model and interfaces to |arc| for portability and
reusability. Developers can write software based on embARC OSP to accelerate
implementation and reduce their learning curve and the time-to-market of final
product implementation.

The goals of embARC are as follows:

- Provide an easy introduction for new users of |arc|.

- Provide useful software stacks as a starting point for building embedded
  products based on |arc| and well-known open-source software packages.

- Provide broad ranges of examples to demonstrate |arc| usage in various
  embedded IoT applications.

- Provide simple, frictionless  **out-of-the box** user experience through:

  * Single archive download containing all embARC packages and examples.
  * Comprehensive pre-integrated tools support.
  * Clear, simple, and extensive **how-to** instructions.



Architecture
############

The embARC Open Software Platform general architecture and components are shown
in the block diagram below.

.. image:: /pic/embARC.jpg
    :alt: embARC

.. note::
   Dotted line boxes indicate supported packages being excluded from the embARC OSP
   distribution. See the related embARC documentation sections for information on
   how to obtain these packages for use with embARC

   - For secure communication, see :ref:`mbed TLS<middleware_mbedtls>`

- :ref:`ARC Hardware Abstraction Layer (ARC HAL)<arc_hal>` provides the
  necessary hardware abstraction for |arc| ISA compatible processors.

- :ref:`Device Hardware Abstraction Layer (HAL)<device_hal>` provides hardware
  abstraction for common platform devices and implementation of DesignWare IP
  drivers, such as UART, IIC, and SPI.

- :ref:`Board Support Package (BSP) Layer<board_bsp>` provides support for
  different boards based on the ARCv2 processors.

- :ref:`OS Layer<os_layer>` provides embedded operating system support,
  including options for traditional Real-Time Operating Systems (RTOSs).

- :ref:`Library Layer<library_layer>` provides basic common library support
  for embedded systems, such as C library, dlmalloc library and SecureShield
  library.

- :ref:`Middleware Layer<middleware_layer>` includes mainstream embedded
  software stack solutions, including FAT file system, network stack, simple
  shell/Command Line Interface (CLI), IoT protocols, lightweight graphics, IoT
  cloud connectivity and more.

Many :ref:`examples<example>`, covering IoT, network, and sensor application cases, are also
provided to help get started quickly with |arc| and the embARC OSP software.


Toolchain Support
#################

The embARC OSP software is supported by the following toolchain offerings from Synopsys:

- The `DesignWare ARC MetaWare Development Toolkit <https://www.synopsys.com/dw/ipdir.php?ds=sw_metaware>`__
  builds on a long legacy of industry-leading compiler and debugger products for embedded applications.
  It is a complete solution that contains all the components needed to support the
  development, debugging, and tuning of embedded applications for the |arc|.

- The `GNU Toolchain for DesignWare ARC Processors <https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases>`__
  provides a pre-built GNU toolchain for |arc| integrated into the Eclipse IDE.
  The GNU tools are maintained by Synopsys with new releases being published at
  regular intervals.

- The `DesignWare ARC MetaWare Development Toolkit Lite Edition <https://www.synopsys.com/cgi-bin/arcmwtk_lite/reg1.cgi>`__
  is a functioning demonstration of the MetaWare Development Toolkit, but has a
  number of restrictions, including a code-size limit of 32 Kb and no run-time
  library sources. It is available for free download and use for academic, personal,
  and noncompetitive evaluation purposes.

Copyright Notice
################

Copyright (c) |today|, Synopsys, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the Synopsys, Inc., nor the names of its contributors
   may be used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. NY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**For complete embARC license information, please refer to the embARC FOSS Notice**.

Fundamental Terms and Concepts
##############################

See :ref:`glossary`
