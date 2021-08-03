
OpenThread Diagnostics Module Reference
=======================================

The OpenThread diagnostics module is a tool for debugging platform hardware manually, and it will also be used during manufacturing process, to verify platform hardware performance.

The diagnostics module supports common diagnostics features that are listed below, and it also provides a mechanism for expanding platform specific diagnostics features.

Common Diagnostics Command List
-------------------------------


* `diag <#diag>`_
* `diag start <#diag-start>`_
* `diag channel <#diag-channel>`_
* `diag power <#diag-power>`_
* `diag send <#diag-send-packets-length>`_
* `diag repeat <#diag-repeat-delay-length>`_
* `diag sleep <#diag-sleep>`_
* `diag stats <#diag-stats>`_
* `diag stop <#diag-stop>`_

diag
^^^^

Show diagnostics mode status.

.. code-block:: bash

   > diag
   diagnostics mode is disabled

diag start
^^^^^^^^^^

Start diagnostics mode.

.. code-block:: bash

   > diag start
   start diagnostics mode
   status 0x00

diag channel
^^^^^^^^^^^^

Get the IEEE 802.15.4 Channel value for diagnostics module.

.. code-block:: bash

   > diag channel
   channel: 11

diag channel \<channel&gt;
^^^^^^^^^^^^^^^^^^^^^^^^^^

Set the IEEE 802.15.4 Channel value for diagnostics module.

.. code-block:: bash

   > diag channel 11
   set channel to 11
   status 0x00

diag power
^^^^^^^^^^

Get the tx power value(dBm) for diagnostics module.

.. code-block:: bash

   > diag power
   tx power: -10 dBm

diag power \<power&gt;
^^^^^^^^^^^^^^^^^^^^^^

Set the tx power value(dBm) for diagnostics module.

.. code-block:: bash

   > diag power -10
   set tx power to -10 dBm
   status 0x00

diag send \<packets&gt; \<length&gt;
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Transmit a fixed number of packets with fixed length.

.. code-block:: bash

   > diag send 20 100
   sending 0x14 packet(s), length 0x64
   status 0x00

diag repeat \<delay&gt; \<length&gt;
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Transmit packets repeatedly with a fixed interval.

.. code-block:: bash

   > diag repeat 100 100
   sending packets of length 0x64 at the delay of 0x64 ms
   status 0x00

diag repeat stop
^^^^^^^^^^^^^^^^

Stop repeated packet transmission.

.. code-block:: bash

   > diag repeat stop
   repeated packet transmission is stopped
   status 0x00

diag sleep
^^^^^^^^^^

Enter radio sleep mode.

.. code-block:: bash

   > diag sleep
   sleeping now...

diag stats
^^^^^^^^^^

Print statistics during diagnostics mode.

.. code-block:: bash

   > diag stats
   received packets: 10
   sent packets: 10
   first received packet: rssi=-65, lqi=101

diag stop
^^^^^^^^^

Stop diagnostics mode and print statistics.

.. code-block:: bash

   > diag stop
   received packets: 10
   sent packets: 10
   first received packet: rssi=-65, lqi=101

   stop diagnostics mode
   status 0x00
