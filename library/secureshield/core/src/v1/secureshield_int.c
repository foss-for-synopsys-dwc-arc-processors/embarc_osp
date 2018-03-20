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
--------------------------------------------- */
//#define DEBUG
//#define DEBUG_HOSTLINK	/* use hostlink to output debug message */
#include "secureshield.h"
#include "secureshield_vmpu.h"
#include "secureshield_int.h"
#include "secureshield_secure_call_exports.h"
#include "secureshield_container_stack.h"
#include "secureshield_container_call.h"


/* default interrupt exception entry */
extern void secureshield_exc_entry_int(void);
/* default cpu exception entry */
extern void secureshield_exc_entry_cpu(void);
/* trap exception handler */
extern void secureshield_trap_handler(void *frame);

static void secureshield_exc_handler_default(void *frame);
static void secureshield_exc_handler_privilege_v(void *frame);
static void secureshield_exc_handler_protect_v(void * frame);

/** secure world exception entry table */
EMBARC_ALIGNED(1024)
const EXC_ENTRY secureshield_exc_entry_table[NUM_EXC_ALL] = {
	[0] = (EXC_ENTRY)NORMAL_ROM_START,
	[1 ... NUM_EXC_CPU-1] = secureshield_exc_entry_cpu,
	[NUM_EXC_CPU ... NUM_EXC_ALL-1] = secureshield_exc_entry_int
 };


/** secure world exception handler table */
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
	secureshield_trap_handler,		/* EV_Trap	*/
	secureshield_exc_handler_default,	/* EV_Extension	*/
	secureshield_exc_handler_default,	/* EV_DivZero	*/
	secureshield_exc_handler_default,	/* EV_DCError	*/
	secureshield_exc_handler_default,	/* EV_Maligned	*/
	secureshield_exc_handler_default,	/* Unused	*/
	secureshield_exc_handler_default	/* Unused	*/
};

/** secure world interrupt handler table */
SECURESHIELD_INT_HANDLER secureshield_int_handler_table[NUM_EXC_INT];

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
		default:
			SECURESHIELD_HALT("unknown cause:0x%x", cause_code);
			break;
	}
}

/**
 * \brief protection violation handler
 * \param[in] frame exception frame
 */
static void secureshield_exc_handler_protect_v(void *frame)
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
		case  AUX_ECR_C_PROTV_INST_FETCH:
			SECURESHIELD_DBG("instruction fetch violation, parameter:0x%x\r\n"
				, parameter);
			ret = vmpu_fault_recovery_mpu(excpt_ret_reg, 0);
			break;
		case AUX_ECR_C_PROTV_LOAD:
			SECURESHIELD_DBG("memory read violation, parameter:0x%x\r\n"
				, parameter);
			if (parameter & 0x04) {
				ret = vmpu_fault_recovery_mpu(_arc_aux_read(AUX_EFA), 1);
			}
			break;
		case AUX_ECR_C_PROTV_STORE:
			SECURESHIELD_DBG("memory write violation, parameter:0x%x\r\n"
				, parameter);
			if (parameter & 0x04) {
				ret = vmpu_fault_recovery_mpu(_arc_aux_read(AUX_EFA), 2);
			}
			break;
		case AUX_ECR_C_PROTV_XCHG:
			SECURESHIELD_DBG("memory read-modify-write violation, parameter:0x%x\r\n"
				, parameter);
			if (parameter & 0x04) {
				ret = vmpu_fault_recovery_mpu(_arc_aux_read(AUX_EFA), 3);
			}
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
 * \brief default exception handler, not recoverable
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
 * \brief check default interrupt attribute
 * \param[in]  intno interrupt no.
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
 * \brief check the access control to the specified interrupt
 * \param[in]  intno interrupt no.
 * \return 0 ok, -1 failed
 */
static int32_t secure_int_ac_check(uint32_t intno)
{
	SECURESHIELD_INT_HANDLER* exc;
	uint32_t is_installed;

	if (secure_int_default_check(intno) !=0 ) {
		return -1;
	}

	/* reduce an offset */
	exc = &secureshield_int_handler_table[intno - NUM_EXC_CPU];

	is_installed = exc->handler ? 1 : 0;

	if (is_installed == 0) {
		SECURESHIELD_DBG("IRQ %d is not installed\r\n");
		return -1;
	}

	/* currently, secure container has the right to access all interrupts */
	if (container_is_secure(g_active_container)) {
		return 0;
	}

	/* more checks for normal container, if the intno does not belong to the active container */
	if (exc->id != g_active_container && exc->id) {
		SECURESHIELD_DBG(
			"Permission denied: IRQ %d is owned by container %d\r\n"
			, intno, exc->id);
		return -1;
	}

	return 0;
}

/**
 * \brief allocate an interrupt to the specified container
 * \param[in]  container_id container id
 * \param[in]  handler 	interrupt handler
 * \param[in]  intno        interrupt no.
 * \return 0 ok, -1 failed
 */
