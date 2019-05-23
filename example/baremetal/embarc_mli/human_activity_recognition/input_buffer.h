/* ------------------------------------------
 * Copyright (c) 2019, Synopsys, Inc. All rights reserved.

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
#ifndef _INPUT_BUFFER_H_
#define _INPUT_BUFFER_H_

#include "mpu9250.h"

/* Values position inside the input buffer */
#define ACCL_BODY_X     0
#define ACCL_BODY_Y     1
#define ACCL_BODY_Z     2
#define GYRO_X          3
#define GYRO_Y          4 
#define GYRO_Z          5
#define ACCL_TOTAL_X    6
#define ACCL_TOTAL_Y    7
#define ACCL_TOTAL_Z    8

extern void 	input_buffer_init(void);
extern void 	input_buffer_push_data(MPU9250_DATA_PTR mp_data);
extern float*	input_buffer_get_next_buffer(void);

#endif /* _INPUT_BUFFER_H_ */