/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */
//#define DEBUG
//#define DEBUG_HOSTLINK
#include <string.h>
#include "secureshield.h"
#include "secureshield_vmpu.h"
#include "secureshield_secure_call_exports.h"
#include "secureshield_container_stack.h"
#include "secureshield_container_call.h"


/* MPU region count */
#ifndef MPU_REGION_COUNT
#define MPU_REGION_COUNT 32
#endif/*MPU_REGION_COUNT*/

/* set default MPU region count */
#ifndef ARC_FEATURE_MPU_REGIONS
#define ARC_FEATURE_MPU_REGIONS 16
#endif/*ARC_FEATURE_MPU_REGIONS*/

/* reserve 2 regions, one region is for the shared rom section (.text, .rodata, ...)
 * one region if for the shared ram section (.text, .rodata, ...)
 */
#define ARC_MPU_RESERVED_REGIONS 2

/* set default minimum region address alignment */
#ifndef ARC_FEATURE_MPU_ALIGNMENT_BITS
#define ARC_FEATURE_MPU_ALIGNMENT_BITS 11
#endif/*ARC_FEATURE_MPU_ALIGNMENT_BITS*/

/* derived region alignment settings */
#define ARC_FEATURE_MPU_ALIGNMENT (1UL<<ARC_FEATURE_MPU_ALIGNMENT_BITS)
#define ARC_FEATURE_MPU_ALIGNMENT_MASK (ARC_FEATURE_MPU_ALIGNMENT-1)


#ifndef CONTAINER_INTERFACE_COUNT
#define CONTAINER_INTERFACE_COUNT	10
#endif

#define AUX_MPU_RDB_VALID_MASK 0x1
#define AUX_MPU_EN_ENABLE   0x40000000
#define AUX_MPU_EN_DISABLE  0x0

#define AUX_MPU_RDP_UE  0x008    /* allow user execution */
#define AUX_MPU_RDP_UW  0x010    /* allow user write */
#define AUX_MPU_RDP_UR  0x020    /* allow user read */
#define AUX_MPU_RDP_KE  0x040    /* only allow kernel execution */
#define AUX_MPU_RDP_KW  0x080    /* only allow kernel write */
#define AUX_MPU_RDP_KR  0x100    /* only allow kernel read */

#define AUX_MPU_RDP_REGION_SIZE(bits)  	(((bits - 1) & 0x3) | (((bits - 1) & 0x1C) << 7))

#define SECURE_CONTAINER_DEFAULT_STATUS	(AUX_STATUS_MASK_IE | (((INT_PRI_MAX - INT_PRI_MIN) << 1) & 0x1e))
#define NORMAL_CONTAINER_DEFAULT_STATUS	(SECURE_CONTAINER_DEFAULT_STATUS | AUX_STATUS_MASK_U)


typedef struct {
	uint32_t base;
	uint32_t end;
	uint32_t rdp;
	uint32_t size;
	uint32_t ac;
} MPU_REGION;

typedef struct {
	MPU_REGION *region;
	uint32_t type;
	uint32_t count;
} MPU_CONTAINER;


typedef struct {
	void * func;
	uint32_t args_num;
} CONTAINER_INTERFACE;


/* structure to describe how a container owns interfaces */
typedef struct {
	CONTAINER_INTERFACE *interface;
	uint32_t count;
} INTERFACE_CONTAINER;

static uint32_t g_mpu_slot;
static uint32_t g_mpu_region_count;
static MPU_REGION g_mpu_list[MPU_REGION_COUNT];
static MPU_CONTAINER g_mpu_container[SECURESHIELD_MAX_CONTAINERS];

static uint32_t g_interface_count;
static CONTAINER_INTERFACE g_interface_list[CONTAINER_INTERFACE_COUNT];
static INTERFACE_CONTAINER g_interface_container[SECURESHIELD_MAX_CONTAINERS];


