/* ==========================================================================
* Synopsys DesignWare Sensor and Control IP Subsystem IO Software Driver and
* documentation (hereinafter, "Software") is an Unsupported proprietary work
* of Synopsys, Inc. unless otherwise expressly agreed to in writing between
* Synopsys and you.
*
* The Software IS NOT an item of Licensed Software or Licensed Product under
* any End User Software License Agreement or Agreement for Licensed Product
* with Synopsys or any supplement thereto. You are permitted to use and
* redistribute this Software in source and binary forms, with or without
* modification, provided that redistributions of source code must retain this
* notice. You may not view, use, disclose, copy or distribute this file or
* any information contained herein except pursuant to this license grant from
* Synopsys. If you do not agree with this notice, including the disclaimer
* below, then you are not authorized to use the Software.
*
* THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
* ========================================================================== */
/*==========================================================================
* Library DW_DFSS-2.1.13
* ========================================================================== */

#ifndef DAC_MASTER_H_
#define DAC_MASTER_H_


#include "device/ip_hal/ioctl.h"


/* DAC control commands */
#define IO_DAC_SET_DAC_ENA          (0x20)
#define IO_DAC_SET_THRESHOLD        (0x21)
#define IO_DAC_SET_TX_EDGE          (0x22)

extern uint32_t io_dac_open(uint32_t dev_id);
extern void io_dac_close(uint32_t dev_id);
extern void io_dac_write(uint32_t dev_id, uint32_t * data,
             uint32_t * size);
/* DAC valid IOCTLs
  cmd                           arg type        arg value
  IO_SET_CB_TX                  io_cb_t         Callback function invoked from ISR when write successfully finishes
  IO_SET_CB_ERR                 io_cb_t         Callback function invoked from ISR when read/write fails
  IO_DAC_SET_DAC_ENA            uint32_t        0 - disable DAC, 1 - enable DAC
  IO_DAC_SET_THRESHOLD          uint32_t        threshold, 0..63
  IO_DAC_SET_TX_EDGE            uint32_t        0 - rising edge, 1 - falling edge
*/
extern void io_dac_ioctl(uint32_t dev_id, uint32_t cmd, void *arg);


#endif              /* DAC_MASTER_H_ */
