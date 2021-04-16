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
* Library DW_DFSS-1.1.6
* ========================================================================== */

#ifndef PDM_RX_H_
#define PDM_RX_H_

#include "ioctl.h"

/* PDM Rx IO control commands */
#define IO_SET_CB_CLIP                  (0x20)

#define IO_PDM_RX_SET_RER0              (0x21)  // enable/disable stereo channel #0
#define IO_PDM_RX_SET_RER1              (0x22)  // enable/disable stereo channel #1
#define IO_PDM_RX_SET_RER2              (0x23)  // enable/disable stereo channel #2
#define IO_PDM_RX_SET_RER3              (0x24)  // enable/disable stereo channel #3

#define IO_PDM_RX_CHAN_ENABLE           (0x30)  // alternative command for channel enable
#define IO_PDM_RX_CHAN_DISABLE          (0x31)  // alternative command for channel disable

#define IO_PDM_RX_SET_REN               (0x25)  // set receive enable bit (cannot clear this bit, argument is ignored)
#define IO_PDM_RX_SET_RR                (0x26)  // set oversampling ratio
#define IO_PDM_RX_SET_CIC_N             (0x27)  // set number of stages
#define IO_PDM_RX_SET_CIC_D             (0x28)  // set CIC delay
#define IO_PDM_RX_SET_DCRC              (0x29)  // DC removal control
#define IO_PDM_RX_SET_BRC_B0            (0x2a)  // BRC bit range shift, channel 0
#define IO_PDM_RX_SET_BRC_B1            (0x2b)  // BRC bit range shift, channel 1
#define IO_PDM_RX_SET_BRC_B2            (0x2c)  // BRC bit range shift, channel 2
#define IO_PDM_RX_SET_BRC_B3            (0x2d)  // BRC bit range shift, channel 3
#define IO_PDM_RX_SET_BRC_CLP           (0x2e)  // BRC clipping detection (CLPS = step & CLPT = threshold)
#define IO_PDM_RX_SET_FIFO_THRESHOLD    (0x2f)  // FIFO threshold level

extern uint32_t io_pdm_rx_open(uint32_t dev_id);
extern void io_pdm_rx_close(uint32_t dev_id);

/* Function:         io_pdm_rx_read
 * Parameters:
 *   dev_id [In]     Identifier of PDM device instance.
 *   data   [In]     Address of input buffer where requested number of acquired (stereo) samples shall be stored.
 *   size   [In/Out] Address of variable that holds input buffer size specifying the number of (stereo) samples to be acquired.
 *                   On invocation of callback, registerd by IO_SET_CB_RX command, the address contains the number
 *                   of (stereo) samples actually acquired.
 * Returns:
 *                   none
 * Notes:
 *                   Data buffer length and data size shall be calculated considering the number of PDM channels enabled.
 */
extern void io_pdm_rx_read(uint32_t dev_id, uint32_t * data,
               uint32_t * size);

/* PDM valid IOCTLs
  cmd                           arg type        arg value
  IO_SET_CB_RX                  io_cb_t         Callback function invoked from ISR whenever a series of samples
                                                is available in the buffer provided by io_pdm_rx_read
  IO_SET_CB_ERR                 io_cb_t         Callback function invoked from ISR whenever input buffer overflow condition whould occur
  IO_PDM_RX_SET_FIFO_THRESHOLD  uint32_t        threshold value for the Rx FIFO (accepts command before first io_pdm_rx_read)
*/
extern void io_pdm_rx_ioctl(uint32_t dev_id, uint32_t cmd, void *arg);

#endif              /* PDM_RX_H_ */