/**
 * \brief find the MPU_REGION in the specified container
 * \param[in] fault_addr fault address
 * \param[in] container the specified container
 *
 * \return the pointer of MPU_REGION which contains the fault address. NULL if no MPU_REGION found
 */
static const MPU_REGION* vmpu_fault_find_container_region(uint32_t fault_addr, const MPU_CONTAINER *container)
{
	uint32_t count;
	const MPU_REGION *region;

	count = container->count;
	region = container->region;
	while (count-- > 0) {
		if ((fault_addr >= region->base) && (fault_addr < region->end)) {
			return region;
		}
		else {
			region++;
		}
	}

	return NULL;
}

/**
 * \brief find the right mpu region according to the fault address
 * \details search the mpu regions of current container first, then search the background
 *          container
 * \param[in] fault_addr fault address
 * \return the pointer of MPU_REGION which contains the fault address. NULL if no MPU_REGION found
 */
static const MPU_REGION* vmpu_fault_find_region(uint32_t fault_addr)
{
	const MPU_REGION *region;
	uint32_t i;


	/* check fault_addr whether in current container  */
	if ((g_active_container) && ((region = vmpu_fault_find_container_region(
		fault_addr, &g_mpu_container[g_active_container])) != NULL)) {
		return region;
	}

	/* if current container is secure,  check all normal containers */
	if (container_is_secure(g_active_container)) {
		for (i = 0; i < g_vmpu_container_count; i++) {
			if (!container_is_secure(i)) {
				if ((region = vmpu_fault_find_container_region(fault_addr,
					&g_mpu_container[i])) != NULL) {
					return region;
				}
			}
		}
		return NULL;
	} else {
		/* check background container 0 */
		return vmpu_fault_find_container_region(fault_addr, &g_mpu_container[0]);
	}
}

/**
 * \brief get the bits according to size
 * \param[in] size
 * \return bits
 */
static int32_t vmpu_region_bits(uint32_t size)
{
	int32_t bits;

	bits = vmpu_bits(size)-1;

	/* minimum region size is 2048 bytes */
	if (bits < ARC_FEATURE_MPU_ALIGNMENT_BITS) {
		bits = ARC_FEATURE_MPU_ALIGNMENT_BITS;
	}

	/* round up if needed */
	if ((1UL << bits) < size) {
		bits++;
	}

	return bits;
}

/**
 * \brief map the AC to MPU region setting
 * \param[in] ac access control
 * \return MPU region setting
 */
static uint32_t vmpu_map_ac(CONTAINER_AC ac)
{
	uint32_t flags;
	CONTAINER_AC ac_res;

	/* map generic ACL's to internal ACL's */
	if (ac & SECURESHIELD_AC_UWRITE) {
		ac_res = SECURESHIELD_AC_UREAD | SECURESHIELD_AC_UWRITE |
			  SECURESHIELD_AC_KREAD | SECURESHIELD_AC_KWRITE;
		flags = AUX_MPU_RDP_UR | AUX_MPU_RDP_UW |
			AUX_MPU_RDP_KR | AUX_MPU_RDP_KW;
	}
	else {
		if (ac & SECURESHIELD_AC_UREAD) {
			if (ac & SECURESHIELD_AC_KWRITE) {
				ac_res = SECURESHIELD_AC_UREAD |
					 SECURESHIELD_AC_KREAD | SECURESHIELD_AC_KWRITE;
				flags = AUX_MPU_RDP_UR |
					AUX_MPU_RDP_KR | AUX_MPU_RDP_KW;
			} else {
				ac_res = SECURESHIELD_AC_UREAD |
					 SECURESHIELD_AC_KREAD;
				flags = AUX_MPU_RDP_UR |
					AUX_MPU_RDP_KR;
			}
		} else {
			if (ac & SECURESHIELD_AC_KWRITE) {
				ac_res = SECURESHIELD_AC_KREAD | SECURESHIELD_AC_KWRITE;
				flags = AUX_MPU_RDP_KR | AUX_MPU_RDP_KW;
			} else {
				if (ac & SECURESHIELD_AC_KREAD) {
					ac_res = SECURESHIELD_AC_KREAD;
					flags = AUX_MPU_RDP_KR;
				} else {
					ac_res = 0;
					flags = 0;
				}
			}
		}
	}

	if (ac & SECURESHIELD_AC_UEXECUTE) {
		ac_res |= SECURESHIELD_AC_UEXECUTE;
		flags |= AUX_MPU_RDP_UE;
	}

	if (ac & SECURESHIELD_AC_KEXECUTE) {
		ac_res |= SECURESHIELD_AC_KEXECUTE;
		flags |= AUX_MPU_RDP_KE;
	}

	/* check if we meet the expected ACL's */
	if ((ac_res) != (ac & SECURESHIELD_AC_ACCESS)) {
		SECURESHIELD_HALT("inferred ACL's (0x%x) don't match expected ACL's (0x%x)",
		ac_res, (ac & SECURESHIELD_AC_ACCESS));
	}

	return flags;
}

