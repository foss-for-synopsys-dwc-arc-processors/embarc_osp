#ifndef _APPL_MEM_CONFIG_H_
#define _APPL_MEM_CONFIG_H_

#ifdef BOARD_EMSK
#define REGION_EXT_RAM_START	0x17F00000	/* boootloader application start address */
#define REGION_EXT_RAM_SIZE		0x100000	/* the mem space covered by bootloader */
#endif

#endif