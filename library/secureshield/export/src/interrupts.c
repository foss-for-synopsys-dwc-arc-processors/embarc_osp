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
#include "arc_exception.h"
#include "secureshield_lib.h"

#if SECURESHIELD_VERSION == 1
int32_t secureshield_int_disable(uint32_t intno)
{
	if (_arc_in_user_mode() == 0) {
		return int_disable(intno);
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_DISABLE, intno);
	}
}

int32_t secureshield_int_enable(uint32_t intno)
{
	if (_arc_in_user_mode() == 0) {
		return int_enable(intno);
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_ENABLE, intno);
	}
}

int32_t secureshield_int_enabled(uint32_t intno)
{
	if (_arc_in_user_mode() == 0) {
		return int_enabled(intno);
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_ENABLED, intno);
	}
}

int32_t secureshield_int_pri_set(uint32_t intno, int32_t intpri)
{
	if (_arc_in_user_mode() == 0) {
		return int_pri_set(intno,intpri);
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_PRI_SET, intno, intpri);
	}
}

int32_t secureshield_int_pri_get(uint32_t intno)
{
	if (_arc_in_user_mode() == 0) {
		return int_pri_get(intno);
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_PRI_GET, intno);
	}
}

int32_t secureshield_int_handler_install(uint32_t intno, void* handler)
{
	if (_arc_in_user_mode() == 0) {
		return int_handler_install(intno, handler);
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_INSTALL, intno, handler);
	}
}

void * secureshield_int_handler_get(uint32_t intno)
{
	if (_arc_in_user_mode() == 0) {
		return int_handler_get(intno);
	} else {
		return (void *)SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_GET, intno);
	}
}

int32_t secureshield_int_sw_trigger(uint32_t intno)
{
	if (_arc_in_user_mode() == 0) {
		return int_sw_trigger(intno);
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_SW_TRIG, intno);
	}
}

int32_t secureshield_int_probe(uint32_t intno)
{
	if (_arc_in_user_mode() == 0) {
		return int_probe(intno);
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_PROBE, intno);
	}
}

int32_t secureshield_int_level_config(uint32_t intno, uint32_t level)
{
	if (_arc_in_user_mode() == 0) {
		return int_level_config(intno, level);
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_LEVEL_CONFIG, intno, level);
	}
}

void secureshield_cpu_lock(void)
{
	if (_arc_in_user_mode() == 0) {
		cpu_lock();
	} else {
		SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_CPU_LOCK);
	}
}

void secureshield_cpu_unlock(void)
{
	if (_arc_in_user_mode() == 0) {
		cpu_lock();
	} else {
		SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_CPU_UNLOCK);
	}
}

uint32_t secureshield_cpu_lock_save(void)
{
	if (_arc_in_user_mode() == 0) {
		return cpu_lock_save();
	} else {
		return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_CPU_LOCK);
	}
}

void secureshield_cpu_unlock_restore(uint32_t status)
{
	if (_arc_in_user_mode() == 0) {
		cpu_unlock_restore(status);
	} else {
		SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_CPU_UNLOCK);
	}
}
#elif SECURESHIELD_VERSION == 2
int32_t secureshield_int_disable(uint32_t intno)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_DISABLE, intno);
}

int32_t secureshield_int_enable(uint32_t intno)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_ENABLE, intno);
}

int32_t secureshield_int_enabled(uint32_t intno)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_ENABLED, intno);
}

int32_t secureshield_int_pri_set(uint32_t intno, int32_t intpri)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_PRI_SET, intno, intpri);
}

int32_t secureshield_int_pri_get(uint32_t intno)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_PRI_GET, intno);
}

int32_t secureshield_int_handler_install(uint32_t intno, void * handler)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_INSTALL, intno, handler);
}

void * secureshield_int_handler_get(uint32_t intno)
{
	return (void *)SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_GET, intno);
}

int32_t secureshield_int_sw_trigger(uint32_t intno)
{

	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_SW_TRIG, intno);
}

int32_t secureshield_int_probe(uint32_t intno)
{

	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_PROBE, intno);
}

int32_t secureshield_int_level_config(uint32_t intno, uint32_t level)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_LEVEL_CONFIG, intno, level);
}

void secureshield_cpu_lock(void)
{
	SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_CPU_LOCK);
}

void secureshield_cpu_unlock(void)
{
	SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_CPU_UNLOCK);
}

uint32_t secureshield_cpu_lock_save(void)
{
	return SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_CPU_LOCK);
}

void secureshield_cpu_unlock_restore(uint32_t status)
{
	SECURESHIELD_SECURE_CALL(SECURESHIELD_SECURE_CALL_INT_EXC, "", SECURESHIELD_INT_EXC_CPU_UNLOCK);
}
#endif