/**
 * \brief update the setting of MPU region in the specified container
 * \param[in] region MPU region
 * \param[in] container_id  the specified container
 * \param[in] base start address
 * \param[in] size region size
 * \param[in] ac  access control of the region
 */
static void vmpu_ac_update_container_region(MPU_REGION *region, uint8_t container_id,
	void* base, uint32_t size, CONTAINER_AC ac)
{
	uint32_t bits, mask, size_rounded;

	SECURESHIELD_DBG("\tcontainer[%d] ac[%d]={0x%x,size=%d,ac=0x%x,",
		container_id, g_mpu_region_count, (uint32_t)base, size, ac);

	/* verify region alignment */
	bits = vmpu_region_bits(size);
	size_rounded = 1UL << bits;
	if (size_rounded != size) {
		if ((ac & (SECURESHIELD_AC_SIZE_ROUND_UP|SECURESHIELD_AC_SIZE_ROUND_DOWN))==0) {
			SECURESHIELD_HALT(
				"container size (%d) not rounded, rounding disabled (rounded=%d)",
				size, size_rounded
			);
			return;
		}

		if (ac & SECURESHIELD_AC_SIZE_ROUND_DOWN) {
			bits--;
			if(bits<ARC_FEATURE_MPU_ALIGNMENT_BITS) {
				SECURESHIELD_HALT("region size (%d) can't be rounded down",size);
				return;
			}
			size_rounded = 1UL << bits;
		}
	}

	/* check for correctly aligned base address */
	mask = size_rounded - 1;
	if (((uint32_t)base) & mask) {
		SECURESHIELD_HALT("base address 0x%x and size (%d) are inconsistent", base, size);
		return;
	}

	/* map generic ACL's to internal ACL's */
	/* enable region & add size */
	region->rdp = vmpu_map_ac(ac) | AUX_MPU_RDP_REGION_SIZE(bits);
	region->base = (uint32_t) base;
	region->end = (uint32_t) base + size_rounded;
	region->size = size_rounded;
	region->ac = ac;

	SECURESHIELD_DBG("rounded=%d}\r\n", size_rounded);

}


/**
 * find the access control of the specified address with the specified size
 * \param[in]  fault_addr [description]
 * \param[in]  size       [description]
 * \return     0
 */
uint32_t vmpu_fault_find_ac(uint32_t fault_addr, uint32_t size)
{
	return 0;
}

/* FIXME: added very simple MPU region switching - optimize! */
/**
 * \brief MPU context switch
 * \param[in] src_id source container
 * \param[in] dst_id destination container
 */
