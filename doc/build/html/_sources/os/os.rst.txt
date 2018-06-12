.. _os_layer:

Operating Systems
=================

Overview
########


embARC OSP provides a choice of well-known and popular embedded OSes with using
|arc| in embedded applications, especially Internet of
Things (IoT).

Two OSs are currently supported in embARC: `FreeRTOS`_, and
`DesignWare ARC MQX RTOS`_.



FreeRTOS
########

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

.. _os_mqx:

DesignWare ARC MQX RTOS
#######################

The `DesignWare ARC MQX RTOS`_ solution offers a highly optimized application
development platform designed for speed and size efficiency, with optimized
support for all DesignWare ARC processors. embARC middleware and drivers are
ported and verified on MQX and available to MQX licensees upon request.

The ARC MQX RTOS solution includes full source code and offers many
comprehensive features that support reliability, deterministic performance,
reconfiguration, and multiprocessing. The scalable design allows MQX users to
pick the components and features they require from MQX and prevents unused
features from being included in the RTOS memory footprint. With code size as
low as 3.5 KB, the MQX RTOS solution is small enough to replace a home-grown
scheduler while still providing a much more comprehensive feature set.

With MQX's function-level scalability, API functions outside of the central
core are only included in the linked image if they are explicitly called by
the application. This, along with dozens of compile-time configuration options
(also grouped into sets), gives application developers maximum control over
the memory footprint of MQX.

.. _FreeRTOS: http://freertos.org
.. _DesignWare ARC MQX RTOS: http://www.synopsys.com/dw/ipdir.php?ds=os_mqx_software