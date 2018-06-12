.. _example_cxx:

CPP Build
#########

Overview
********

 This example is designed to test C++ support in embARC.

Detailed Description
====================
 - Extra Required Tools
      NO

 - Extra Required Peripherals
      NO

 - Design Concept
    This example is designed to test C++ support in embARC.

 - Usage Manual
    Here we take EMSK 2.3 EM7D for example, you can run the program using Metaware toolset.

    - Re-configure the EMSK 2.3 board to EM7D configuration.
    - Command: ``gmake BD_VER=23 CUR_CORE=arcem7d run`` .

 - Extra Comments
    NO

Sample Output
=============

You will see the following output in uart console window when the download is successful.

.. code-block:: console

    -----------------------------------------------------------
     ____                                _ ____
    |  _ \ _____      _____ _ __ ___  __| | __ ) _   _
    | |_) / _ \ \ /\ / / _ \ '__/ _ \/ _` |  _ \| | | |
    |  __/ (_) \ V  V /  __/ | |  __/ (_| | |_) | |_| |
    |_|   \___/ \_/\_/ \___|_|  \___|\__,_|____/ \__, |
                                                 |___/
                         _       _    ____   ____
           ___ _ __ ___ | |__   / \  |  _ \ / ___|
          / _ \ '_ ` _ \| '_ \ / _ \ | |_) | |
         |  __/ | | | | | |_) / ___ \|  _ <| |___
          \___|_| |_| |_|_.__/_/   \_\_| \_\\____|
    ------------------------------------------------------------

    embARC Build Time: Mar 16 2018, 09:58:46
    Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1
    a is 4
    i:120
    i is:32896
    someComputing: i=99
    id is 2         x:1
    id is 2         y:1
    id is 3         z:4
    id is 0         w:5
    x and y are the same
