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
#ifndef _SECURESHIELD_INT_H_
#define _SECURESHIELD_INT_H_


typedef struct {
	INT_HANDLER handler;	/* interrupt handler function */
	uint8_t id;		/* the container it belongs to */
	uint8_t enabled;	/* enabled status */
} SECURESHIELD_INT_HANDLER;


extern EXC_ENTRY secureshield_entry_table[];
extern EXC_HANDLER secureshield_exc_handler_table[];
extern SECURESHIELD_INT_HANDLER secureshield_int_handler_table[];

extern int32_t secure_int_disable(uint32_t intno);
extern int32_t secure_int_enable(uint32_t intno);
extern int32_t secure_int_enabled(uint32_t intno);
extern int32_t secure_int_pri_set(uint32_t intno, int intpri);
extern int32_t secure_int_pri_get(uint32_t intno);
extern int32_t secure_int_handler_install(uint32_t intno, INT_HANDLER handler);
extern INT_HANDLER secure_int_handler_get(uint32_t intno);
extern int32_t secure_int_sw_trigger(uint32_t intno);
extern int32_t secure_int_probe(uint32_t intno);
extern int32_t secure_int_level_config(uint32_t intno, uint32_t level);
extern void secureshield_int_init(void);


#if SECURESHIELD_VERSION == 1
extern uint32_t secureshield_interrupt_handle(INT_EXC_FRAME *sp);
extern uint32_t secureshield_int_return(INT_EXC_FRAME *sp);
extern void secureshield_int_ops(INT_EXC_FRAME *frame);

#elif SECURESHIELD_VERSION == 2
extern void * secureshield_interrupt_handle(uint32_t *sp);
extern int32_t secureshield_int_ops(uint32_t ops, uint32_t par1, uint32_t par2);
#endif

#endif /* _SECURESHIELD_INT_H_ */


