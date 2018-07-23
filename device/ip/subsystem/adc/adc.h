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

#ifndef ADC_H_
#define ADC_H_

#include "ioctl.h"


/* ADC control commands */
#define IO_ADC_SET_CLK_DIVIDER          (0x20)
#define IO_ADC_SET_CONFIG               (0x21)
#define IO_ADC_SET_SEQ_TABLE            (0x22)
#define IO_ADC_SET_SEQ_MODE             (0x23)
#define IO_ADC_SET_SEQ_STOP             (0x24)
#define IO_ADC_SET_RX_THRESHOLD         (0x25)
#define IO_ADC_SET_ADC_ENA              (0x26)

#define IO_ADC_INPUT_SINGLE_END       0
#define IO_ADC_INPUT_DIFF             1
#define IO_ADC_OUTPUT_PARAL           0
#define IO_ADC_OUTPUT_SERIAL          1
#define IO_ADC_CAPTURE_RISING_EDGE    0
#define IO_ADC_CAPTURE_FALLING_EDGE   1

#define IO_ADC_SEQ_MODE_SINGLESHOT    0
#define IO_ADC_SEQ_MODE_REPETITIVE    1

/* Structure representing AD converter configuration */
typedef struct {
    uint32_t in_mode;   /* ADC input mode: single ended or differential */
    uint32_t out_mode;    /* ADC output mode: parallel or serial */
    uint32_t serial_dly;  /* Number of adc_clk the first bit of serial output is delayed for after start of convertion */
    uint32_t capture_mode;  /* ADC controller capture mode: by rising or falling edge of adc_clk */
    uint32_t sample_width;  /* ADC sample width */
} io_adc_config_t;


/* Structure representing ADC sequence table entry. */
typedef struct {
    uint32_t sample_dly;  /* delay to be introduced prior to start of convertion, in terms of adc clocks */
    uint32_t channel_id;  /* ADC input associated with the entry */
} io_adc_seq_entry_t;


/* Structure representing ADC sequence table. */
typedef struct {
    io_adc_seq_entry_t *entries;
    uint32_t num_entries;
} io_adc_seq_table_t;


extern uint32_t io_adc_open(uint32_t dev_id);
extern void io_adc_close(uint32_t dev_id);

/* Function: io_adc_read
 * Initiate acquisition of samples from a series of ADC inputs specified by IO_ADC_SET_SEQ_TABLE
 * control command. In case the ADC is configured to operate in repetitive mode then subsequent calls do not change hardware status,
 * but are expected to provide data buffers to store acquired samples. It is up to application to issue subsequent io_adc_read calls
 * promptly in this case. Failure to do that causes overflow condition and callback function registered by IO_SET_CB_ERR is called
 * from ISR with ADC instance identifier as its argument. Upon successful acquisition of specified number of samples the callback
 * function registered by IO_SET_CB_RX command is called with address of ready buffer passed as its argument.
 *
 * Parameters:
 *   dev_id [In]   Identifier of ADC device instance.
 *   data [In]     Address of input buffer where requested number of acquired samples shall be stored
 *   size [In/Out] Address of variable that holds input buffer size specifying the number of samples to be acquired.
 *                 On invocation of callback, registerd by IO_SET_CB_RX command, the address contains the number
 *                 of samples actually acquired.
 * Returns:
 *   none
 */
extern void io_adc_read(uint32_t dev_id, uint32_t * data, uint32_t * size);

/* ADC valid IOCTLs
  cmd                     arg type        arg value
  IO_SET_CB_RX             io_cb_t         Callback function invoked from ISR whenever a series of samples
                                           is available in the buffer provided by io_adc_read
  IO_SET_CB_ERR            io_cb_t         Callback function invoked from ISR whenever input buffer overflow condition whould occur
  IO_ADC_SET_CLK_DIVIDER   uint32_t        ADC clock ratio (0 - 2097152)
  IO_ADC_SET_CONFIG        io_adc_config_t Instance of the structure that holds ADC configuration parameteres
  IO_ADC_SET_SEQ_TABLE  io_adc_seq_table_t Instance of the structure that holds ADC sequence table entries
  IO_ADC_SET_SEQ_MODE      uint32_t        Mode of operation of ADC sequencer:
                                           IO_ADC_SEQ_MODE_SINGLESHOT - execute a sequence once then stop automatically
                                           IO_ADC_SEQ_MODE_REPETITIVE  - execute a sequence until the sequencer is stopped by IO_ADC_SET_SEQ_STOP
                                           command or buffer overflow occurs because of application failure to provide new buffer in time
  IO_ADC_SET_SEQ_STOP      uint32_t        N/A
  IO_ADC_SET_RX_THRESHOLD  uint32_t        threshold value for the Rx FIFO
*/
extern void io_adc_ioctl(uint32_t dev_id, uint32_t cmd, void *arg);

#endif        /* ADC_H_ */
