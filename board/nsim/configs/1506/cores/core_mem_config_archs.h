#ifndef _CORE_MEM_CONFIG_ARCHS_H_
#define _CORE_MEM_CONFIG_ARCHS_H_

/**
 * The unit of XXXX_SIZE is Byte
 * For REGION_ROM, ICCM, EXT_ROM and EXT_RAM are available
 * For REGION_RAM, DCCM and EXT_RAM are available
 */

#ifndef ICCM_SIZE
#define ICCM_SIZE	0x80000
#endif

#ifndef DCCM_SIZE
#define DCCM_SIZE	0x80000
#endif

#define REGION_ROM	ICCM
#define REGION_RAM	DCCM

#ifndef ICCM_START
#define ICCM_START	0x0
#endif

#ifndef DCCM_START
#define DCCM_START	0x80000000
#endif

#endif /* _CORE_MEM_CONFIG_ARCHS_H_ */
