/*------------------------------------------
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
 * \version 2017.03
 * \date 2016-09-18
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
//#define DEBUG
//#define DEBUG_HOSTLINK	/* use hostlink to output debug message */
#include "secureshield.h"
#include "secureshield_vmpu.h"
#include "secureshield_int.h"
#include "secureshield_secure_call_exports.h"
#include "secureshield_container_stack.h"
#include "secureshield_container_call.h"

#ifndef SECURESHIELD_SECURE_INT_EXC_STACK_SIZE
#define SECURESHIELD_SECURE_INT_EXC_STACK_SIZE 256 // 1024 bytes
#endif

/* default interrupt exception entry */
extern void secureshield_exc_entry_int(void);
/* default cpu exception entry */
extern void secureshield_exc_entry_cpu(void);

/* default exception handler */
static void secureshield_exc_handler_default(void *frame);
/* privilege violation exception handler */
static void secureshield_exc_handler_privilege_v(void *frame);
/* protection violation exception handler */
static void secureshield_exc_handler_protect_v(void * frame);

extern uint32_t * secureshield_runtime_stack_ptr;

EMBARC_ALIGNED(1024)
const EXC_ENTRY secureshield_exc_entry_table[NUM_EXC_ALL] = {
	[0] = (EXC_ENTRY)NORMAL_ROM_START,
	[1 ... NUM_EXC_CPU-1] = secureshield_exc_entry_cpu,
	[NUM_EXC_CPU ... NUM_EXC_ALL-1] = secureshield_exc_entry_int
 };


EXC_HANDLER secureshield_exc_handler_table[NUM_EXC_CPU] = {
	secureshield_exc_handler_default,	/* Reset */
	secureshield_exc_handler_default,	/* Memory Error */
	secureshield_exc_handler_default,	/* Instruction Error */
	secureshield_exc_handler_default,	/* EV_MachineCheck */
	secureshield_exc_handler_default,	/* Reserved */
	secureshield_exc_handler_default,	/* Reserved */
	secureshield_exc_handler_protect_v,	/* EV_PortV */
	secureshield_exc_handler_privilege_v,	/* EV_PrivilegeV */
	secureshield_exc_handler_default,	/* EV_SWI	*/
	secureshield_exc_handler_default,	/* EV_Trap	*/
	secureshield_exc_handler_default,	/* EV_Extension	*/
	secureshield_exc_handler_default,	/* EV_DivZero	*/
	secureshield_exc_handler_default,	/* EV_DCError	*/
	secureshield_exc_handler_default,	/* EV_Maligned	*/
	secureshield_exc_handler_default,	/* Unused	*/
	secureshield_exc_handler_default	/* Unused	*/
};

/* secure interrupt table */
SECURESHIELD_INT_HANDLER secureshield_int_handler_table[NUM_EXC_INT];

/**
 * \brief handler for new ecr in secureshield
 * \param[in] parameter exception parameter
 */
static void secure_violation_handler(uint32_t parameter)
{

	switch (parameter) {
		case 0x1:
			SECURESHIELD_HALT("N to S with wrong return type");
			break;
		case 0x2:
			SECURESHIELD_HALT("Mismatched return U/K");
			break;
		case 0x3:
			SECURESHIELD_HALT("IRQ/Exception return fetch from wrong mode");
			break;
		case 0x4:
			SECURESHIELD_HALT("Attempt to halt processor in N mode");
			break;
		case 0x20:
			SECURESHIELD_HALT("Attempt to access secure resource from N mode");
			break;
		case 0x40:
			SECURESHIELD_HALT("SID violation on resource access");
			break;
		default:
			SECURESHIELD_HALT("unknown parameter");
			break;
	}
}

/**
 * \brief privilege violation exception handler
 * \param[in] frame exception frame
 */
