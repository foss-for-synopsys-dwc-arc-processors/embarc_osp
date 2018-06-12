.. _add_new_board:

Add New Board
#############

Here, we will add a new board named ``virtual`` to show the steps to add a new
board in embARC OSP.

1. Create a sub folder under ``board`` and name it as ``virtual``. Create two
new files ``virtual.mk`` and ``virtual.h`` in this folder.

2. Create necessary sub-folders and files under ``virtual``. The ``virtual`` BSP is consisted of these sub-folers and files. Here, there are 3 folders, ``common``, ``config`` and ``drivers``.

	* ``common``: the common routines of ``virtual`` are in this folder. ``board_init`` function and ``board_timer_xxx`` functions are suggested to be here.

	* ``configs``: the board config inforamtion is in this folder, including

	        - memory information in ``target_memory.h``
	        - ``*.tcf`` file has information for arc configuration, compiler option etc..
	        - ``virtual_hardware.h`` has necessary hardware definitions, e.g., perpheral address, interrupt number assignment.
	        - ``*.mk`` files, which should be included in ``virtual.mk`` are necessary build system information.
	        - other necessary files

	* ``drivers``: the on-board drivers implementation of ``virtual`` board

3. Modify the ``virtual.mk``. The necessary information should be added in
   ``virtual.mk``, such as source folders, including folders, object output
   folders and make rules. The build system will automatically compile all the
   source files (.c, .s) in the added folders and create library named
   ``libvirtual.a`` finally.

4. Modify the ``virtual.h`` and add it into ``board.h``. ``virtual.h`` is the
   only place to expose board sepcific informaton, e.g., function
   declarataion, hardware definitions, macros.

.. note::
   For details, please take other exist board support folder like ``<embARC>/board/emss`` for reference.