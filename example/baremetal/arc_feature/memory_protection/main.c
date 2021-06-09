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
// Notice for EMSDP: please define REGION_RAM as REGION_DCCM in target_mem_config.h to run this test.

/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"

#if ARC_FEATURE_MPU_PRESENT
#define REGION_ROM_ATTR (ARC_MPU_REGION_FLASH_ATTR | AUX_MPU_ATTR_S \
			 | AUX_MPU_ATTR_SID(1))
#define REGION_RAM_ATTR (ARC_MPU_REGION_RAM_ATTR | AUX_MPU_ATTR_S \
			 | AUX_MPU_ATTR_SID(1))
#define REGION_DEFAULT_ATTR (AUX_MPU_ATTR_KW | AUX_MPU_ATTR_KR \
			     | AUX_MPU_ATTR_S | AUX_MPU_ATTR_SID(1))

static EMBARC_ALIGNED(2048) uint32_t user_mode_stack[512];

extern uint8_t _f_init[];
extern uint8_t _e_rodata[];
extern uint8_t _f_data[];

Inline void trigger_read_violation(uint32_t addr)
{
	uint32_t val;

	Asm(
		"ld %0, [%1] \n"
		"mov %0, 0   \n"
		: "=r" (val)
		: "r" (addr));
}

Inline void trigger_write_violation(uint32_t addr)
{
	uint32_t val;

	Asm(
		"mov %0, 0   \n"
		"st %0, [%1] \n"
		"mov %0, 0   \n"
		: "=r" (val)
		: "r" (addr));
}

Inline void trigger_execute_violation(uint32_t addr)
{
	uint32_t val;

	Asm(
		"jl [%1]  \n"
		"mov %0, 0\n"
		: "=r" (val)
		: "r" (addr));
}

static void user_mode_func(void)
{
	/* try to read, write, execute in kernel area */
	trigger_write_violation((uint32_t)_f_data);
	trigger_execute_violation((uint32_t)_f_data);
	trigger_read_violation((uint32_t)_f_data);
	/* try to execute privileged instructions */
	arc_clri();
	arc_kflag(0);
	arc_goto_kernelmode(0, 0);

	EMBARC_PRINTF("MPU Test Done\r\n");
	while (1) {
		;
	}
}

static void dump_protv_access_err(uint32_t parameter)
{
	switch (parameter) {
	case 0x1:
		EMBARC_PRINTF("code protection scheme");
		break;
	case 0x2:
		EMBARC_PRINTF("stack checking scheme");
		break;
	case 0x4:
		EMBARC_PRINTF("MPU");
		break;
	case 0x8:
		EMBARC_PRINTF("MMU");
		break;
	case 0x10:
		EMBARC_PRINTF("NVM");
		break;
	case 0x24:
		EMBARC_PRINTF("Secure MPU");
		break;
	case 0x44:
		EMBARC_PRINTF("Secure MPU with SID mismatch");
		break;
	default:
		EMBARC_PRINTF("unknown parameter");
		break;
	}
}

static void dump_protv_exception(uint32_t cause, uint32_t parameter)
{
	switch (cause) {
	case 0x0:
		EMBARC_PRINTF("Instruction fetch violation: ");
		dump_protv_access_err(parameter);
		break;
	case 0x1:
		EMBARC_PRINTF("Memory read protection violation: ");
		dump_protv_access_err(parameter);
		break;
	case 0x2:
		EMBARC_PRINTF("Memory write protection violation: ");
		dump_protv_access_err(parameter);
		break;
	case 0x3:
		EMBARC_PRINTF("Memory read-modify-write violation: ");
		dump_protv_access_err(parameter);
		break;
	case 0x10:
		EMBARC_PRINTF("Normal vector table in secure memory");
		break;
	case 0x11:
		EMBARC_PRINTF("NS handler code located in S memory");
		break;
	case 0x12:
		EMBARC_PRINTF("NSC Table Range Violation");
		break;
	default:
		EMBARC_PRINTF("unknown cause");
		break;
	}
	EMBARC_PRINTF("\r\n");
}

static void protect_violation_handler(void *p_excinf)
{
	INT_EXC_FRAME_T *exc_frame;
	uint32_t exc_cause;
	uint32_t exc_param;
	uint32_t excpt_cause_reg;

	exc_frame = (INT_EXC_FRAME_T *)p_excinf;
	excpt_cause_reg = arc_aux_read(AUX_ECR);
	exc_cause = (excpt_cause_reg >> 8) & 0xff;
	exc_param = (excpt_cause_reg >> 0) & 0xff;
	dump_protv_exception(exc_cause, exc_param);
	/* return to next instruction */
	if (exc_cause == 0x0) {
		exc_frame->ret = exc_frame->blink;
	} else {
		exc_frame->ret = exc_frame->ret + 4;
	}
}

