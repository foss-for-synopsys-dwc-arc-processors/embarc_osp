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

/**
 * \addtogroup	MID_BUF_RINGBUFFER
 * @{
 *
 * \file	ringbuffer.h
 * \brief	a ringbuffer implemention header file
 * \details	ringbuffer or called circular buffer is described in wikipedia as this
 *	<a href="http://en.wikipedia.org/wiki/Circular_buffer">Circular_buffer</a>.
 *
 *
 */
#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup	MID_BUF_RBDT		Ringbuffer Data Types and Structure
 * \ingroup	MID_BUF_RINGBUFFER
 * \brief	ringbuffer data types and structure definitions
 * @{
 */
typedef  char	rb_buftype;		/*!< 8 bit integer typedef */

/**
 * @brief 	ringbuffer structure
 */
typedef struct ringbuffer
{
	rb_buftype *buf;			/*!< ringbuffer buffer pool 	*/
	volatile unsigned int wridx;		/*!< ringbuffer write index   */
	volatile unsigned int rdidx;		/*!< ringbuffer read index 	*/
	volatile unsigned int avail;		/*!< ringbuffer available size */
	volatile unsigned int size;		/*!< ringbuffer buffer size 	*/
} RINGBUFFER;
/** @} end of group MID_BUF_RBDT */

extern int rb_init(RINGBUFFER *rb, rb_buftype *buf, unsigned int item_cnt);
extern int rb_dbg(RINGBUFFER *rb);
extern int rb_isfull(RINGBUFFER *rb);
extern int rb_isempty(RINGBUFFER *rb);
extern int rb_add(RINGBUFFER *rb, rb_buftype item);
extern int rb_remove(RINGBUFFER *rb, rb_buftype *item);
extern unsigned int rb_push(RINGBUFFER *rb, rb_buftype *items, unsigned int cnt);
extern unsigned int rb_pop(RINGBUFFER *rb, rb_buftype *items, unsigned int cnt);

#ifdef __cplusplus
}
#endif

#endif /* _RINGBUFFER_H_ */
/** @} end of group MID_BUF_RINGBUFFER */