static void secureshield_exc_handler_privilege_v(void *frame)
{
	uint32_t excpt_cause_reg;
	uint32_t excpt_ret_reg;
	uint8_t cause_code;
	uint8_t parameter;

	excpt_cause_reg = _arc_aux_read(AUX_ECR);
	excpt_ret_reg = _arc_aux_read(AUX_ERRET);

	parameter = excpt_cause_reg & 0xff;
	cause_code = (excpt_cause_reg >> 8) & 0xff;

	SECURESHIELD_DBG("privilege violation exception at 0x%x\r\n cause:", excpt_ret_reg);
	switch (cause_code) {
		case AUX_ECR_C_PRIV_PRIV:
			SECURESHIELD_HALT("privilege violation, parameter:0x%x", parameter);
			break;
		case AUX_ECR_C_PRIV_DIS_EXT:
			SECURESHIELD_HALT("disabled extension, parameter:0x%x", parameter);
			break;
		case AUX_ECR_C_PRIV_ACT_HIT:
			SECURESHIELD_HALT("action point hit, parameter:0x%x", parameter);
			break;
		case AUX_ECR_C_PRIV_ACT_SECURE:
			SECURESHIELD_HALT("kernel only extension violation, parameter 0x%x", parameter);
			/* for seti, clri, wfi, sleep instructions, do we need to do special handling for it ? */
			secure_violation_handler(parameter);
			break;
		default:
			SECURESHIELD_HALT("unknown cause:0x%x", cause_code);
			break;
	}
	/* \todo, how to recover from privilege violation exception */
}

/**
 * \brief protection violation exception handler
 * \param[in] frame exception frame
 */
static void secureshield_exc_handler_protect_v(void * frame)
{
	uint32_t excpt_cause_reg;
	uint32_t excpt_ret_reg;
	uint8_t cause_code;
	uint8_t parameter;
	int32_t ret = -1;

	excpt_cause_reg = _arc_aux_read(AUX_ECR);
	excpt_ret_reg = _arc_aux_read(AUX_ERRET);

	parameter = excpt_cause_reg & 0xff;
	cause_code = (excpt_cause_reg >> 8) & 0xff;


	SECURESHIELD_DBG("protection violation exception at 0x%x\r\n cause:", excpt_ret_reg);
	switch (cause_code) {
		case AUX_ECR_C_PROTV_INST_FETCH:	// 0x24 i-fetch invalid S/N transition, 0x44 i-fetch invalid SID, 0x00/0x04 old instruction violation 
			SECURESHIELD_DBG("instruction fetch violation, parameter:0x%x\r\n"
				, parameter);
			ret = vmpu_fault_recovery_mpu(excpt_ret_reg, 0);
			break;
		case  AUX_ECR_C_PROTV_LOAD:	// 0x24 S-mode read, 0x44 SID mismatch, 0x01/0x2/0x4 old read violation
			SECURESHIELD_DBG("memory read violation, parameter:0x%x\r\n"
				, parameter);
			if (parameter & 0x04) {
				ret = vmpu_fault_recovery_mpu(_arc_aux_read(AUX_EFA), 1);
			}
			break;
		case AUX_ECR_C_PROTV_STORE: // 0x24 S-mode write, 0x44 SID mismatch, 0x1/0x2/0x4 old write violation
			SECURESHIELD_DBG("memory write violation, parameter:0x%x\r\n"
				, parameter);
			if (parameter & 0x04) {
				ret = vmpu_fault_recovery_mpu(_arc_aux_read(AUX_EFA), 2);
			}
			break;
		case AUX_ECR_C_PROTV_XCHG:	// 0x24 S-mode violation on read-modify-write, 0x44 SID mismatch, 0x1/0x2/0x4 old read-modify-write violation
			SECURESHIELD_DBG("memory read-modify-write, parameter:0x%x\r\n"
				, parameter);
			if (parameter & 0x04) {
				ret = vmpu_fault_recovery_mpu(_arc_aux_read(AUX_EFA), 3);
			}
			break;
		case AUX_ECR_C_PROTV_NS_VECT_IN_S:	// NS vector table in S memory
			SECURESHIELD_DBG("Normal vector table in secure memory\r\n");
			break;
		case AUX_ECR_C_PROTV_NS_HANDLER_IN_S:	// NS handler code located in S memory
			SECURESHIELD_DBG("Normal handler code in secure memory\r\n");
			break;
		case AUX_ECR_C_PROTV_NSC_RANGE: 	// NSC table range violation
			SECURESHIELD_DBG("NSC table range violation\r\n");
			break;
		default:
			SECURESHIELD_DBG("unknown cause:0x%x\r\n", cause_code);
			break;
	}

	if (ret != 0) {
		SECURESHIELD_HALT("halt caused by protection violation");
	}
}