static void dump_privilege_exception(uint32_t cause, uint32_t parameter)
{
	switch (cause) {
	case 0x0:
		EMBARC_PRINTF("Privilege violation");
		break;
	case 0x1:
		EMBARC_PRINTF("disabled extension");
		break;
	case 0x2:
		EMBARC_PRINTF("action point hit");
		break;
	case 0x10:
		switch (parameter) {
		case 0x1:
			EMBARC_PRINTF("N to S return using incorrect return mechanism");
			break;
		case 0x2:
			EMBARC_PRINTF("N to S return with incorrect operating mode");
			break;
		case 0x3:
			EMBARC_PRINTF("IRQ/exception return fetch from wrong mode");
			break;
		case 0x4:
			EMBARC_PRINTF("attempt to halt secure processor in NS mode");
			break;
		case 0x20:
			EMBARC_PRINTF("attempt to access secure resource from normal mode");
			break;
		case 0x40:
			EMBARC_PRINTF("SID violation on resource access (APEX/UAUX/key NVM)");
			break;
		default:
			EMBARC_PRINTF("unknown parameter");
			break;
		}
		break;
	case 0x13:
		switch (parameter) {
		case 0x20:
			EMBARC_PRINTF("attempt to access secure APEX feature from NS mode");
			break;
		case 0x40:
			EMBARC_PRINTF("SID violation on access to APEX feature");
			break;
		default:
			EMBARC_PRINTF("unknown parameter");
			break;
		}
		break;
	default:
		EMBARC_PRINTF("unknown cause");
		break;
	}
	EMBARC_PRINTF("\r\n");
}

static void privilege_violation_handler(void *p_excinf)
{
	INT_EXC_FRAME_T *exc_frame;
	uint32_t exc_cause;
	uint32_t exc_param;
	uint32_t excpt_cause_reg;

	exc_frame = (INT_EXC_FRAME_T *)p_excinf;
	excpt_cause_reg = arc_aux_read(AUX_ECR);
	exc_cause = (excpt_cause_reg >> 8) & 0xff;
	exc_param = (excpt_cause_reg >> 0) & 0xff;
	dump_privilege_exception(exc_cause, exc_param);
	/* return to next instruction */
	exc_frame->ret = exc_frame->ret + 4;
}
#endif /* ARC_FEATURE_MPU_PRESENT */

/* For HSDK (HS4x) boards to run this example,
   it is necessary to change memory settings in target_mem_config.h
   Otherwise the .data section (_f_data) will fall in ROM MPU region and cause error
   e.g. you could define REGION_EXT_ROM_START to 0x00000000 and REGION_EXT_ROM_SIZE to 0x10000000
   and then define REGION_ROM to REGION_EXT_ROM
 */
int main(void)
{
#if ARC_FEATURE_MPU_PRESENT
	arc_mpu_disable();

	/* install exception handler */
	exc_entry_install(EXC_NO_TRAP, exc_entry_arc_goto_kernelmode);
	exc_handler_install(EXC_NO_PRO_VIO, protect_violation_handler);
	exc_handler_install(EXC_NO_PRI_VIO, privilege_violation_handler);
	/* initialize MPU configuration */
	/* code area (.text + .rodata) */
	arc_mpu_region_config(0, (uint32_t)_f_init,
			      (uint32_t)(_e_rodata - _f_init),
			      REGION_ROM_ATTR);
	/* user mode stack */
	arc_mpu_region_config(1, (uint32_t)user_mode_stack, 2048, REGION_RAM_ATTR);

	/* default mpu setting: kernel R+W */
	arc_mpu_default(REGION_DEFAULT_ATTR);
#if ARC_FEATURE_MPU_VERSION == 2 || ARC_FEATURE_MPU_VERSION == 3
	arc_mpu_enable();
#endif
	/* try to write ROM area */
	EMBARC_PRINTF("raise violation of write to ROM \r\n");
	trigger_write_violation((uint32_t)_f_init);
	/* try to execute in RAM area */
	EMBARC_PRINTF("raise violation of execute in RAM \r\n");
	trigger_execute_violation((uint32_t)_f_data);
	EMBARC_PRINTF("go to user mode \r\n");
#if	ARC_FEATURE_STACK_CHECK
	// disable stack check
	arc_stack_check_disable();
#endif
	arc_goto_usermode(user_mode_func, &user_mode_stack[512]);
	// arc_goto_usermode() does NOT return
#else /* ARC_FEATURE_MPU_PRESENT */
	EMBARC_PRINTF("This example is not supported under current configurations \r\n");
#endif /* ARC_FEATURE_MPU_PRESENT */
}
