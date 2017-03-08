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
 * \file mtarch.c
 * \brief the target dependent part of multi-threading library
 */

#include "sys/mt.h"

unsigned int * mtarch_sp_next;			/* the sp of next to run thread */
struct mtarch_thread *mtarch_running;	/* current running thread */


/**
 * \brief  init the target dependent part of multi-threading library
 */
void mtarch_init(void)
{

}

/**
 * \brief  start a child thread
 *
 * \param[in] t	the child thread
 * \param[in] function	the child thread body
 * \param[in] data	thread body parameters
 */
void mtarch_start(struct mtarch_thread *t, void (*function)(void *), void *data){
	unsigned int * p_stk = &t->stack[MTARCH_STACKSIZE];

	*--p_stk = (unsigned int)data;
	*--p_stk = (unsigned int)function;
	*--p_stk = (unsigned int)mt_exit; /* blink */
	*--p_stk = (unsigned int)start_r; /* thread entry */
	t->sp = p_stk;
}

/**
 * \brief execute a child thread
 *
 * \param[in] t the child thread
 */
void mtarch_exec(struct mtarch_thread *t)
{
	mtarch_running = t;
	dispatch();
	mtarch_running = NULL;
}

/**
 * \brief  give up the cpu
 */
void mtarch_yield(void)
{
	dispatch();
}

/**
 * \brief  remove
 */
void mtarch_remove(void)
{

}

/**
 * \brief hook
 */
void mtarch_pstop(void)
{

}

/**
 * \brief hook
 */
void mtarch_pstart(void)
{

}

/**
 * \brief stop the specific thread
 *
 * \param thread
 */
void mtarch_stop(struct mtarch_thread *thread)
{

}

/**
 * \brief  get the stack usage of thread (unused now)
 *
 * \param[in] t thread
 *
 * \returns used stack
 */
int mtarch_stack_usage(struct mt_thread *t)
{
#if 0
	unsigned int i;

	for(i = 0; i < MTARCH_STACKSIZE; ++i) {
		if(t->thread.stack[i] != i) {
			return MTARCH_STACKSIZE - i;
		}
	}
#endif
	return MTARCH_STACKSIZE;
}

