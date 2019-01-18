.. _bsp:

embARC BSP
==========

Overview
########

The embARC **Board support Package** (BSP) is a software distributions aimed at facilitating the development and evaluation of embedded systems based on
ARCv2 processors.

Unlike the `embARC OSP <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp>`_ which is a full-featured distribution for ARC embedded systems,
the embARC BSP is designed to provide a minimalistic board support package for ARC users by defining consistent and simple software interfaces to the processors
and onboard devices.

The embARC BSP code is originated from embARC OSP. The code are reorganized with the same ARC HAL, device HAL and driver implementations for DesignWare IP
peripherals. The operating systems, common libraries, middleware components and applications are not included in the embARC BSP. The code in embARC BSP is
independent from the build system. It can be ported to various build system easily. The lightweight makefile and Eclipse IDE are both supported in embARC BSP
as reference.

- Support MetaWare & ARC GNU toolchains
- Support EMSK 2.3 and TCF file
- Support various build systems and compiling environments, such as simple makefile, ARC MetaWare & GNU IDE, makefile, IDE support
- Use embARC ARC Hardware Abstraction Layer (HAL), C & C++ support, assembly support
- Simple UART and GPIO, I2C, SPI drivers
- No middleware, no OS, no uDMA
- Hello & blinky example (UART, GPIO, timer)

.. image:: /pic/images/bsp/bsp_files.jpg
    :alt: embARC BSP Files

.. note:: The current version of BSP code inherits from embARC OSP 2017.04. Only :ref:`board_emsk` is supported in the embARC BSP.

User Guide
##########

Follow these steps to run *hello world* in the embARC BSP.

1. See :ref:`getting_started` to set up the development environment and prepare hardware.

2. Checking out the embARC BSP code. The embARC BSP source code is hosted in a github repository that supports cloning via git. Use the following scripts
to get this repository. And check out a copy of the source code to your local machine. You can download the zip file to get the code in the github as
alternative.

  .. code-block:: console

     # On Windows
     cd %userprofile%
     # On Linux
     cd ~
     # embARC BSP for EMSK
     git clone https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_emsk_bsp.git embarc_bsp

3. The embARC BSP code is independent with build systems. Various build systems can support embARC BSP code. Using the *hello world* as a base model,
the following sections will describe how to build a embARC BSP application.

  .. toctree::
     :maxdepth: 1

     bsp_build_cmd.rst
