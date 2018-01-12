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
#undef LIB_SECURESHIELD_OVERRIDES
#include "arc_builtin.h"
#include "arc_exception.h"
#include "secureshield_lib.h"

#if SECURESHIELD_VERSION == 1
uint32_t secureshield_arc_lr_reg(uint32_t addr)
{
	if (_arc_in_user_mode() == 0) {
		return _arc_lr_reg(addr);
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_SYS, "", SECURESHIELD_SYS_LR, addr);
	}
}

void secureshield_arc_sr_reg(uint32_t addr, uint32_t val)
{
	if (_arc_in_user_mode() == 0) {
		_arc_sr_reg(addr, val);
	} else {
		SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_SYS, "", SECURESHIELD_SYS_SR, addr, val);
	}
}

int32_t secureshield_container_id_self(void)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_SYS, "", SECURESHIELD_SYS_CONTAINER_ID_SELF);
}

int32_t secureshield_container_id_caller(void)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_SYS, "", SECURESHIELD_SYS_CONTAINER_ID_CALLER);
}
#elif SECURESHIELD_VERSION == 2

uint32_t secureshield_arc_lr_reg(uint32_t addr)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_SYS, "", SECURESHIELD_SYS_LR, addr);
}

void secureshield_arc_sr_reg(uint32_t addr, uint32_t val)
{
	SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_SYS, "", SECURESHIELD_SYS_SR, addr, val);
}


int32_t secureshield_container_id_self(void)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_SYS, "", SECURESHIELD_SYS_CONTAINER_ID_SELF);
}

int32_t secureshield_container_id_caller(void)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_SYS, "", SECURESHIELD_SYS_CONTAINER_ID_CALLER);
}

static void temp_trap_handler(void)
{

}

/**
 * \brief
 * The exception state registers (ERET, ECR, ERSTATUS, ERBTA, EFA, ESYN) will be protected by a new internal bit.
 * This will be set on exception entry if the exception is triggered on a secure mode instruction and cleared on
 * exception entry if the exception is triggered on a non-secure instruction. If set, this bit will prevent
 * read/write access to these registers from non-secure mode. Any attempt to access these registers in NS mode
 * while protected will result in an EV_PriviligeV exception.
 *
 * this function is used to clear this bit.
 */
void secureshield_except_bit_clear(void)
{
    EXC_HANDLER prev_trap_handler;

    exc_nest_count = 1;
    prev_trap_handler = exc_handler_get(EXC_NO_TRAP);
    exc_handler_install(EXC_NO_TRAP, (EXC_HANDLER)temp_trap_handler);
    Asm("trap_s 0");
    exc_nest_count = 0;
    exc_handler_install(EXC_NO_TRAP, prev_trap_handler);
}

#endif