/**
 * \brief default exception handler
 * \param[in] frame exception frame
 */
static void secureshield_exc_handler_default(void *frame)
{
	uint32_t excpt_cause_reg;
	uint32_t excpt_ret_reg;
	uint32_t vector_no;

	excpt_cause_reg = _arc_aux_read(AUX_ECR);
	excpt_ret_reg = _arc_aux_read(AUX_ERRET);
	vector_no = (excpt_cause_reg >> 16) & 0xff;

	SECURESHIELD_DBG("default cpu exception handler\r\n");
	SECURESHIELD_DBG("excpt:%d, last sp:0x%x, ecr:0x%x, eret:0x%x\r\n",
		vector_no, (uint32_t)frame, excpt_cause_reg, excpt_ret_reg);

	SECURESHIELD_HALT("halt caused by raising the default cpu exception handler");
}

/**
 * \brief check interrupt parameter
 * \param[in] intno interrupt no.
 * \return 0 ok, -1 failed
 */
static int32_t secure_int_default_check(uint32_t intno)
{
	/* check the range */
	if (intno < NUM_EXC_CPU || intno >= NUM_EXC_ALL) {
		SECURESHIELD_DBG("Not allowed: IRQ %d is out of range\r\n", intno);
		return -1;
	}

	/* check the entry whether is owned by secureshield */
	if (secureshield_exc_entry_table[intno] != secureshield_exc_entry_int) {
		SECURESHIELD_DBG(
			"Permission denied: IRQ %d entry is not owned by secureshield\r\n"
			, intno);
		return -1;
	}
	return 0;
}

/**
 * \brief check interrupt's access control
 * \param[in] intno interrupt number
 * \return 0 ok, -1 failed
 */
static int32_t secure_int_ac_check(uint32_t intno)
{
	SECURESHIELD_INT_HANDLER* exc;
	uint32_t is_installed;

	if (secure_int_default_check(intno) != 0) {
		return -1;
	}

	/* reduce an offset */
	exc = &secureshield_int_handler_table[intno - NUM_EXC_CPU];

	is_installed = exc->handler ? 1 : 0;

	if (is_installed == 0) {
		SECURESHIELD_DBG("IRQ %d is not installed\r\n", intno);
		return -1;
	}

	/* currently, secure container has the right to access all interrupts */
	if (container_is_secure(g_active_container))
		return 0;

	/* more checks for normal container, if the intno does not belong to the active container */
	if (exc->id != g_active_container && exc->id) {
		SECURESHIELD_DBG(
			"Permission denied: IRQ %d is owned by container %d"
			, intno, exc->id);
		return -1;
	}

	return 0;
}

/**
 * \brief disable interrupt
 * \param[in] intno interrupt no.
 * \return 0 0k, -1 failed
 */
int32_t secure_int_disable(uint32_t intno)
{
	if (secure_int_ac_check(intno) != 0) {
		return -1;
	}

	arc_int_disable(intno);
	secureshield_int_handler_table[intno - NUM_EXC_CPU].enabled = 0;
	return 0;
}

/**
 * \brief enable interrupt
 * \param[in] intno interrupt no.
 * \return 0 0k, -1 failed
 */
int32_t secure_int_enable(uint32_t intno)
{
	if (secure_int_ac_check(intno) != 0) {
		return -1;
	}

	arc_int_enable(intno);
	secureshield_int_handler_table[intno - NUM_EXC_CPU].enabled = 1;
	return 0;
}

