/*
 * Copyright (c) 2014, Swedish Institute of Computer Science.
 *
 * Copyright (c) 2017, Synopsys, Inc.
 * Modified for port to ARC processor by Wayne Ren wei.ren@synopsys.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file mtarch-asm.s
 * \brief thread switch support of contiki multi-thread library
 */

#include "arc.h"
#include "arc_asm_common.h"

	.text
	.align 4
	.global start_r
	.global dispatch

dispatch:
/*
 * here a trick is used, mtarch_sp_next points to the sp of next to run thread
 * the sp of mtarch_running is used to save old sp
 * context switch of threads just happened between contiki thread and childr
 * thread.
 */
	ld	r0, [mtarch_running]
	ld	r1, [r0]
	st	r1, [mtarch_sp_next]
	SAVE_NONSCRATCH_REGS
	mov	r0, dispatch_r
	PUSH	r0
	ld	r0, [mtarch_running]	/* save old sp */
	st	sp, [r0]
	ld	sp, [mtarch_sp_next]	/* load new sp */
	POP	r0
	j	[r0]

dispatch_r:
	RESTORE_NONSCRATCH_REGS
	j	[blink]

/* the entry of child thread */
start_r:
	POP	blink
	POP	r1
	POP	r0
	j	[r1]