void vmpu_switch(uint8_t src_id, uint8_t dst_id)
{
	int32_t i, mpu_slot;
	const MPU_CONTAINER *container;
	const MPU_REGION *region;

	if (!g_mpu_region_count || dst_id >= SECURESHIELD_MAX_CONTAINERS) {
		SECURESHIELD_HALT("dst container out of range (%d)", dst_id);
		return;
	}

	SECURESHIELD_DBG("switching from %d to %d\r\n", src_id, dst_id);
	/* remember active container */
	g_active_container = dst_id;

	/* update target container first to make target stack available */
	mpu_slot = ARC_MPU_RESERVED_REGIONS;

	if (dst_id) {
		/* handle target container next */
		container = &g_mpu_container[dst_id];
		region = container->region;

		for (i = 0; i < container->count; i++) {
			if (mpu_slot >= ARC_FEATURE_MPU_REGIONS) {
				return;
			}

			 /* RDP must be set before RDB */
			_arc_aux_write(AUX_MPU_RDP0 + mpu_slot * 2, region->rdp);
			_arc_aux_write(AUX_MPU_RDB0 + mpu_slot * 2,
				region->base | AUX_MPU_RDB_VALID_MASK);

			region++;
			mpu_slot++;
		}
	}

	/* handle background container at last */
	container = g_mpu_container;
	region = container->region;

	for (i = 0; i < container->count; i++) {
		if (mpu_slot >= ARC_FEATURE_MPU_REGIONS) {
			return;
		}

		_arc_aux_write(AUX_MPU_RDP0 + mpu_slot * 2, region->rdp);
		_arc_aux_write(AUX_MPU_RDB0 + mpu_slot *2,
			region->base | AUX_MPU_RDB_VALID_MASK);

		region++;
		mpu_slot++;
	}

	/* clear remaining slots */
	while (mpu_slot < ARC_FEATURE_MPU_REGIONS) {
		_arc_aux_write(AUX_MPU_RDP0 + mpu_slot * 2, 0);
		_arc_aux_write(AUX_MPU_RDB0 + mpu_slot * 2, 0);
		mpu_slot++;
	}
}

/**
 * \brief load container
 * \param[in] container_id the specified container
 */
void vmpu_load_container(uint8_t container_id)
{
	if (container_id == 0) {
		vmpu_switch(0, 0);
	} else {
		SECURESHIELD_HALT("currently only container 0 can be loaded");
	}
	/*  container 0 is the background container and shared to other containers, must be loaded */
}

/**
 * \brief add an global MPU region
 * \param[in] region region id
 * \param[in] base start address
 * \param[in] size region size
 * \param[in] ac region access control
 * \return adjusted region size, 0 for fail
 */
uint32_t vmpu_ac_static_region(uint8_t region, void* base, uint32_t size, CONTAINER_AC ac)
{
	MPU_REGION res;

	/* apply access control */
	vmpu_ac_update_container_region(&res, 0, base, size, ac);


	_arc_aux_write(AUX_MPU_RDP0+region*2, res.rdp);
	_arc_aux_write(AUX_MPU_RDB0+region*2, res.base | AUX_MPU_RDB_VALID_MASK);

	return res.size;
}

/**
 * \brief add a memory space region for the specified container
 * \param[in] container_id the specified container
 * \param[in] addr start address
 * \param[in] size region size
 * \param[in] ac access control
 */
void vmpu_ac_mem(uint8_t container_id, void* addr, uint32_t size, CONTAINER_AC ac)
{
	MPU_CONTAINER *container;
	MPU_REGION *region;

	if (g_mpu_region_count >= MPU_REGION_COUNT) {
		SECURESHIELD_HALT("vmpu_ac_mem ran out of regions");
		return ;
	}

	/* assign container region pointer */
	container = &g_mpu_container[container_id];
	if (!container->region) {
		container->region = &g_mpu_list[g_mpu_region_count];
	}

	/* allocate new MPU region */
	region = &container->region[container->count];
	if (region->rdp) {
		SECURESHIELD_HALT("unordered region allocation");
		return;
	}

	/* calculate MPU RASR/BASR registers */
	vmpu_ac_update_container_region(region, container_id, addr, size, ac);

	/* take account for new region */
	container->count++;
	g_mpu_region_count++;
}