int32_t vmpu_ac_irq(uint8_t container_id, INT_HANDLER handler, uint32_t intno)
{
	SECURESHIELD_INT_HANDLER* exc;

	if (secure_int_default_check(intno) != 0) {
		return -1;
	}

	exc = &secureshield_int_handler_table[intno - NUM_EXC_CPU];

	/* interrupt is already installed */
	/* other container can change background container's irq resources */
	if(exc->id) {
		SECURESHIELD_DBG(
			"Permission denied: IRQ %d is owned by container %d\r\n"
			, intno, exc->id);
		return -1;
	}

	exc->id = container_id;
	exc->handler = handler;

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
 * \brief install an interrupt handler
 * \param[in]  intno   interrupt no.
 * \param[in]  handler interrupt handler
 * \return     0 ok, -1 failed
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
			"Permission denied: IRQ %d is owned by container %d\r\n"
			, intno, exc->id);
		return -1;
	}


	exc->handler = (EXC_HANDLER) handler;
	exc->id = handler ? g_active_container : 0;


	SECURESHIELD_DBG("IRQ %d %s container %d\r\n",
		intno,
		handler ? "registered to" : "released by",
		g_active_container);
	return 0;
}

/**
 * \brief get interrupt handler
 * \param[in]  intno interrupt no.
 * \return interrupt handler
 */
