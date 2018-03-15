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
#include "secureshield_sys.h"
#include "secureshield_container_stack.h"
#include "secureshield_secure_call_exports.h"


/* max aux entries in container configurations */

#ifndef CONTAINER_AUX_COUNT
#define CONTAINER_AUX_COUNT		8
#endif

/* the aux reg address allocated to a normal container */
typedef struct {
	uint32_t start;	/* start address */
	uint32_t end;	/* end address */
} CONTAINER_AUX;

typedef struct {
	CONTAINER_AUX *aux; /* the start pointer to  container_aux table */
	uint32_t count;	/* the number of container_aux table items owned to the container */
} AUX_CONTAINER;


/* current used aux entries */
static uint32_t g_aux_count;
/* the list to manage the access to aux regs from normal containers */
static CONTAINER_AUX g_aux_list[CONTAINER_AUX_COUNT];
static AUX_CONTAINER g_aux_container[SECURESHIELD_MAX_CONTAINERS];


/**
 * \brief check whether the specified container is able to access the specified aux address
 * \param[in] container_id  container id
 * \param[in] addr         address in aux register space
 */
static int32_t _secureshield_aux_check(uint8_t container_id, uint32_t addr)
{
	uint32_t i;
	AUX_CONTAINER *container;
	CONTAINER_AUX *aux;

	if (container_id >= SECURESHIELD_MAX_CONTAINERS) {
		SECURESHIELD_DBG("container_id out of range\r\n");
		return -1;
	}

	container = &g_aux_container[container_id];

	aux = &container->aux[0];

	for (i = 0; i < container->count; i++) {
		if (addr >= aux->start && addr < aux->end) {
			return 0;
		}
		aux++;
	}

	/* if aux is not found, try to search in container 0 which is background container */
	container = &g_aux_container[0];
	aux = &container->aux[0];

	if (!container->aux) {
		SECURESHIELD_DBG("no aux region is allocated for container 0 and %d\r\n",
				 container_id);
		return -1;
	}

	for (i = 0; i < container->count; i++) {
		if (addr >= aux->start && addr < aux->end) {
			return 0;
		}
		aux++;
	}

	SECURESHIELD_DBG("the aux regs:0x%x is not allocated to container 0 and %d\r\n",
			 addr, container_id);
	return -1;
}


/**
 * \brief read an aux register
 * \param[in]  addr the aux reg address
 * \return  aux register value (ok) or 0xffffffff (failed)
 */
uint32_t secure_arc_lr_reg(uint32_t addr)
{
	if (_secureshield_aux_check(g_active_container, addr) != 0 ) {
		return 0xffffffff;
	}

	return _arc_lr_reg(addr);
}

/**
 * write an aux register
 * \param[in] addr the aux reg address
 * \param[in] val  value to write
 */
void secure_arc_sr_reg(uint32_t addr, uint32_t val)
{
	if (_secureshield_aux_check(g_active_container, addr) == 0) {
		_arc_sr_reg(addr, val);
	}
}


/**
 * \brief allocate an aux region to the specified container
 * \param[in]  container_id container id
 * \param[in]  start        start address
 * \param[in]  size         address size
 * \return     0 ok, -1 failed
 */
int32_t vmpu_ac_aux(uint8_t container_id, uint32_t start, uint32_t size)
{
	AUX_CONTAINER *container;
	CONTAINER_AUX *aux;

	if (g_aux_count >= CONTAINER_AUX_COUNT) {
		SECURESHIELD_DBG("no more place for aux region\r\n");
		return -1;
	}

	if (container_id >= SECURESHIELD_MAX_CONTAINERS) {
		SECURESHIELD_DBG("container id out of range\r\n");
		return -1;
	}

	SECURESHIELD_DBG("\tcontainer [%d] has aux regs area from @0x%x with size %d\r\n",
		container_id, start, size);

	/* assign aux container pointer */
	container = &g_aux_container[container_id];

	if (!container->aux) {
		container->aux = &g_aux_list[g_aux_count];
	}

	/* allocate new aux region */
	aux = &container->aux[container->count];

	aux->start = start;
	aux->end = start + size;

	/* take account for new aux region */
	container->count++;
	g_aux_count++;

	return 0;
}

/**
 * \brief get current container's id
 * \return  container id
 */
int32_t secure_container_id_self(void)
{
	return g_active_container;
}


/**
 * \brief get the id of caller container's id
 * \return  caller container's id
 */
int32_t secure_container_id_caller(void)
{
	if (g_container_stack_ptr < 1) {
		/* no container call in stack */
		return -1;
	}

	/* how about the case of container switch caused by IRQ */

	return g_container_stack[g_container_stack_ptr - 1].src_id;
}

#if SECURESHIELD_VERSION == 1
/**
 * \brief system service provided by secureshield runtime
 * \param[in] frame exception frame
 */
void secureshield_sys_ops(INT_EXC_FRAME *frame)
{
	/* FIXME: remove switch case structure, use jump table */
	/* r0 is used as operation id */
	switch (frame->r0) {
		case SECURESHIELD_SYS_LR:
			SECURESHIELD_DBG("read aux reg 0x%x\r\n", frame->r1);
			frame->r0 = secure_arc_lr_reg(frame->r1);
			break;
		case SECURESHIELD_SYS_SR:
			SECURESHIELD_DBG("write aux reg 0x%x\r\n", frame->r1);
			secure_arc_sr_reg(frame->r1, frame->r2);
			break;
		case SECURESHIELD_SYS_CONTAINER_ID_SELF:
			frame->r0 = secure_container_id_self();
			break;
		case SECURESHIELD_SYS_CONTAINER_ID_CALLER:
			frame->r0 = secure_container_id_caller();
			break;
		default:
			SECURESHIELD_DBG("Unsupported sys level operation:%d\r\n", frame->r0);
			break;
	}
}

#elif SECURESHIELD_VERSION == 2
/**
 * \brief secureshield runtime service
 * \param[in]  sys_id system service id
 * \param[in]  par1   1st parameter
 * \param[in]  par2   2nd parameter
 * \param[in]  par3   3rd parameter
 * \param[in]  par4   4th parameter
 * \return        return value of runtime service
 */
int32_t secureshield_sys_ops(uint32_t sys_id, uint32_t par1, uint32_t par2, uint32_t par3, uint32_t par4)
{
	int32_t ret = -1;

	switch (sys_id) {
		case SECURESHIELD_SYS_LR:
			SECURESHIELD_DBG("read aux reg 0x%x\r\n", par1);
			ret =  secure_arc_lr_reg(par1);
			break;
		case SECURESHIELD_SYS_SR:
			SECURESHIELD_DBG("write aux reg 0x%x\r\n", par1);
			secure_arc_sr_reg(par1, par2);
			break;
		case SECURESHIELD_SYS_CONTAINER_ID_SELF:
			ret = secure_container_id_self();
			break;
		case SECURESHIELD_SYS_CONTAINER_ID_CALLER:
			ret = secure_container_id_caller();
			break;
		default:
			SECURESHIELD_DBG("Unsupported sys level operation:%d\r\n", sys_id);
			break;
	}
	return ret;
}
#endif


