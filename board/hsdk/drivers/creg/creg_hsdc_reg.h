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
#ifndef _CREG_HSDC_REG_H_
#define _CREG_HSDC_REG_H_

/* CREG bit definitions */
#define VERSION_REF               0x0000000CU //2 bits
#define VERSION_REF_POS           2

//FPGA version
#define VERSION_DAY               0x0000001FU //5 bits
#define VERSION_DAY_POS           0
#define VERSION_MONTH             0x000001E0U //4 bits
#define VERSION_MONTH_POS         5
#define VERSION_YEAR              0x001FFE00U //12 bits
#define VERSION_YEAR_POS          9
#define VERSION_TIME_MIN          0x07E00000U //6 bits
#define VERSION_TIME_MIN_POS      21
#define VERSION_TIME_HOUR         0xF8000000U //5 bits
#define VERSION_TIME_HOUR_POS     27

//start register
#define START_MASK               (0x0000000FU)
#define START_POS                0

#define START_MODE_MASK          (0x00000010U)
#define START_MODE_POS           4

#define POLARITY_MASK            (0x00000100U)
#define POLARITY_POS             8

#define CORE_SEL_MASK            (0x00000600U)
#define CORE_SEL_POS             9

#define MULTI_CORE_MASK          (0x00003000U)
#define MULTI_CORE_POS           12

#define DEBUG_UART_MODE_MASK     (0x00010000U)
#define DEBUG_UART_MODE_POS      16

//sw register
#define RESET_MASK               (0x00000001U)
#define RESET_POS                0

#include "creg_hsdc_reg_map.h"

#endif /* _CREG_HSDC_REG_H_ */

