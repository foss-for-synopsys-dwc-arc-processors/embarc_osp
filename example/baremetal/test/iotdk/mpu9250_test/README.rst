.. _example_mpu9250_test:

MPU9250_Test
######

Overview
********

 This example is designed to test MPU9250 sensor in iotdk board.

Detailed Description
====================
 * Extra Required Tools
    NO

 * Extra Required Peripherals
    NO

 * Design Concept
    This example is designed to test MPU9250 sensor in iotdk board.

 * Usage Manual
    Outputs accelerometer, gyroscope, magnetometer raw data get from MPU9250 to the console.

 * Extra Comments
    NO

Buidling and Running
********************

This example outputs to the console. It is supported by iotdk board. The commands
to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/test/mpu9250_test
    $ make BOARD=iotdk TOOLCHAIN=gnu run

.. note:: Make sure you have selected the correct configuration of iotdk

Sample Output
=============

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

    embARC Build Time: Sep 12 2018, 15:05:22
    Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)



    accel   x= -200,   y=  248,   z=16040
    gpro    x=   -9,   y=   -5,   z=  -13
    mag     x=   37,   y=   12,   z=  -26
