.. _example_mpu9250_test:

MPU9250 Test
############

Overview
********

 This example is designed to test MPU9250 sensor in iotdk board with MPU9250 internal DMP module.

Detailed Description
====================
 * Extra Required Tools
    NO

 * Extra Required Peripherals
    NO

 * Design Concept
    This example is designed to test MPU9250 sensor in iotdk board with MPU9250 internal DMP module.

 * Usage Manual
    Outputs pitch, roll, yaw get from MPU9250 DMP to the console.

 * Extra Comments
    NO

Buidling and Running
********************

This example outputs to the console. It is supported by iotdk board. The commands
to run this example are as follows:

.. code-block:: console

    $ cd <embarc_root>/example/baremetal/imu_9250
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

    embARC Build Time: Sep 12 2018, 14:26:39
    Compiler Version: Metaware, 4.2.1 Compatible Clang 4.0.1 (branches/release_40)
    mpu_set_sensor complete ......
    mpu_configure_fifo complete ......
    mpu_set_sample_rate complete ......
    dmp_load_motion_driver_firmware complete ......
    dmp_set_orientation complete ......
    dmp_enable_feature complete ......
    dmp_set_fifo_rate complete ......
    mpu_set_dmp_state complete ......



    dmp:  pitch=0000.1,  roll=0000.3,  yaw=-145.2
