.. _coding_style:

Coding style
============

The embARC coding style is inspired by the `Linux Kernel Coding Style
<https://github.com/torvalds/linux/blob/master/Documentation/CodingStyle>`_.
Variable naming such as **xxx_yyy_zzz** represents a paradigm of
objective_action_target, for example uart_get_info (UART GET INFORMATION).

The source-code files in embARC are documented with C-style code comments and
Doxygen-based comments. A Doxygen comment example is shown below:

.. code-block:: c

	/* embARC HAL */
	#include "embARC.h"
	#include "embARC_debug.h"

	/** main entry */
	int main(void)
	{

		EMBARC_PRINTF("Hello embARC from template example\r\n");

		return E_SYS;
	}
	/** @} */
