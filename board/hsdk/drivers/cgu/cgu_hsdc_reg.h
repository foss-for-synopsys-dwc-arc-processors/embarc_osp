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
#ifndef __CGU_HSDC_REG_H__
#define __CGU_HSDC_REG_H__

/* CGU bit definitions */

//Ctrl
#define CGU_HSDC_CTRL_PD             0x00000001U //Power Down
#define CGU_HSDC_CTRL_PD_POS         0
#define CGU_HSDC_CTRL_BYPASS         0x00000002U //Bypass
#define CGU_HSDC_CTRL_BYPASS_POS     1
#define CGU_HSDC_CTRL_OD             0x0000000CU //Output divider
#define CGU_HSDC_CTRL_OD_POS         2
#define CGU_HSDC_CTRL_R              0x000001F0U //Input divider
#define CGU_HSDC_CTRL_R_POS          4
#define CGU_HSDC_CTRL_F              0x0000FE00U //Feedback divider
#define CGU_HSDC_CTRL_F_POS          9
#define CGU_HSDC_CTRL_BS             0x00010000U //Band selection
#define CGU_HSDC_CTRL_BS_POS         20

//Status
#define CGU_HSDC_STS_PLL_LOCK        0x00000001U //1=in lock
#define CGU_HSDC_STS_PLL_ERROR       0x00000002U //1=illegal programming PLL

//Measure
#define CGU_HSDC_FMEAS_RCNT          0x00007FFFU //Reference counter
#define CGU_HSDC_FMEAS_FCNT          0x3FFF8000U //Reference counter
#define CGU_HSDC_FMEAS_DONE          0x40000000U //1=measurement done
#define CGU_HSDC_FMEAS_START         0x80000000U //1=start measurement

//Monitor
#define CGU_HSDC_MON_N               0x000000FFU //Divider-by-N

//Idivs
#define CGU_HSDC_IDIV_N              0x000000FFU //Divider-by-N
//Sub
#define CGU_HSDC_SUB_IDIV_N0         0x0000000FU
#define CGU_HSDC_SUB_IDIV_N1         0x000000F0U
#define CGU_HSDC_SUB_IDIV_N2         0x00000F00U
#define CGU_HSDC_SUB_IDIV_N3         0x0000F000U

//Reset
#define CGU_HSDC_SW_RESET            0x00000001U

#define CGU_HSDC_SW_RESET_DELAY_MASK 0xFFFF0000U
#define CGU_HSDC_SW_RESET_DELAY_POS  16

//Info
#define CGU_HSDC_MINOR_REV           0x0000FFFFU
#define CGU_HSDC_MAJOR_REV           0xFFFF0000U
#define CGU_HSDC_TYPE                0x0000FFFFU
#define CGU_HSDC_DW                  0xFFFF0000U

#include "cgu_hsdc_reg_map.h"

#endif //__CGU_HSDC_REG_H__

