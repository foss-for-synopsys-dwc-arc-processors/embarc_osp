/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/** Copyright (C) 1989-2013 Synopsys, Inc.                              **/
/** All Rights Reserved.                                                **/
/**                                                                     **/
/** SYNOPSYS CONFIDENTIAL                                               **/
/**                                                                     **/
/** This is an unpublished proprietary work of Synopsys, Inc., and is   **/
/** fully protected under copyright and trade secret laws. You may not  **/
/** view, use, disclose, copy, or distribute this file or any           **/
/** information contained herein except pursuant to a valid written     **/
/** license from Synopsys.                                              **/
/**                                                                     **/
/** For more information, contact                                       **/
/** est-adm@synopsys.com or call (650) 584-1631                         **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/
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

