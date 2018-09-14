.. _bsp_build_cmd:

Building the BSP from the Command Line
======================================

Build Example for EMSK
######################

#. Generate board configuration files and move them to the application folder. Use following commands for the EMSK 2.3 board and ARC EM7D as an example.

   .. code-block:: console

      # EMSK 2.3, ARC EM7D on Windows
      cd embarc_bsp\board\emsk\configs\23
      # EMSK 2.3, ARC EM7D on Linux
      cd embarc_bsp/board/emsk/configs/23
      # MetaWare
      tcftool arcem7d.tcf -q -x C_defines,arc_core_config.h -x ccac.arg
      # ARC GNU
      tcftool arcem7d.tcf -q -x C_defines,arc_core_config.h -x gcc.arg

#. Move the linker script and copy *arc_core_config.h* and *.arg* to the application folder.

   .. code-block:: console

      # MetaWare on Windows
      mv arc_core_config.h ccac.arg ../../../../example/hello/mwdt
      cp ld\linker_mw.ld ../../../../example/hello/mwdt
      cd ..\..\..\..\example\hello\mwdt
      mv linker_mw.ld arc_core.ld
      # ARC GNU on Windows
      mv arc_core_config.h gcc.arg ../../../../example/hello/arcgnu
      cp ld\linker_gnu.ld ../../../../example/hello/arcgnu
      cd ..\..\..\..\example\hello\arcgnu
      mv linker_gnu.ld arc_core.ld
      # MetaWare on Linux
      mv arc_core_config.h ccac.arg ../../../../example/hello/mwdt
      cp ld/linker_mw.ld ../../../../example/hello/mwdt
      cd ../../../../example/hello/mwdt
      mv linker_mw.ld arc_core.ld
      # ARC GNU on Linux
      mv arc_core_config.h gcc.arg ../../../../example/hello/arcgnu
      cp ld/linker_gnu.ld ../../../../example/hello/arcgnu
      cd ../../../../example/hello/arcgnu
      mv linker_gnu.ld arc_core.ld

#. Build the *hello world* example for the EMSK 2.3 board and ARC EM7D.

   .. code-block:: console

      # MetaWare
      gmake
      # ARC GNU
      make

.. note:: **make** for ARC GNU toolchain, **gmake** for MetaWare toolkit.
