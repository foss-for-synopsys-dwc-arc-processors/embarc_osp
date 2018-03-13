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
//#define DEBUG_HOSTLINK 	/* use hostlink to output debug message */
#include "secureshield.h"
#include "secureshield_vmpu.h"
#include "secureshield_container_stack.h"

/* total container count */
uint32_t g_vmpu_container_count;
/* current running container */
uint32_t g_active_container;
/* container call stack variables */
CONTAINER_STACK_ITEM g_container_stack[SECURESHIELD_CONTAINER_STACK_MAX_DEPTH];
/* container stack pointer */
uint32_t g_container_stack_ptr;

/* area to save container context */
CONTAINER_CONTEXT g_container_context[SECURESHIELD_MAX_CONTAINERS];
uint32_t g_container_stack_curr_id;

/**
 * \brief do necessary config checks
 * \return -1 failed, 0 ok
 */
static int32_t vmpu_config_checks(void)
{
	/* verify secureshield config structure */
	if (__secureshield_config.magic != SECURESHIELD_MAGIC) {
		SECURESHIELD_HALT("config magic mismatch: &0x%x = 0x%x - expected 0x%x"
			,(uint32_t)&__secureshield_config, __secureshield_config.magic,
			SECURESHIELD_MAGIC);
		return -1;
	}

	/* verify basic assumptions about EMBARC_BITS/EMBARC_CLZ */
	SECURESHIELD_ASSERT(EMBARC_CLZ(0) == 32);
	SECURESHIELD_ASSERT(EMBARC_CLZ(1UL << 31) == 0);
	SECURESHIELD_ASSERT(EMBARC_BITS(0) == 0);
	SECURESHIELD_ASSERT(EMBARC_BITS(1UL << 31) == 32);
	SECURESHIELD_ASSERT(EMBARC_BITS(0x8000UL) == 16);
	SECURESHIELD_ASSERT(EMBARC_BITS(0x8001UL) == 16);
	SECURESHIELD_ASSERT(EMBARC_BITS(1) == 1);

	/* verify that __secureshield_config is within valid ROM for secureshield */
	SECURESHIELD_ASSERT(((uint32_t)&__secureshield_config) >= SECURE_ROM_START);
	SECURESHIELD_ASSERT((uint32_t)__secureshield_config.secure_rom_start == SECURE_ROM_START);
	SECURESHIELD_ASSERT((uint32_t)__secureshield_config.secure_ram_start == SECURE_RAM_START);
	SECURESHIELD_ASSERT((uint32_t)__secureshield_config.background_rom_start == NORMAL_ROM_START);
	SECURESHIELD_ASSERT((uint32_t)__secureshield_config.background_ram_start == NORMAL_RAM_START);

	SECURESHIELD_ASSERT(((((uint32_t)&__secureshield_config) + sizeof(__secureshield_config))
			 <= (SECURE_ROM_START + SECURE_ROM_SIZE)));

	/* verify RAM relocation */
	SECURESHIELD_DBG("secureshield_ram : @0x%x (%d bytes)\r\n",
		(uint32_t)__secureshield_config.secure_ram_start,
	 	VMPU_REGION_SIZE(__secureshield_config.secure_ram_start,
	 	__secureshield_config.secure_ram_end));

	SECURESHIELD_ASSERT(__secureshield_config.secure_ram_end > __secureshield_config.secure_ram_start);


	/* verify that secure rom area is accessible and after public code */
	SECURESHIELD_ASSERT(__secureshield_config.secure_rom_end >= __secureshield_config.secure_rom_start);
	SECURESHIELD_ASSERT((uint32_t)__secureshield_config.secure_rom_end <= (uint32_t)(SECURE_ROM_START + SECURE_ROM_SIZE));


	/* verify configuration table */
	SECURESHIELD_ASSERT(__secureshield_config.cfgtbl_ptr_start <= __secureshield_config.cfgtbl_ptr_end);
	SECURESHIELD_ASSERT(__secureshield_config.cfgtbl_ptr_start >= __secureshield_config.secure_rom_start);
	SECURESHIELD_ASSERT((uint32_t)__secureshield_config.cfgtbl_ptr_end <= (uint32_t)(SECURE_ROM_START + SECURE_ROM_SIZE));

	return 0;
}


/**
 * \brief setup the execution environment for each container
 */
static void vmpu_setup_containers(void)
{
	uint32_t i, count;
	const CONTAINER_AC_ITEM *region;
	const CONTAINER_CONFIG **container_cfgtbl;
	uint8_t container_id;

	/* enumerate and initialize containers */
	g_vmpu_container_count = 0;
	for (container_cfgtbl = (const CONTAINER_CONFIG**) __secureshield_config.cfgtbl_ptr_start;
		container_cfgtbl < (const CONTAINER_CONFIG**) __secureshield_config.cfgtbl_ptr_end;
		container_cfgtbl++) {
		/* ensure that configuration resides in secureshield rom */
		if (!(VMPU_SECURE_ROM_ADDR(*container_cfgtbl) &&
			VMPU_SECURE_ROM_ADDR(
				((uint8_t*)(*container_cfgtbl)) + (sizeof(**container_cfgtbl)-1)
			))) {
			SECURESHIELD_HALT("invalid address - *container_cfgtbl must point to rom (0x%x)"
				, (uint32_t)*container_cfgtbl);
			return;
		}

		/* check for magic value in container configuration */
		if (((*container_cfgtbl)->magic) != SECURESHIELD_CONTAINER_MAGIC) {
			SECURESHIELD_HALT("container[%d] @0x%x - invalid magic",
				g_vmpu_container_count,
				(uint32_t)(*container_cfgtbl));
			return;
		}

		/* increment container counter */
		if ((container_id = g_vmpu_container_count++) >= SECURESHIELD_MAX_CONTAINERS) {
			SECURESHIELD_HALT("container number overflow");
			return;
		}

		/* load container ACs in table */
		SECURESHIELD_DBG("container[%d] AC table:\r\n", container_id);

		/* add AC for all container stacks, the actual start addresses and
		 * sizes are resolved later in vmpu_ac_stack */
		vmpu_ac_container(container_id, *container_cfgtbl);

		/* enumerate container ACs */
		if ((region = (*container_cfgtbl)->ac_table)!=NULL) {
			count = (*container_cfgtbl)->ac_count;
			for(i = 0; i < count; i++) {
				/* \todo  check the region ? */
				/* \todo: add more AC type support */
				if (region->ac & SECURESHIELD_AC_IRQ) {
					vmpu_ac_irq(container_id, region->param1, region->param2);
				} else if (region->ac & SECURESHIELD_AC_INTERFACE) {
					vmpu_ac_interface(container_id, region->param1, region->param2);
				} else if (region->ac & SECURESHIELD_AC_AUX) {
					vmpu_ac_aux(container_id, (uint32_t)region->param1, region->param2);
				} else { /* memory space based ACs */
					vmpu_ac_mem(container_id, region->param1,region->param2,
						region->ac);
				}
				/* proceed to next AC */
				region++;
			}
		}
	}

	SECURESHIELD_DBG("containers setup\r\n");
}

/**
 * \brief the initial init of vmpu
 */
int32_t vmpu_init_pre(void)
{
	return vmpu_config_checks();
}

/**
 * \brief the post init of vmpu
 */
void vmpu_init_post(void)
{
	/* init memory protection */
	vmpu_arch_init();

	/* setup containers */
	vmpu_setup_containers();

	/* load background container 0 */
	vmpu_load_container(0);
}