/**
 * \brief load the basic configuration of the specified container
 * \param[in] container_id the specified container
 * \param[in] container_cfg container configuration
 */
void vmpu_ac_container(uint8_t container_id, const CONTAINER_CONFIG *container_cfg)
{
/* note: for ARCv2, minimum region size is 2K bytes, region address must be
aligned to the size of region and the region's size must be  2K, 4K, 8K ...*/
	uint32_t size;
	uint32_t text_attribute;
	uint32_t rodata_attribute;
	uint32_t ram_attribute;


	if (container_cfg->type == SECURESHIELD_CONTAINER_SECURE) {
		/* secure container shares the same MPU regions of secureshield runtime */
		g_container_context[container_id].cpu_status = SECURE_CONTAINER_DEFAULT_STATUS;

		text_attribute = SECURESHIELD_ACDEF_KTEXT;
		rodata_attribute = SECURESHIELD_ACDEF_KROM;
		ram_attribute = SECURESHIELD_ACDEF_KRAM;

	} else if (container_cfg->type == SECURESHIELD_CONTAINER_NORMAL) {
		g_container_context[container_id].cpu_status = NORMAL_CONTAINER_DEFAULT_STATUS;

		text_attribute = SECURESHIELD_ACDEF_UTEXT;
		rodata_attribute = SECURESHIELD_ACDEF_UROM;
		ram_attribute = SECURESHIELD_ACDEF_URAM;

	} else {
		SECURESHIELD_HALT("unsupported container type");
		return;
	}


	size = VMPU_REGION_SIZE(container_cfg->text_start, container_cfg->rodata_end);
	if (size != 0) {
		vmpu_ac_mem(container_id, container_cfg->text_start, size, text_attribute);
	}

	size = VMPU_REGION_SIZE(container_cfg->data_start, container_cfg->bss_end);
	if (size != 0) {
		vmpu_ac_mem(container_id, container_cfg->data_start, size, ram_attribute);

		if (container_id) {
			memcpy(container_cfg->data_start, container_cfg->data_load_start, VMPU_REGION_SIZE(container_cfg->data_start, container_cfg->data_end));
			memset(container_cfg->bss_start, 0, VMPU_REGION_SIZE(container_cfg->bss_start, container_cfg->bss_end));
		}
	}

	/* handle background container */
	if (!container_id) {
		/* container 0 is background container, it uses the default stack */
		g_container_context[container_id].cur_sp = (uint32_t *)_arc_aux_read(AUX_USER_SP);
	} else {
		g_container_context[container_id].cur_sp =  container_cfg->stack_area + container_cfg->stack_size;
	}
}

/**
 * \brief MPU walk function when an MPU exception occurs
 * \param[in] fault_address, the address which caused exception
 * \param[in] type,  not used now
 * \return -1 the mpu region not found, 0 mpu region found
 */
int32_t vmpu_fault_recovery_mpu(uint32_t fault_addr, uint32_t type)
{
	const MPU_REGION *region;

	type = type;

	/* MPU region walk like MMU region walk */
	if ((region = vmpu_fault_find_region(fault_addr)) == NULL) {
		SECURESHIELD_DBG("the region for 0x%x is not found\r\n", fault_addr);
		return -1;
	}

	_arc_aux_write(AUX_MPU_RDP0 + g_mpu_slot * 2, region->rdp);
	_arc_aux_write(AUX_MPU_RDB0 + g_mpu_slot * 2, region->base | AUX_MPU_RDB_VALID_MASK);
	g_mpu_slot++;

	/* if g_mpu_slot is overflow  go back to start to do the recovery */
	if (g_mpu_slot >= ARC_FEATURE_MPU_REGIONS) {
		/* why + 2 ?  because the first 2 regions maybe used for container's
		 rom (.text &.rodata) and ram (.data and .bss)
		 */
		g_mpu_slot = ARC_MPU_RESERVED_REGIONS + 2;
	}

	return 0;
}