/**
 * \brief is interrupt enabled?
 * \param[in] intno interrupt no.
 * \return 0 disabled, 1 enabled, -1 failed
 */
int32_t secure_int_enabled(uint32_t intno)
{
	if (secure_int_ac_check(intno) != 0) {
		return -1;
	}

	_arc_aux_write(AUX_IRQ_SELECT, intno);
	return _arc_aux_read(AUX_IRQ_ENABLE);
}

/**
 * \brief set interrupt priority
 * \param[in] intno interrupt no.
 * \return 0 0k, -1 failed
 */
int32_t secure_int_pri_set(uint32_t intno, int intpri)
{

	if (secure_int_ac_check(intno) != 0) {
		return -1;
	}

	intpri = intpri - INT_PRI_MIN;
	arc_int_pri_set(intno,(uint32_t)intpri);
	return 0;
}

/**
 * \brief get interrupt priority
 * \param[in] intno interrupt no.
 * \return interrupt priority, 0 for failed
 */
int32_t secure_int_pri_get(uint32_t intno)
{
	if (secure_int_ac_check(intno) != 0) {
		return 0;
	}

	return (int32_t)arc_int_pri_get(intno) + INT_PRI_MIN;;
}

/**
 * \brief trigger the interrupt in software
 * \param[in] intno interrupt no.
 * \return  0 ok,  -1 failed
 */
int32_t secure_int_sw_trigger(uint32_t intno)
{
	if (secure_int_ac_check(intno) != 0) {
		return -1;
	}

	arc_int_sw_trigger(intno);
	return 0;
}

/**
 * \brief probe the interrupt whether is pending
 * \param[in] intno interrupt no.
 * \return 1 pending, 0 no pending, -1 error
 */
int32_t secure_int_probe(const uint32_t intno)
{
	if (secure_int_ac_check(intno) != 0) {
		return -1;
	}

	return  arc_int_probe(intno);
}

/**
 * \brief config the trigger level of interrupt
 * \param[in] intno interrupt no.
 * \param[in] level 0-level trigger, 1-pulse triggered
 */
int32_t secure_int_level_config(const uint32_t intno, const uint32_t level)
{
	if (secure_int_ac_check(intno) != 0) {
		return -1;
	}

	arc_int_level_config(intno, level);
	return 0;
}

/**
 * \brief lock cpu--just disable the interrupt belonging to current container
 */
void secure_cpu_lock(void)
{
	uint32_t i;
	SECURESHIELD_INT_HANDLER* exc;

	exc = secureshield_int_handler_table;
	for (i = 0; i < NUM_EXC_INT; i++) {
		if ((exc->id == g_active_container || exc->id == 0 ) && exc->handler != NULL) {
			arc_int_disable(i + NUM_EXC_CPU);
		}
		exc++;
	}

}

/**
 * \brief unlock cpu--just enable the interrupt belonging to current container
 */
void secure_cpu_unlock(void)
{
	uint32_t i;
	SECURESHIELD_INT_HANDLER* exc;

	exc = secureshield_int_handler_table;
	for (i = 0; i < NUM_EXC_INT; i++) {
		if ((exc->id == g_active_container || exc->id == 0) && exc->handler != NULL
			 && exc->enabled) {
			/* pass disabled interrupt */
			arc_int_enable(i + NUM_EXC_CPU);
		}
		exc++;
	}

}

/**
 * \brief allocate the specific interrupt to the specific container
 * \param[in] container_id container id
 * \param[in] handler interrupt handler
 * \param[in] intno interrupt
 * \return 0 ok, -1 failed
 */
