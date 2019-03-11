.. _getting_started:

Getting Started
===============

Use this guide to get started with your :ref:`embARC OSP <introduction_embarc_osp>`
development.

Checking Out the embARC OSP source code
#######################################

The embARC OSP source code is hosted in a GitHub repository that supports
cloning via git. There are scripts and such in this repo that you'll need to set up your development environment, and we'll be using Git to get this repo. If you don't have Git installed, please refer to the beginning of the host OS-specific getting started instructions below for help.

We'll begin by using Git to clone the repository anonymously.

.. code-block:: console

   # On Windows
   cd %userprofile%
   # On Linux
   cd ~

   git clone https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp.git embarc_osp

You have successfully checked out a copy of the source code to your local
machine.

If you are not familiar with git or git doesn't work, you can also download the embARC OSP code
through http. The latest release of embARC OSP is `here <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp/releases>`__.

Setting Up the Development Environment
######################################

The embARC OSP platform supports, but not limited to, the following development machine operating systems:

* Microsoft Windows 10 / Windows 7
* Ubuntu 16.04

Use the following procedure to create a new development environment

.. toctree::
   :maxdepth: 1

   software_requirement.rst

.. note:: The embARC OSP uses make/gmake as a build tool and these are provided as part of the ARC development tools.

.. _building_a_sample_application:

Building a Sample Application
#############################

Using the ``blinky`` example, the following steps, which contains the commands
used for the ARC GNU toolchain in a Windows environment, will describe how
to build an embARC OSP application. Please use the
appropriate commands for your OS.

1. Make sure the ARC GNU toolchain is installed in your environment, see :ref:`software_requirement`.

2. Navigate to :ref:`example_blinky` example located at embARC OSP directory.

.. code-block:: console

   cd embarc_osp\example\baremetal\blinky

3. Build the :ref:`example_blinky` example for the EMSK 2.3 board and ARC EM11D.

.. code-block:: console

   make TOOLCHAIN=gnu BOARD=emsk BD_VER=23 CUR_CORE=arcem11d

.. note:: **make** for ARC GNU toolchain, **gmake** for MetaWare toolkit. For more details of commands, see :ref:`makefile_manual`

Hardware Preparation
####################

Use the following procedures to prepare your ARC board to run/debug the embARC applications.

* :ref:`Getting started with EM starter kit <getting_started_with_em_starter_kit>`
* :ref:`Getting started with HS development kit <getting_started_with_hs_development_kit>`
* :ref:`Getting started with IoT Development kit <getting_started_with_iot_development_kit>`
* :ref:`Getting started with ARC EM Software Development Platform <getting_started_with_arc_em_software_development_platform>`

See the following procedures to check peripherals and board settings. The peripherals are not required for all the embARC OSP examples.

.. toctree::
   :maxdepth: 1

   hardware_requirement.rst
   peripheral_preparation.rst

Running a Sample Application
############################

Follow these steps to run the sample application.

#. Make sure the *blinky* example has been built successfully in :ref:`building_a_sample_application`. Get the .elf file ``blinky_gnu_arcem11d.elf`` in ``<blinky>\obj_emsk_23\gnu_arcem11d``.

#. Follow to steps in :ref:`getting_started_with_em_starter_kit` to set and connect EMSK 2.3 with EM11D to your local machine.

#. Navigate to the embARC OSP directory and run :ref:`example_blinky` example.

.. code-block:: console

   cd embarc_osp\example\baremetal\blinky
   make TOOLCHAIN=gnu BOARD=emsk BD_VER=23 CUR_CORE=arcem11d run

.. note:: No peripheral modules are required to run the ``blinky`` example. If there are LEDs on boards, these LED will be used to blink; If there are no LEDs on boards, a blink message will be output in the UART.

See the following manual for information on make/gmake command and embARC makefile system.

.. toctree::
   :maxdepth: 1

   makefile_manual.rst


Advanced Topics
###############

For advance topics related to application development, please refer :ref:`application_development`.