/**
 * \brief add an interface for the specified container
 * \param[in] container_id the specified container
 * \param[in] func interface function
 * \param[in] args_num valid argument number
 * \return  0 for success, -1 for fail
 */
int32_t vmpu_ac_interface(uint8_t container_id, void *func, uint32_t args_num)
{
	INTERFACE_CONTAINER *container;
	CONTAINER_INTERFACE *interface;

	if (container_id == 0) {
		SECURESHIELD_HALT("container 0 is background container, no need to allocate interface");
		return -1;
	}

	if (g_interface_count >= CONTAINER_INTERFACE_COUNT) {
		SECURESHIELD_HALT("no more place for interface");
		return -1;
	}

	SECURESHIELD_DBG("\tcontainer[%d] has interface @0x%x with %d arguments\r\n",
		container_id, (uint32_t)func, args_num);

	/* assign interface container pointer */
	container = &g_interface_container[container_id];

	if (!container->interface) {
		container->interface = &g_interface_list[g_interface_count];
	}

	/* allocate new MPU region */
	interface = &container->interface[container->count];

	interface->func = func;
	interface->args_num = args_num;

	/* take account for new interface */
	container->count++;
	g_interface_count++;

	return 0;
}

/**
 * \brief get the correct interface of the specified container
 * \param[in] container_id the specified container
 * \param[in] func the callee interface
 * \param[in] args_num the argument number of the callee interface
 * \return real interface pointer
 */
void * vmpu_find_interface(uint8_t container_id, void *func, uint32_t args_num)
{
	uint32_t i;
	INTERFACE_CONTAINER *container;
	CONTAINER_INTERFACE *interface;

	if (container_id == 0) {
		SECURESHIELD_HALT("container 0 has no interfaces");
		return NULL;
	}

	container = &g_interface_container[container_id];

	if (!container->interface) {
		return NULL;
	}

	interface = &container->interface[0];

	for (i = 0; i < container->count; i++) {
		if (interface->func == func && interface->args_num == args_num) {
			return func;
		}
		interface++;
	}

	return NULL;
}

/* \brief vmpu_arch_init is target-dependent */
void vmpu_arch_init(void)
{
	uint32_t mpu_cfg;

	g_mpu_slot = ARC_MPU_RESERVED_REGIONS;
	/* init protected container memory enumeration pointer */
	mpu_cfg = _arc_aux_read(AUX_BCR_MPU);
	SECURESHIELD_DBG("MPU version:%x, regions:%d\r\n", mpu_cfg & 0xff, (mpu_cfg >> 8) & 0xff);
	SECURESHIELD_DBG("MPU ALIGNMENT=0x%x\r\n", 1UL << ARC_FEATURE_MPU_ALIGNMENT_BITS);
	SECURESHIELD_DBG("MPU ALIGNMENT_BITS=%d\r\n", vmpu_bits(1UL << ARC_FEATURE_MPU_ALIGNMENT_BITS));

	/* background for secureshield and application */
	/* application cannot access secureshield except execution */
	/* for iccm, UREAD, KREAD, KWRITE, U&S EXECUTE are ok */
	SECURESHIELD_DBG("Shared AC table to all containers:\r\n");

	vmpu_ac_static_region(0, (void *)SECURE_ROM_START,
		(uint32_t)(__secureshield_config.secure_rom_end) - SECURE_ROM_START,
		 SECURESHIELD_ACDEF_KROM |  SECURESHIELD_AC_EXECUTE);

	/* for application iccm, all operations are allowed */
	vmpu_ac_static_region(1, (void *)SECURE_RAM_START,
		(uint32_t)(__secureshield_config.secure_ram_end) - SECURE_RAM_START,
		 SECURESHIELD_ACDEF_KRAM);
}