int32_t vmpu_ac_irq(uint8_t container_id, INT_HANDLER handler, uint32_t intno)
{
	SECURESHIELD_INT_HANDLER* exc;

	if (secure_int_default_check(intno) != 0)
		return -1;

	exc = &secureshield_int_handler_table[intno - NUM_EXC_CPU];

	/* interrupt is already installed */
	/* other container can change background container's irq resources */
	if (exc->id) {
		SECURESHIELD_DBG(
			"Permission denied: IRQ %d is owned by container %d\n\r"
			, intno, exc->id);
		return -1;
	}

	SECURESHIELD_DBG("\tcontainer[%d] has interrupt %d with handler @%x\r\n",
		container_id, intno, (uint32_t)handler);

	exc->id = container_id;
	exc->handler = handler;

	if (container_id != 0) {
		//SECURESHIELD_DBG("normal interrupts are shared to all normal containers\r\n");
		arc_int_secure_set(intno, 1);
	} else {
		arc_int_secure_set(intno, 0); // background container's interrupt is normal interrupt
	}
	return 0;
}

/**
 * \brief add an handler for the specific secure interrupt
 * \param[in]  intno   interrupt no.
 * \param[in]  handler pointer to handler
 * \return 0 ok, -1 failed
 */
int32_t secure_int_handler_install(uint32_t intno, INT_HANDLER handler)
{

	SECURESHIELD_INT_HANDLER* exc;

	if (secure_int_default_check(intno) != 0) {
		return -1;
	}

	/* reduce an offset */
	exc = &secureshield_int_handler_table[intno - NUM_EXC_CPU];


	/* if the intno does not belong to the active container */
	if (exc->id != g_active_container && exc->id) {
		SECURESHIELD_DBG(
			"Permission denied: IRQ %d is owned by container %d"
			, intno, exc->id);
		return -1;
	}


	exc->handler = (EXC_HANDLER) handler;
	exc->id = handler ? g_active_container : 0;

	SECURESHIELD_DBG("IRQ %d %s container %d\n\r",
		intno,
		handler ? "registered to" : "released by",
		g_active_container);

	return 0;
}

/**
 * \brief get the handler of the specific intno
 * \param[in]  intno interrupt no.
 * \return     pointer to handler
 */
INT_HANDLER secure_int_handler_get(uint32_t intno)
{
	if (secure_int_ac_check(intno) != 0) {
		return NULL;
	}

	return secureshield_int_handler_table[intno - NUM_EXC_CPU].handler;
}

typedef struct aux_irq_ctrl_field {
	/* note: little endian */
	uint32_t save_nr_gpr_pairs: 5;	/** Indicates number of general-purpose register pairs saved, from 0 to 8/16 */
	uint32_t res: 4;		/** Reserved */
	uint32_t save_blink: 1;		/** Indicates whether to save and restore BLINK */
	uint32_t save_lp_regs: 1;	/** Indicates whether to save and restore loop registers (LP_COUNT, LP_START, LP_END) */
	uint32_t save_u_to_u: 1;	/** Indicates if user context is saved to user stack */
	uint32_t res2: 1;		/** Reserved */
	uint32_t save_idx_regs: 1;	/** Indicates whether to save and restore code-density registers (EI_BASE, JLI_BASE, LDI_BASE) */
	uint32_t res3: 18;		/** Reserved */
} aux_irq_ctrl_field_t;

typedef union {
	aux_irq_ctrl_field_t bits;
	uint32_t value;
} aux_irq_ctrl_t;

/**
 * \brief secureshield interrupt init
 */
