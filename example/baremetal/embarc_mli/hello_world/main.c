/* ------------------------------------------
 * Copyright (c) 2021, Synopsys, Inc. All rights reserved.

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
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"
#include "mli_api.h"


/**
 * \brief	Test hardware board without any peripheral
 */
int main(void)
{
    EMBARC_PRINTF("************************************\n");
    int8_t data_in[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    mli_tensor in = { 0 };
    in.el_type = MLI_EL_FX_8;
    in.rank = 1;
    in.shape[0] = 8;
    in.mem_stride[0] = 1;
    in.data.capacity = sizeof(data_in);
    in.data.mem.void_p = data_in;

    int8_t data_out[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    mli_tensor out = { 0 };
    out.el_type = MLI_EL_FX_8;
    out.rank = 1;
    out.shape[0] = 8;
    out.mem_stride[0] = 1;
    out.data.capacity = sizeof(data_out);
    out.data.mem.void_p = data_out;

    mli_status status;
    status = mli_krn_eltwise_add_fx8(&in, &in, &out);
    EMBARC_ASSERT(status == MLI_STATUS_OK);
    for (int i = 0; i < sizeof(data_out)/sizeof(data_out[0]); i++) {
        EMBARC_PRINTF("%d ", ((int8_t*)(out.data.mem.void_p))[i]);
    }
    EMBARC_PRINTF("\n");

    status = mli_krn_eltwise_sub_fx8(&in, &in, &out);
    EMBARC_ASSERT(status == MLI_STATUS_OK);
    for (int i = 0; i < sizeof(data_out)/sizeof(data_out[0]); i++) {
        EMBARC_PRINTF("%d ", ((int8_t*)(out.data.mem.void_p))[i]);
    }
    EMBARC_PRINTF("\n");
    EMBARC_PRINTF("************************************\n");
	return E_SYS;
}
