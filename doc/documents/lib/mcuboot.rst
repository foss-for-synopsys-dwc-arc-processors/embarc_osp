.. _lib_mcuboot:

MCUBoot
#######


Introduction
============

MCUboot is a secure bootloader for 32-bit MCUs. The goal of MCUboot is to define a common infrastructure for the bootloader,
system flash layout on microcontroller systems, and to provide a secure bootloader that enables easy software upgrade.

MCUboot is operating system and hardware independent and relies on hardware porting layers from the operating system it works with.

MCUBoot is hosted on github now.

Using MUCBoot
=============

embARC uses MCUBoot in bootloader to validate the binary image by SHA-256 hash function and asymmetric encryption RSA algorithm for the integrity check
and signature verification, and embARC uses MCUBoot image tools to sign each application binary.

Steps
-----

* Install Python3
* Install several dependencies on Python libraries

  .. code-block:: console

      $ cd <embarc_root>/library/mcuboot
      $ pip3 install --user -r scripts/requirements.txt

Sign your application
---------------------

This signs the image by computing hash over the image, and then signing that hash. Signature is computed by newt tool when it's creating the image.
This signature is placed in the image trailer.

embARC already adds the sign command in compile system, you can use it very conveniently.

.. code-block:: console

    $ cd <embarc_applcation>
    $ make sign

There is a default public keypair used to signature and validation under the folder *<embarc_root>/library/mcuboot*.
If you want to use a new one please reference `manage keys manual`_ to generate and replace the *key.c* and *root-rsa-2048.pem*.

.. note:: By default, embARC configures MCUBoot in the bootloader to use RSA as asymmetric encryption function. So make sure to generate the rsa-2048 keys.

.. _manage keys manual: https://github.com/JuulLabs-OSS/mcuboot/blob/master/docs/imgtool.md