void secureshield_int_init(void)
{
	uint32_t i;
	aux_irq_ctrl_t ictrl;

	ictrl.value = 0;

#ifndef ARC_FEATURE_RF16
	ictrl.bits.save_nr_gpr_pairs = 6;	/* r0 to r11 (r12 saved manually) */
#else
	ictrl.bits.save_nr_gpr_pairs = 3;	/* r0 to r3, r10, r11  */
#endif
	ictrl.bits.save_blink = 1;
	ictrl.bits.save_lp_regs = 1;		/* LP_COUNT, LP_START, LP_END */
	ictrl.bits.save_u_to_u = 1;		/* user context saved on user stack */
	ictrl.bits.save_idx_regs = 1;		/* JLI, LDI, EI */

	_arc_aux_write(AUX_INT_VECT_BASE_S, (uint32_t)secureshield_exc_entry_table);
	for (i = NUM_EXC_CPU;i < NUM_EXC_ALL; i++) {

		/* interrupt level triggered, disabled, priority is the lowest */
		_arc_aux_write(AUX_IRQ_SELECT, i);
		_arc_aux_write(AUX_IRQ_ENABLE, 0);
		_arc_aux_write(AUX_IRQ_TRIGGER, 0);
		_arc_aux_write(AUX_IRQ_PRIORITY, (INT_PRI_MAX - INT_PRI_MIN) |
			(1 << AUX_IRQ_PRIORITY_BIT_S));
	}
	/* U bit is set, register is saved in user stack */
	_arc_aux_write(AUX_IRQ_CTRL, ictrl.value);
	arc_int_ipm_set(INT_PRI_MAX - INT_PRI_MIN);

	/* all exceptions are handled in secure world, except swi and trap exception */
	_arc_aux_write(AUX_SEC_EXCEPT, 0xfcff);
}


/**
 * \brief secure interrupt handle
 * \details the secure interrupt is handled in secureshield runtime stack
 *
 * \param[in] src_frame interrupt frame
 * \return  interrupt handler address
 */
void * secureshield_interrupt_handle(INT_EXC_FRAME *src_frame, uint32_t runtime)
{
	uint32_t src_id, dst_id;
	INT_HANDLER handler;

	/* reuse src_id and dst_id */
	src_id = _arc_aux_read(AUX_IRQ_CAUSE);

	/* if triggered by software, clear it */
	if (src_id == _arc_aux_read(AUX_IRQ_HINT)) {
		_arc_aux_write(AUX_IRQ_HINT, 0);
	}

	/* verify IRQ access privileges */
	if (secure_int_default_check(src_id) != 0) {
		arc_int_disable(src_id);
		return NULL;
	}

	/* get dst_id and interrupt handler */
	dst_id = secureshield_int_handler_table[src_id - NUM_EXC_CPU].id;
	handler = secureshield_int_handler_table[src_id - NUM_EXC_CPU].handler;


	/* check interrupt handler */
	if (!handler) {
		SECURESHIELD_DBG("Unprivileged handler for IRQ %d not found\r\n", src_id);
		arc_int_disable(src_id);
		return NULL;
	}

	src_id = g_container_stack_curr_id;

	_arc_aux_write(AUX_ERRET, (uint32_t)handler);


	if (src_id != dst_id) {
		vmpu_switch(src_id, dst_id);
		container_stack_push(src_id, (uint32_t *)src_frame - ARC_CALLEE_FRAME_SIZE, src_frame->status32, dst_id);

		/* for all normal interrupts happened in secure world, it's handled in background container stack */
		if (container_is_secure(dst_id)) {
			_arc_aux_write(AUX_KERNEL_SP, (uint32_t)g_container_context[0].cur_sp);
		}

		if (runtime) {
			_arc_aux_write(AUX_SEC_K_SP,  (uint32_t)g_container_context[src_id].cur_sp);
		}
		return (void *) g_container_context[dst_id].cur_sp + 1;
	} else {
		container_stack_push(src_id, (uint32_t *)src_frame, src_frame->status32, dst_id);
		if (runtime) {
			// must be normal containers
			return (void *)_arc_aux_read(AUX_KERNEL_SP);
		}
		return (void *) g_container_context[dst_id].cur_sp;
	}
}

/**
 * \brief the handling after secure interrupt handler returns
 * \param[in]  status cpu status (not used now)
 * \return        previous interrupted container's stack pointer
 */
