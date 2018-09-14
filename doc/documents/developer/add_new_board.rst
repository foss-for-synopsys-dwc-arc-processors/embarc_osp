.. _add_new_board:

Adding a New Board
##################

This section describes the process to add a new board named ``myboard`` in embARC OSP.

1. Create a sub-folder under ``board`` folder and named ``myboard``. Create two
new files ``myboard.mk`` and ``myboard.h`` in this folder.

2. Create necessary sub-folders and files under ``myboard``. The BSP for ``myboard`` is made up of these sub-folders and files. There are 3 folders required, ``common``, ``config`` and ``drivers``.

	* ``common``: contains the common routines for ``myboard``. The ``board_init`` function and ``board_timer_xxx`` functions should also be implemented in this folder.

	* ``configs``: contains board configuration information, including

	        - ``target_memory.h`` file containing memory information
	        - ``*.tcf`` file containing information about the arc configuration, compiler options, etc.
	        - ``myboard_hardware.h`` file containing necessary hardware definitions, e.g., peripheral address, interrupt number assignments, etc.
	        - ``*.mk`` files, to be included in ``myboard.mk`` with required build system information.
	        - any other required files

	* ``drivers``: contains on-board driver implementations for ``myboard`` board

3. Modify ``myboard.mk``.  Add necessary information in
   ``virtual.mk``, such as source folders, include folders, object output
   folders and make rules. The build system will automatically compile all the
   source files (.c, .s) in the added folders and finally create a library named
   ``libmyboard.a``.

4. Modify the ``myboard.h`` and add it into ``board.h``. ``myboard.h`` is the
   only place where board specific information should be exposed, e.g., function
   declaration, hardware definitions, macros.

.. note::
   For details, please refer to other existing board support folders such as ``<embARC>/board/emsk`` for reference.