INT_HANDLER secure_int_handler_get(uint32_t intno)
{
	if (secure_int_ac_check(intno) != 0) {
		return NULL;
	}

	return secureshield_int_handler_table[intno - NUM_EXC_CPU].handler;
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
	for(i = 0; i < NUM_EXC_INT; i++) {
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
	for(i = 0; i < NUM_EXC_INT; i++) {
		if ((exc->id == g_active_container || exc->id == 0) && exc->handler != NULL
			 && exc->enabled) {
			arc_int_enable(i + NUM_EXC_CPU);
		}
		exc++;
	}

}

/**
 *  \brief interrupt initialization in secure world
 */
void secureshield_int_init(void)
{
	uint32_t i;
	struct aux_irq_ctrl {
	/* note: little endian */
		uint32_t save_nr_gpr_pairs:5, res:4,
			save_blink:1, save_lp_regs:1, save_u_to_u:1,
			res2:1, save_idx_regs:1, res3:18;
	} ictrl;
#ifndef ARC_FEATURE_RF16
	ictrl.save_nr_gpr_pairs = 6;	/* r0 to r11 (r12 saved manually) */
#else
	ictrl.save_nr_gpr_pairs = 3;	/* r0 to r3, r10, r11  */
#endif
	ictrl.save_blink = 1;
	ictrl.save_lp_regs = 1;		/* LP_COUNT, LP_START, LP_END */
	ictrl.save_u_to_u = 0;		/* user context saved on secure stack */
	ictrl.save_idx_regs = 1;	/* JLI, LDI, EI */

	_arc_aux_write(AUX_INT_VECT_BASE, (uint32_t)secureshield_exc_entry_table);
	for (i = NUM_EXC_CPU;i < NUM_EXC_ALL; i++)
	{
		/* interrupt level triggered, disabled, priority is the lowest */
		_arc_aux_write(AUX_IRQ_SELECT, i);
		_arc_aux_write(AUX_IRQ_ENABLE, 0);
		_arc_aux_write(AUX_IRQ_TRIGGER, 0);
		_arc_aux_write(AUX_IRQ_PRIORITY, INT_PRI_MAX - INT_PRI_MIN);
	}

	_arc_aux_write(AUX_IRQ_CTRL, *(uint32_t *)&ictrl);
	arc_int_ipm_set(INT_PRI_MAX - INT_PRI_MIN);
}


uint32_t* dst_container_user_sp;

/**
 * \brief common part of interrupt handling in secure world
 * \param[in] src_frame   interrupt frame
 * \return  target container sp
 */
uint32_t secureshield_interrupt_handle(INT_EXC_FRAME *src_frame)
{
	uint32_t src_id, dst_id;
	INT_HANDLER handler;

	/* reuse src_id*/
	src_id = _arc_aux_read(AUX_IRQ_CAUSE);

	/* if triggered by software, clear it */
	if (src_id == _arc_aux_read(AUX_IRQ_HINT)) {
		_arc_aux_write(AUX_IRQ_HINT, 0);
	}

	/* verify IRQ access privileges */
	if (secure_int_default_check(src_id) != 0) {
		/* failed, directly return and disable the interrupt*/
		arc_int_disable(src_id);
		return 0;
	}

	/* get dst_id and interrupt handler */
	dst_id = secureshield_int_handler_table[src_id - NUM_EXC_CPU].id;
	handler = secureshield_int_handler_table[src_id - NUM_EXC_CPU].handler;

	/* check interrupt handler */
	if(!handler) {
		/* failed, directly return and disable the interrupt*/
		arc_int_disable(src_id);
		SECURESHIELD_DBG("Unprivileged handler for IRQ %d not found\r\n", src_id);
		return 0;
	}

	/* gather information from current state */
	src_id = g_container_stack_curr_id;

	src_frame->erbta = 0;

	/* a proper context switch is only needed when container changed */
	if(src_id != dst_id) {
		/* switch access control table */
		vmpu_switch(src_id, dst_id);
		/* save current state */
		container_stack_push(src_id, (uint32_t *)src_frame - ARC_CALLEE_FRAME_SIZE,
			(uint32_t *)_arc_aux_read(AUX_USER_SP), src_frame->status32, dst_id);
		/* gather information from current state */

		if (!container_is_secure(dst_id)) {
			dst_container_user_sp = g_container_context[dst_id].normal_sp;
		} else {
			dst_container_user_sp = 0;
		}

		_arc_aux_write(AUX_ERRET, (uint32_t)handler);
		_arc_aux_write(AUX_ERSTATUS, g_container_context[dst_id].cpu_status);

		return (uint32_t)g_container_context[dst_id].cur_sp;

	} else {
		if (container_is_secure(src_id)) {
			_arc_aux_write(AUX_MPU_EN, 0x40000000);
			arc_unlock_restore(src_frame->status32);
			handler(0);

			return 0;
		} else {
			container_stack_push(src_id, (uint32_t *)src_frame - ARC_CALLEE_FRAME_SIZE,
			(uint32_t *)_arc_aux_read(AUX_USER_SP), src_frame->status32, dst_id);
			_arc_aux_write(AUX_ERRET, (uint32_t)handler);
			_arc_aux_write(AUX_ERSTATUS, src_frame->status32);
			dst_container_user_sp = (uint32_t *)_arc_aux_read(AUX_USER_SP);

			return (uint32_t)g_container_context[dst_id].cur_sp;
		}
	}
}

/**
 * \brief interrupt return from normal world
 * \param[in] dst_frame   exception frame
 * \return  target container sp
 */
uint32_t secureshield_int_return(INT_EXC_FRAME *dst_frame)
{
	uint32_t src_id, dst_id;
	uint32_t *src_sp;


	/* discard the created cpu frame, recover the original sp of destination container */
	dst_id = g_container_stack_curr_id;

	if (container_stack_pop(dst_id, (uint32_t *)dst_frame + ARC_EXC_FRAME_SIZE,
		(uint32_t *)_arc_aux_read(AUX_USER_SP), dst_frame->status32) != 0 ) {
		return 0;
	}

	src_id = g_container_stack[g_container_stack_ptr].src_id;
	src_sp = g_container_stack[g_container_stack_ptr].src_sp;

	if (src_id != dst_id) {
		/* switch access control table */
		vmpu_switch(dst_id, src_id);
	}

	/* clear the first set bit in AUX_IRQ_ACT to simulate the quit of interrupt */
	//_arc_sr_reg(AUX_IRQ_ACT, _arc_lr_reg(AUX_IRQ_ACT) & ~(1 << _arc_lr_reg(AUX_IRQ_PRIORITY)));
	// Asm is more effective, it bitscan option is enabled */
	Asm(
	" lr %%r0, [%[irq_act]]\n"
	" ffs %%r1, %%r0\n"
	" bclr %%r0, %%r0, %%r1\n"
	" sr %%r0, [%[irq_act]]\n" :: [irq_act] "i" (AUX_IRQ_ACT));

	return (uint32_t) src_sp;
}

/**
 * \brief interrupt operation handler
 * \param[in] frame exception frame
 */
void secureshield_int_ops(INT_EXC_FRAME *frame)
{
	/* FIXME: remove switch case structure, use jump table */
	/* r0 is used as operation id */
	switch (frame->r0) {
		case SECURESHIELD_INT_EXC_GET:
			frame->r0 = (uint32_t)secure_int_handler_get(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_INSTALL:
			frame->r0 = secure_int_handler_install(frame->r1, (INT_HANDLER)frame->r2);
			break;
		case SECURESHIELD_INT_EXC_ENABLE:
			frame->r0 = secure_int_enable(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_ENABLED:
			frame->r0 = secure_int_enabled(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_DISABLE:
			frame->r0 = secure_int_disable(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_PRI_SET:
			frame->r0 = secure_int_pri_set(frame->r1, frame->r2);
			break;
		case SECURESHIELD_INT_EXC_PRI_GET:
			frame->r0 = secure_int_pri_get(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_PROBE:
			frame->r0 = secure_int_probe(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_LEVEL_CONFIG:
			frame->r0 = secure_int_level_config(frame->r1, frame->r2);
			break;
		case SECURESHIELD_INT_EXC_SW_TRIG:
			frame->r0 = secure_int_sw_trigger(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_CPU_LOCK:
			frame->r0 = 0;
			secure_cpu_lock();
			break;
		case SECURESHIELD_INT_EXC_CPU_UNLOCK:
			frame->r0 = 0;
			secure_cpu_unlock();
			break;
		default:
			SECURESHIELD_DBG("Unsupported interrupt operation:%d\r\n", frame->r0);
			frame->r0 = 0xffffffff;
			break;
	}
}