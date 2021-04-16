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

#ifndef PWM_MASTER_H_
#define PWM_MASTER_H_


#include "device/ip_hal/ioctl.h"


/* PWM control commands */
#define IO_PWM_SET_CLKDIV          (0x20)
#define IO_PWM_SET_TIMER_CFG       (0x21)
#define IO_PWM_ENABLE_TIMER        (0x22)
#define IO_PWM_SET_GENERATOR_CFG   (0x23)
#define IO_PWM_MASK_CHANNEL        (0x24)
#define IO_PWM_ENABLE_CHANNEL      (0x25)
#define IO_PWM_SET_TRIGGER_CFG     (0x26)
#define IO_PWM_ENABLE_TRIGGER      (0x27)
#define IO_PWM_SET_FAULT_INPUT_POL (0x28)
#define IO_PWM_SET_FAULT_CFG       (0x29)

/* PWM interrupt callback values */
#define IO_PWM_CB_NPERIODS         (0)
#define IO_PWM_CB_UPD_MISSED(h)    ((h)+4)
#define IO_PWM_CB_TRIGGER(h)       ((h)+8)
#define IO_PWM_CB_FAULT(h)         ((h)+12)
#define IO_PWM_CB_DEV(h)           ((h) << 16)

/* PWM typedefs */
typedef struct _pwm_timer_cfg_t {
    uint16_t max;
    uint16_t periods;
    uint8_t mode;
} io_pwm_timer_cfg_t, *io_pwm_timer_cfg_pt;

typedef struct _pwm_generator_cfg_t {
    uint8_t chans_id;
    uint8_t polarity;
    uint16_t threshold;
    uint16_t deadzone;
} io_pwm_generator_cfg_t, *io_pwm_generator_cfg_pt;

typedef struct _pwm_chan_cfg_t {
    uint8_t chan_id;
    uint8_t value;
} io_pwm_chan_cfg_t, *io_pwm_chan_cfg_pt;

typedef struct _pwm_trigger_cfg_t {
    uint8_t chans_id;
    uint8_t width;
    uint8_t mode;
    uint8_t is_enabled;
} io_pwm_trigger_cfg_t, *io_pwm_trigger_cfg_pt;

typedef struct _pwm_fault_cfg_t {
    uint8_t chans_id;
    uint8_t mode;
    uint8_t mask;
} io_pwm_fault_cfg_t, *io_pwm_fault_cfg_pt;

extern uint32_t io_pwm_open(uint32_t dev_id);
extern void io_pwm_close(uint32_t dev_id);
/* PWM valid IOCTLs
  cmd                           arg type             arg value
  IO_SET_CB_TX                  io_cb_t              Callback function invoked from ISR when write successfully finishes
  IO_SET_CB_ERR                 io_cb_t              Callback function invoked from ISR when read/write fails
  IO_PWM_SET_CLKDIV             uint32_t             defines clock division (1 + value)
  IO_PWM_SET_TIMER_CFG          io_pwm_timer_cfg_pt  timer configuration (mode [0-up/1-down/2-updown/3-downup], max value, no. of periods)
  IO_PWM_ENABLE_TIMER           uint32_t             0 - disable / 1 - enable
  IO_PWM_SET_GENERATOR_CFG      io_pwm_generator_cfg_pt generator configuration (id : 0-3 = 01/23/45/67, polarity [xy : 00-11 = normal,normal - inverted,inverted] threshold, deadzone)
  IO_PWM_MASK_CHANNEL           io_pwm_chan_cfg_pt       0 - non-masked / 1 - masked
  IO_PWM_ENABLE_CHANNEL         io_pwm_chan_cfg_pt       0 - disable / 1 - enable
  IO_PWM_SET_TRIGGER_CFG        io_pwm_trigger_cfg_pt   trigger configuration (id : 0-3 = 01/23/45/67, width = trigger width, mode [0-up/1-down/2-up+down], is_enabled : 0/1)
  IO_PWM_ENABLE_TRIGGER         io_pwm_chan_cfg_pt       0 - disable / 1 - enable
  IO_PWM_SET_FAULT_INPUT_POL    io_pwm_chan_cfg_pt       0 - active-high / 1 - active-low
  IO_PWM_SET_FAULT_CFG          io_pwm_fault_cfg_pt     fault configuration (id : 0-3 = 01/23/45/67, mode [00-disable/01-inactive/10-low/11-high], mask [xyz inputs])
*/
extern void io_pwm_ioctl(uint32_t dev_id, uint32_t cmd, void *arg);


#endif              /* PWM_MASTER_H_ */
