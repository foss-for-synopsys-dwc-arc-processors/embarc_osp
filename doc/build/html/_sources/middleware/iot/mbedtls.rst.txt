.. _middleware_mbedtls:

mbed TLS
########

Introduction
============

mbed TLS (formerly known as PolarSSL) makes it trivially easy for developers
to include cryptographic and SSL/TLS capabilities in their (embedded)
products, facilitating this functionality with a minimal coding footprint.

mbed TLS is available as open source under the Apache 2.0 license, the GPL 2.0
license, or under an mbed partnership. The Apache 2.0 license enables you to
use mbed TLS in both open source and closed source projects.

embARC supports mbed TLS in FreeRTOS.

Using mbed TLS
==============

Due to export control requirements for United States companies, mbed TLS is
not distributed as part of embARC. These packages have been ported and
verified for use with embARC. The embARC OSP distribution contains required
patches to integrate mbed TLS into embARC.

Steps:

- Make sure ``<embARC>/middleware/mbedtls/apply_embARC_patch.sh`` is Unix format.
  If not, use dos2unix to convert it first.

- Run ``bash <embARC>/middleware/mbedtls/apply_embARC_patch.sh`` in shell
  environment (Linux or Cygwin) to download required source code and patch
  them. The output is as follow:

  .. code-block:: console

     $ cd <embARC>/middleware/mbedtls
     $ bash ./apply_embarc_patch.sh
       Clone repo from https://github.com/ARMmbed/mbedtls.git at commit or tag mbedtls-2.4.1
       Branch master set up to track remote branch coverity_scan from origin.
       Already on 'master'
       commit b2c6b254430bd97aced22a4946bec1a51ccd5173
       Author: Janos Follath <janos.follath@arm.com>
       Date:   Tue Dec 13 14:14:03 2016 +0000

       Update library version number to 2.4.1
       Switched to a new branch 'embARC'
       Try to apply patch for this repo
       Applying: Add embARC (FreeRTOS+LwIP) support to mbedtls
       .git/rebase-apply/patch:433: space before tab in indent.
                (void) mbedtls_timing_get_timer( &ctx->timer, 1 );
       .git/rebase-apply/patch:448: space before tab in indent.
                return( 2 );
       .git/rebase-apply/patch:451: space before tab in indent.
                return( 1 );
       .git/rebase-apply/patch:458: new blank line at EOF.
       +
       warning: 4 lines add whitespace errors.
       Patches are applied
