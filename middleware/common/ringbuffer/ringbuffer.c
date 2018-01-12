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
 * \defgroup	MID_BUF_RINGBUFFER	Ringbuffer Middleware
 * \ingroup	MID_BUF
 * \brief	a ringbuffer middleware implemention.
 *	ringbuffer or called circular buffer is described in wikipedia as this
 *	<a href="http://en.wikipedia.org/wiki/Circular_buffer">Circular_buffer</a>.
 * @{
 *
 * \file
 * \brief	a ringbuffer implemention
 * \details	ringbuffer or called circular buffer is described in wikipedia as this
 *	<a href="http://en.wikipedia.org/wiki/Circular_buffer">Circular_buffer</a>.
 */
#include <stddef.h>

#include "ringbuffer.h"

#define	 DEBUG
#include "embARC_debug.h"

/**
 * \defgroup	MID_BUF_RBOP	Ringbuffer Operation API
 * \ingroup	MID_BUF
 * \brief	ringbuffer operation api called by other functions,
 *	like init, debug, empty & full detect, add/remove operation
 * @{
 */
/**
 * \brief	init ringbuffer buffer and size
 * \param[in]	rb	ringbuffer that you want to init
 * \param[in]	buf	buffer that ringbuffer will use
 * \param[in]	item_cnt	the buffer size in rb_buftype
 * \retval	0	ringbuffer init success
 * \retval	-1	ringbuffer passed arguments miss the requirement
 */
int rb_init(RINGBUFFER *rb, rb_buftype *buf, unsigned int item_cnt)
{
	if ( (rb==NULL) || (buf==NULL) || (item_cnt==0) ) {
		return -1;
	}

	rb->buf = buf;
	/** size convert from bytes to rb_buftype size */
	rb->size = item_cnt;
	rb->avail = item_cnt;
	rb->wridx = 0; /* index can count from 0 - size */
	rb->rdidx = 0;

	return 0;
}

/**
 * \brief	print ringbuffer info
 * \param[in]	rb	ringbuffer to query
 * \retval	0	ringbuffer init success
 * \retval	-1	ringbuffer passed arguments miss the requirement
 */
int rb_dbg(RINGBUFFER *rb)
{
	if ( rb == NULL ) return -1;

	DBG("size:%d, avail:%d, write index:%d, read index:%d\n\r",
		rb->size, rb->avail, rb->wridx, rb->rdidx);

	return 0;
}

/**
 * \brief	check whether ringbuffer is full
 * \param[in]	rb	ringbuffer to query
 * \retval	1	buffer was full
 * \retval	0	buffer was not full
 * \retval	-1	ringbuffer passed arguments miss the requirement
 */
int rb_isfull(RINGBUFFER *rb)
{
	if ( rb == NULL ) return -1;

	return ( rb->avail == 0 );
}

/**
 * \brief	check whether ringbuffer is empty
 * \param[in]	rb	ringbuffer to query
 * \retval	1	buffer was empty
 * \retval	0	buffer was not empty
 * \retval	-1	ringbuffer passed arguments miss the requirement
 */
int rb_isempty(RINGBUFFER *rb)
{
	if ( rb == NULL ) return -1;

	return (rb->avail == rb->size);
}

/**
 * \brief	add a new item(rb_buftype) to ringbuffer
 * \param[in]	rb	ringbuffer to operate
 * \param[in]	item	item to add
 * \retval	0	add item to buffer successfully
 * \retval	-1	buffer error, buffer was not valid
 * \retval	-2	buffer full
 */
int rb_add(RINGBUFFER *rb, rb_buftype item)
{
	int ercd = 0;

	ercd = rb_isfull(rb);
	if ( ercd == -1 ) { /* buffer error */
		ercd = -1;
	} else if ( ercd == 1 ) { /* buffer full */
		ercd = -2;
	} else if ( ercd == 0 ) { /* buffer not full */
		rb->wridx = rb->wridx % rb->size;  /* get the right write index 	*/
		rb->buf[rb->wridx] = item;	/* add new item */
		rb->wridx ++;			/* increase write index */
		rb->avail --;
		ercd = 0;
	}

	return ercd;
}

/**
 * \brief	remove one valid item(rb_buftype) from ringbuffer
 * \param[in]	rb	ringbuffer to operate
 * \param[out]	item	the removed item get from the ringbuffer
 * \retval	0	success
 * \retval	-1	ringbuffer error, buffer was not valid
 * \retval	-2	buffer full
 * \retval	-3	item not valid
 */
int rb_remove(RINGBUFFER *rb, rb_buftype *item)
{
	int ercd = 0;

	ercd = rb_isempty(rb);

	if ( ercd == -1 ) { /* buffer error */
		ercd = -1;
	} else if ( ercd == 1 ) { /* buffer empty */
		ercd = -2;
	} else if ( ercd == 0 ) { /* buffer not empty */
		if ( item == NULL ) { /* item not valid */
			ercd = -3;
		} else {
			rb->rdidx = rb->rdidx % rb->size;  /* get the right read index */
			*item = rb->buf[rb->rdidx];  /* remove new item */
			rb->rdidx ++;  /* increase read index */
			rb->avail ++;
			ercd = 0;
		}
	}

	return ercd;
}

/**
 * \brief	Push more items(rb_buftype) into ringbuffer
 * \param[in]	rb	ringbuffer to operate
 * \param[out]	items	the items need to pushed into ringbuffer
 * \retval	>=0	pushed size
 */
unsigned int rb_push(RINGBUFFER *rb, rb_buftype *items, unsigned int cnt)
{
	unsigned int wricnt = 0;

	while (wricnt < cnt) {
		if (rb_add(rb, items[wricnt]) == 0) {
			wricnt ++;
		} else {
			break;
		}
	}
	return wricnt;
}

/**
 * \brief	Pop more items(rb_buftype) from ringbuffer
 * \param[in]	rb	ringbuffer to operate
 * \param[out]	items	the items need to poped from ringbuffer
 * \retval	>=0	poped size
 */
unsigned int rb_pop(RINGBUFFER *rb, rb_buftype *items, unsigned int cnt)
{
	unsigned int reacnt = 0;

	while (reacnt < cnt) {
		if (rb_remove(rb, &items[reacnt]) == 0) {
			reacnt ++;
		} else {
			break;
		}
	}
	return reacnt;
}
/** @} end of group MID_BUF_RBOP */

/** @} end of group MID_BUF_RINGBUFFER */