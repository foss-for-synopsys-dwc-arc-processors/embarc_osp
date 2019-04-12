.. _example:

Examples
========

Overview
########

The embARC OSP platform comes with a large collection of examples using OS,
middleware and peripherals supported in embARC OSP. These examples provide a
useful  reference and starting point for embARC developers for their own
applications. The .elf files of the following examples can be loaded through
JTAG, or the .bin  files through SD card by secondary bootloader.

.. toctree::
   :maxdepth: 1

   example/example.rst

.. _examples_in_embarc_osp:
.. table:: Examples in embARC OSP
   :widths: auto

   =======================================================  =============================================
   Example Directory                                        Example Document Link
   =======================================================  =============================================
   example/baremetal/arc_feature/cache                      :ref:`example_arc_feature_cache`
   example/baremetal/arc_feature/timer_interrupt            :ref:`example_arc_feature_timer_interrupt`
   example/baremetal/arc_feature/udma                       :ref:`example_arc_feature_udma`
   example/baremetal/ble_hm1x                               :ref:`example_ble_hm1x`
   example/baremetal/blinky                                 :ref:`example_blinky`
   example/baremetal/bootloader                             :ref:`example_bootloader`
   example/baremetal/cxx                                    :ref:`example_cxx`
   example/baremetal/dma_spiflash                           :ref:`example_dma_spiflash`
   example/baremetal/imu_mpu9250                            :ref:`example_mpu9250_test`
   example/baremetal/graphic_u8glib                         :ref:`example_graphic_u8glib`
   example/baremetal/openthread/cli                         :ref:`example_openthread_cli`
   example/baremetal/openthread/ncp                         :ref:`example_openthread_ncp`
   example/freertos/esp8266_wifi                            :ref:`example_esp8266_wifi`
   example/freertos/iot/coap/coap_server                    :ref:`example_iot_coap_coap_server`
   example/freertos/iot/lwm2m/lwm2m_client                  :ref:`example_iot_lwm2m_lwm2m_client`
   example/freertos/iot/lwm2m/lwm2m_server                  :ref:`example_iot_lwm2m_lwm2m_server`
   example/freertos/kernel                                  :ref:`example_kernel`
   example/freertos/kernel_secure                           :ref:`example_kernel_secure`
   example/freertos/net/httpserver                          :ref:`example_net_httpserver`
   example/freertos/net/ntshell                             :ref:`example_net_ntshell`
   example/baremetal/secureshield/secret_normal             :ref:`example_secureshield_secret_normal`
   example/baremetal/secureshield/secret_secure             :ref:`example_secureshield_secret_secure`
   example/baremetal/secureshield/secret_secure_sid         :ref:`example_secureshield_secret_secure_sid`
   example/baremetal/secureshield/test_case                 :ref:`example_secureshield_test_case`
   example/freertos/sec/mbedtls/dtls/client                 :ref:`example_sec_mbedtls_dtls_client`
   example/freertos/sec/mbedtls/dtls/server                 :ref:`example_sec_mbedtls_dtls_server`
   example/freertos/sec/mbedtls/ssl/client2                 :ref:`example_sec_mbedtls_ssl_client2`
   example/freertos/sec/mbedtls/ssl/server2                 :ref:`example_sec_mbedtls_ssl_server2`
   =======================================================  =============================================

The repository `embARC Applications <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_applications>`_ contains various applications based
on `embARC OSP <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp>`_, developed by the embARC team and community.

* `AWS IoT Smarthome Application <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_applications/tree/master/aws_iot_smarthome>`_
* `AWS IoT Smarthome Multi-node Application <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_applications/tree/master/aws_iot_smarthome_multinode>`_
* `iBaby Smarthome Application <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_applications/tree/master/ibaby_smarthome_multinode>`_
* `iLight SmartDevice Application <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_applications/tree/master/ilight_smartdevice>`_
* `OpenThread Smarthome Application <https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_applications/tree/master/ot_smarthome_multinode>`_

Application Note
################
The following application notes provide step-by-step instructions on a broad range of topics to familiarize you with the use of embARC OSP.

* `Quick start guide <https://embarc.org/pdf/20150710_embARC_application_note_quick_start_guide.pdf>`_
* `Adding a new NT-Shell command <https://embarc.org/pdf/20150710_embARC_application_note_ntshell_newcmd.pdf>`_
* `How to create your own peripheral modules for use with the ARC EMSK and embARC <https://embarc.org/pdf/20151013_embARC_application_note_create_module_EMSK_embARC_v3.pdf>`_
* `Timer and ISR <https://embarc.org/pdf/20150805_embARC_application_note_timer_and_isr.pdf>`_
* `Using a secondary bootloader on the EMSK <https://embarc.org/pdf/20150710_embARC_application_note_secondary_bootloader.pdf>`_
* `Using a smart home IoT application with EMSK <https://embarc.org/pdf/embARC_appnote_how_to_use_smart_home_iot.pdf>`_

.. note:: Confirm that the version of embARC OSP and board is the correct one as noted in the application note documents
