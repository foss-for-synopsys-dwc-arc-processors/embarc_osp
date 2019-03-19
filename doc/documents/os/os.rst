.. _os_layer:

Operating Systems
=================

Overview
########


In embARC OSP, `FreeRTOS`_ is adopted to provide OS kernel service. An optimized
|arc| port is provided in ``os/freertos/portable``.


`FreeRTOS`_ is the market-leading real-time operating system (or RTOS), and the
de-facto standard solution for microcontrollers and small microprocessors.

- Provides a single and independent solution for many different architectures
  and development tools.

- Feature rich and is still undergoing continuous active development.

- Minimal ROM, RAM, and processing overhead. Typical RTOS kernel
  binary image is 6K to 12K bytes.

- Very simple. The core of the RTOS kernel is contained in only three C
  files. The majority of the many files included in the .zip file download
  relate only to the numerous demonstration applications.

- Truly free for use in commercial applications.

- Comes with a porting, platform development, or application development
  service if required.

- Is well established with a large and ever growing user base.

- Provides ample documentation.

- Highly scalable, simple, and easy to use.

.. note:: It's also possible to integrate other RTOS into embARC OSP

.. _FreeRTOS: https://freertos.org