void *secureshield_interrupt_handle_return(uint32_t status)
{
	uint32_t src_id, dst_id;

	dst_id = g_container_stack_curr_id;

	container_stack_pop(dst_id, g_container_context[dst_id].cur_sp, status);

	src_id = g_container_stack[g_container_stack_ptr].src_id;

	if (src_id != dst_id) {

		/* for all normal interrupts happened in secure world, it's handled in background container stack */
		if (container_is_secure(src_id)) {
			_arc_aux_write(AUX_KERNEL_SP, (uint32_t)g_container_context[0].cur_sp);
		}

		vmpu_switch(dst_id, src_id);
		return (void *)g_container_stack[g_container_stack_ptr].src_sp + 1; // need restore callee regs
	}

	return g_container_stack[g_container_stack_ptr].src_sp;
}

/**
 * \brief interrupt operation handler
 * \param[in]  ops  operation id
 * \param[in]  par1 1st parameter
 * \param[in]  par2 2nd parameter
 * \return     operation result
 */
int32_t secureshield_int_ops(uint32_t ops, uint32_t par1, uint32_t par2)
{
	int32_t ret = -1;
	/* FIXME: remove switch case structure, use jump table */
	switch (ops) {
		case SECURESHIELD_INT_EXC_GET:
			ret = (int32_t)secure_int_handler_get(par1);
			break;
		case SECURESHIELD_INT_EXC_INSTALL:
			ret = secure_int_handler_install(par1, (INT_HANDLER)par2);
			break;
		case SECURESHIELD_INT_EXC_ENABLE:
			ret = secure_int_enable(par1);
			break;
		case SECURESHIELD_INT_EXC_ENABLED:
			ret = secure_int_enabled(par1);
			break;
		case SECURESHIELD_INT_EXC_DISABLE:
			ret = secure_int_disable(par1);
			break;
		case SECURESHIELD_INT_EXC_PRI_SET:
			ret = secure_int_pri_set(par1, par2);
			break;
		case SECURESHIELD_INT_EXC_PRI_GET:
			ret = secure_int_pri_get(par1);
			break;
		case SECURESHIELD_INT_EXC_PROBE:
			ret = secure_int_probe(par1);
			break;
		case SECURESHIELD_INT_EXC_LEVEL_CONFIG:
			ret = secure_int_level_config(par1, par2);
			break;
		case SECURESHIELD_INT_EXC_SW_TRIG:
			ret = secure_int_sw_trigger(par1);
			break;
		case SECURESHIELD_INT_EXC_CPU_LOCK:
			secure_cpu_lock();
			break;
		case SECURESHIELD_INT_EXC_CPU_UNLOCK:
			secure_cpu_unlock();
			break;
		default:
			SECURESHIELD_DBG("Unsupported interrupt operation:%d\r\n", ops);
			break;
	}

	return ret;
}


int32_t int_disable(const uint32_t intno) EMBARC_LINKTO(secure_int_disable);
int32_t int_enable(const uint32_t intno) EMBARC_LINKTO(secure_int_enable);
int32_t int_enabled(const uint32_t intno) EMBARC_LINKTO(secure_int_enabled);
int32_t int_pri_get(const uint32_t intno) EMBARC_LINKTO(secure_int_pri_get);
int32_t int_pri_set(const uint32_t intno, int32_t intpri) EMBARC_LINKTO(secure_int_pri_set);
int32_t int_probe(const uint32_t intno) EMBARC_LINKTO(secure_int_probe);
int32_t int_sw_trigger(const uint32_t intno) EMBARC_LINKTO(secure_int_sw_trigger);
int32_t int_level_config(const uint32_t intno, const uint32_t level) EMBARC_LINKTO(secure_int_level_config);
void cpu_lock() EMBARC_LINKTO(secure_cpu_lock);
void cpu_unlock() EMBARC_LINKTO(secure_cpu_unlock);
uint32_t cpu_lock_save(void) EMBARC_LINKTO(secure_cpu_lock);
void cpu_unlock_restore(const uint32_t status) EMBARC_LINKTO(secure_cpu_unlock);
int32_t int_handler_install(const uint32_t intno, INT_HANDLER handler) EMBARC_LINKTO(secure_int_handler_install);
INT_HANDLER int_handler_get(const uint32_t intno) EMBARC_LINKTO(secure_int_handler_get);