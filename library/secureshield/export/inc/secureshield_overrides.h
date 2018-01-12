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
//#ifndef _SECURESHIELD_OVERRIDES_H_
//#define _SECURESHIELD_OVERRIDES_H_

#include "secureshield_int_exports.h"
#include "secureshield_sys_ops_exports.h"

#ifdef OVERRIDE_ARC_HAL_EXCEPTION_H
#undef int_disable
#undef int_enable
#undef int_pri_set
#undef int_pri_get
#undef int_handler_install
#undef int_handler_get
#undef int_sw_trigger
#undef int_probe
#undef int_level_config
#undef cpu_lock
#undef cpu_unlock
#undef cpu_lock_restore

#define int_disable(intno)			secureshield_int_disable(intno)
#define int_enable(intno)			secureshield_int_enable(intno)
#define int_pri_set(intno, intpri)		secureshield_int_pri_set(intno, intpri)
#define int_pri_get(intno)			secureshield_int_pri_get(intno)
#define int_handler_install(intno, handler)	secureshield_int_handler_install(intno, handler)
#define int_handler_get(intno)			secureshield_int_handler_get(intno)
#define int_sw_trigger(intno)			secureshield_int_sw_trigger(intno)
#define int_probe(intno)			secureshield_int_probe(intno)
#define int_level_config(intno, level)		secureshield_int_level_config(intno, level)
#define cpu_lock()				secureshield_cpu_lock()
#define cpu_unlock()				secureshield_cpu_unlock()
#define cpu_lock_save()				secureshield_cpu_lock_save()
#define cpu_unlock_restore(status)		secureshield_cpu_unlock_restore(status)

#endif /* OVERRIDE_ARC_HAL_EXCEPTION_H */

#ifdef OVERRIDE_ARC_HAL_BUILTIN_H
#undef _arc_lr_reg
#undef _arc_sr_reg
#define _arc_lr_reg(reg)			secureshield_arc_lr_reg(reg)
#define _arc_sr_reg(reg, val)			secureshield_arc_sr_reg(reg,val)
#endif /* OVERRIDE_ARC_HAL_BUILTIN_H */

//#endif	/* _SECURESHIELD_OVERRIDES_H_ */