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

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "pwm.h"
#include "io_config.h"
#include "apexextensions.h"


/* EIA PWM device registers  */
#define     PWM_CTRL              (0x000)	/* PWM control register */
#define     PWM_CHN_CONFIG        (0x001)	/* PWM channel config register */
#define     PWM_TRIGGER           (0x002)	/* PWM trigger register */
#define     PWM_FAULT			  (0x003)	/* PWM fault register */
#define     PWM_EVENTS            (0x004)	/* PWM events register */
#define		PWM_INTCTRL	          (0x005)	/* PWM interrupt control register */
#define		PWM_INTSTAT			  (0x006)	/* PWM interrupt status register */
#define		PWM_INTCLR			  (0x007)	/* PWM interrupt clear register */
#define     PWM_THRESHOLD_01      (0x008)	/* PWM threshold register */
#define     PWM_DEADZONE_01       (0x00c)	/* PWM deadzone register */
#define     PWM_TIMER_MAX         (0x010)	/* PWM timer max register */
#define     PWM_NPERIODS          (0x011)	/* PWM no of periods register */
#define     PWM_CLK_DIV           (0x012)	/* PWM clock divider register */
#define     PWM_CLKEN             (0x020)	/* PWM clock enable register */


/* PWM specific macros */
#define     PWM_TIMER_MODE_MSK		(0xc)
#define     PWM_TIMER_MODE_POS		(2)
#define		PWM_POL_CHANS_MSK		(0x3 << 16)
#define		PWM_POL_CHANS_POS		(16)
#define		PWM_MASK_CHAN_MSK		(0x1 << 8)
#define		PWM_MASK_CHAN_POS		(8)
#define		PWM_ENABLE_CHAN_MSK		(0x1)
#define		PWM_TRIGGER_WIDTH_MSK   (0x7 << 12)
#define		PWM_TRIGGER_WIDTH_POS   (12)
#define		PWM_TRIGGER_MODE_MSK    (0x3 << 4)
#define		PWM_TRIGGER_MODE_POS    (4)
#define		PWM_FAULT_RECOVER_MSK   (0x1 << 23)
#define		PWM_FAULT_RECOVER_POS   (23)
#define		PWM_FAULT_INPUT_POL_MSK (0x1 << 20)
#define		PWM_FAULT_INPUT_POL_POS (20)
#define		PWM_FAULT_MODE_MSK      (0x3 << 12)
#define		PWM_FAULT_MODE_POS      (12)
#define		PWM_FAULT_MASK_MSK      (0x7 << 0)
#define		PWM_FAULT_MASK_POS      (0)
#define		PWM_FAULT_0_MASK_MSK    (0x249)	/* bit 0,3,6,9 */
#define		PWM_UPDM_INT_POS		(4)
#define		PWM_TRIGGER_INT_POS		(8)
#define		PWM_FAULT_INT_POS       (12)
#define     PWM_TX_INT_MSK			(0xff1)
#define     PWM_ERR_INT_MSK			(0x7 << 12)

typedef _Interrupt void (*ISR) ();

/* Private data structure maintained by the driver. */
typedef struct pwm_info_struct {
    uint32_t reg_base;		/* base address of device register set */
    /* pwm instance data */
    uint8_t instID;
    uint8_t new_trig_cnt;
    io_pwm_trigger_cfg_t new_triggers[4];

    /* Callbacks */
    IO_CB_FUNC tx_cb;
    IO_CB_FUNC err_cb;
    /* Interrupt numbers and handlers */
    uint8_t new_periods_vector;	/* ISR vectors */
    uint8_t missed_update_vector;
    uint8_t trigger_vector;
    uint8_t fault_vector;
    ISR tx_isr;			/* PWM device ISRs */
    ISR err_isr;
} pwm_info_t, *pwm_info_pt;

#ifdef IO_PWM0_PRESENT
#define PWM_DEV_PRESENT
#endif
#ifdef IO_PWM1_PRESENT
#define PWM_DEV_PRESENT
#endif
#ifdef IO_PWM2_PRESENT
#define PWM_DEV_PRESENT
#endif
#ifdef IO_PWM3_PRESENT
#define PWM_DEV_PRESENT
#endif

#ifdef PWM_DEV_PRESENT

#define PWM_MAX_CNT	(4)

static void pwm_tx_ISR_proc(uint32_t dev_id);
static void pwm_err_ISR_proc(uint32_t dev_id);

#ifdef IO_PWM0_PRESENT
static _Interrupt void pwm0_tx_ISR()
{
    pwm_tx_ISR_proc(0);
}

static _Interrupt void pwm0_err_ISR()
{
    pwm_err_ISR_proc(0);
}
#endif
#ifdef IO_PWM1_PRESENT
static _Interrupt void pwm1_tx_ISR()
{
    pwm_tx_ISR_proc(1);
}

static _Interrupt void pwm1_err_ISR()
{
    pwm_err_ISR_proc(1);
}
#endif
#ifdef IO_PWM2_PRESENT
static _Interrupt void pwm2_tx_ISR()
{
    pwm_tx_ISR_proc(2);
}

static _Interrupt void pwm2_err_ISR()
{
    pwm_err_ISR_proc(2);
}
#endif
#ifdef IO_PWM3_PRESENT
static _Interrupt void pwm3_tx_ISR()
{
    pwm_tx_ISR_proc(3);
}

static _Interrupt void pwm3_err_ISR()
{
    pwm_err_ISR_proc(3);
}
#endif


/* SPI master devices private data structures */
static pwm_info_pt pwm_handles[PWM_MAX_CNT] = { 0 };

static pwm_info_t pwm_devs[] = {
#ifdef IO_PWM0_PRESENT
    {.instID = 0,
     .reg_base = AR_IO_PWM0_CTRL,
     .new_periods_vector = IO_PWM0_INT_NEW_NPERIODS,
     .missed_update_vector = IO_PWM0_INT_UPD_MISSED,
     .trigger_vector = IO_PWM0_INT_TRG,
     .fault_vector = IO_PWM0_INT_FAULT,
     .tx_isr = pwm0_tx_ISR,
     .err_isr = pwm0_err_ISR},
#endif
#ifdef IO_PWM1_PRESENT
    {.instID = 1,
     .reg_base = AR_IO_PWM1_CTRL,
     .new_periods_vector = IO_PWM1_INT_NEW_NPERIODS,
     .missed_update_vector = IO_PWM1_INT_UPD_MISSED,
     .trigger_vector = IO_PWM1_INT_TRG,
     .fault_vector = IO_PWM1_INT_FAULT,
     .tx_isr = pwm1_tx_ISR,
     .err_isr = pwm1_err_ISR},
#endif
#ifdef IO_PWM2_PRESENT
    {.instID = 2,
     .reg_base = AR_IO_PWM2_CTRL,
     .new_periods_vector = IO_PWM2_INT_NEW_NPERIODS,
     .missed_update_vector = IO_PWM2_INT_UPD_MISSED,
     .trigger_vector = IO_PWM2_INT_TRG,
     .fault_vector = IO_PWM2_INT_FAULT,
     .tx_isr = pwm2_tx_ISR,
     .err_isr = pwm2_err_ISR},
#endif
#ifdef IO_PWM3_PRESENT
    {.instID = 3,
     .reg_base = AR_IO_PWM3_CTRL,
     .new_periods_vector = IO_PWM3_INT_NEW_NPERIODS,
     .missed_update_vector = IO_PWM3_INT_UPD_MISSED,
     .trigger_vector = IO_PWM3_INT_TRG,
     .fault_vector = IO_PWM3_INT_FAULT,
     .tx_isr = pwm3_tx_ISR,
     .err_isr = pwm3_err_ISR},
#endif
    {.instID = PWM_MAX_CNT}
};


#define     REG_WRITE( reg, x )   _sr( (unsigned)(x), (unsigned)(dev->reg_base + reg) )
#define     REG_READ( reg )       _lr( (unsigned)(dev->reg_base + reg) )

uint32_t io_pwm_open(uint32_t dev_id)
{
    uint32_t h = 0;
    pwm_info_pt dev;

    h = 0;
    while ((pwm_devs[h].instID != dev_id)
	   && (pwm_devs[h].instID != PWM_MAX_CNT)) {
	h++;
    }
    if ((pwm_devs[h].instID == PWM_MAX_CNT) || (0 != pwm_handles[dev_id])) {	/* dev_id not part of design, or still open */
	return 1;
    }
    pwm_handles[dev_id] = &pwm_devs[h];
    dev = pwm_handles[dev_id];

    /* enable clock */
    REG_WRITE(PWM_CLKEN, 0x1);
    /* automatically recover from faults */
    REG_WRITE(PWM_FAULT, PWM_FAULT_RECOVER_MSK);

    /* init */
    _setvecti(dev->new_periods_vector, dev->tx_isr);
    _setvecti(dev->missed_update_vector, dev->tx_isr);
    _setvecti(dev->trigger_vector, dev->tx_isr);
    _setvecti(dev->fault_vector, dev->err_isr);

    dev->new_trig_cnt = 0;

    return 0;
}


void io_pwm_close(uint32_t dev_id)
{
    pwm_info_pt dev = pwm_handles[dev_id];

    /* reset */
    REG_WRITE(PWM_EVENTS, 0x1);
    /* disable clock */
    REG_WRITE(PWM_CLKEN, 0);

    _setvecti(dev->new_periods_vector, NULL);
    _setvecti(dev->missed_update_vector, NULL);
    _setvecti(dev->trigger_vector, NULL);
    _setvecti(dev->fault_vector, NULL);
    dev->tx_cb = NULL;
    dev->err_cb = NULL;

    dev->new_trig_cnt = 0;

    pwm_handles[dev_id] = 0;
}


void io_pwm_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
    pwm_info_pt dev = pwm_handles[dev_id];
    io_pwm_timer_cfg_pt timer_cfg = 0;
    io_pwm_generator_cfg_pt gen_cfg = 0;
    io_pwm_chan_cfg_pt chan_cfg = 0;
    io_pwm_trigger_cfg_pt trigger_cfg = 0;
    io_pwm_fault_cfg_pt fault_cfg = 0;
    uint32_t reg_value = 0, reg_value2 = 0;
    uint32_t h = 0;

    switch (cmd) {
    case IO_SET_CB_TX:
	dev->tx_cb = ((io_cb_t *) arg)->cb;
	break;
    case IO_SET_CB_ERR:
	dev->err_cb = ((io_cb_t *) arg)->cb;
	break;

    case IO_PWM_SET_CLKDIV:
	REG_WRITE(PWM_CLK_DIV, (*((uint32_t *) arg) - 1));
	break;

    case IO_PWM_SET_TIMER_CFG:
	timer_cfg = (io_pwm_timer_cfg_pt) arg;
	REG_WRITE(PWM_TIMER_MAX, timer_cfg->max);
	REG_WRITE(PWM_NPERIODS, (timer_cfg->periods - 1));
	/* change mode only when timer is disabled */
	reg_value = REG_READ(PWM_CTRL);
	if ((reg_value & 0x2) == 0) {
	    reg_value &= ~PWM_TIMER_MODE_MSK;
	    REG_WRITE(PWM_CTRL,
		      (reg_value |
		       (timer_cfg->mode << PWM_TIMER_MODE_POS)));
	}
	break;

    case IO_PWM_ENABLE_TIMER:
	reg_value = REG_READ(PWM_CTRL);
	reg_value &= ~0x2;
	REG_WRITE(PWM_CTRL, (reg_value | ((*((uint32_t *) arg)) << 1)));
	/* enable/disable nperiods interrupt */
	reg_value = REG_READ(PWM_INTCTRL);
	if (1 == (*((uint32_t *) arg))) {
	    reg_value |= 0x1;
	} else {
	    reg_value &= ~0x1;
	}
	REG_WRITE(PWM_INTCTRL, reg_value);
	break;

    case IO_PWM_SET_GENERATOR_CFG:
	gen_cfg = (io_pwm_generator_cfg_pt) arg;
	REG_WRITE((PWM_THRESHOLD_01 + gen_cfg->chans_id),
		  gen_cfg->threshold);
	REG_WRITE((PWM_DEADZONE_01 + gen_cfg->chans_id),
		  gen_cfg->deadzone);
	reg_value = REG_READ(PWM_CHN_CONFIG);
	reg_value &= ~(PWM_POL_CHANS_MSK << 2 * gen_cfg->chans_id);
	REG_WRITE(PWM_CHN_CONFIG,
		  (reg_value |
		   (gen_cfg->
		    polarity << (PWM_POL_CHANS_POS +
				 2 * gen_cfg->chans_id))));
	break;

    case IO_PWM_MASK_CHANNEL:
	chan_cfg = (io_pwm_chan_cfg_pt) arg;
	reg_value = REG_READ(PWM_CHN_CONFIG);
	reg_value &= ~(PWM_MASK_CHAN_MSK << chan_cfg->chan_id);
	REG_WRITE(PWM_CHN_CONFIG,
		  (reg_value |
		   (chan_cfg->
		    value << (PWM_MASK_CHAN_POS + chan_cfg->chan_id))));
	break;

    case IO_PWM_ENABLE_CHANNEL:
	chan_cfg = (io_pwm_chan_cfg_pt) arg;
	reg_value = REG_READ(PWM_CHN_CONFIG);
	reg_value &= ~(PWM_ENABLE_CHAN_MSK << chan_cfg->chan_id);
	REG_WRITE(PWM_CHN_CONFIG,
		  (reg_value | (chan_cfg->value << chan_cfg->chan_id)));
	/* enable/disable missed update interrupt */
	reg_value = REG_READ(PWM_INTCTRL);
	if (1 == chan_cfg->value) {
	    reg_value |= (0x1 << (PWM_UPDM_INT_POS + chan_cfg->chan_id));
	} else {
	    reg_value &= ~(0x1 << (PWM_UPDM_INT_POS + chan_cfg->chan_id));
	}
	REG_WRITE(PWM_INTCTRL, reg_value);
	break;

    case IO_PWM_SET_TRIGGER_CFG:
	trigger_cfg = (io_pwm_trigger_cfg_pt) arg;
	reg_value = REG_READ(PWM_CTRL);
	if ((reg_value & 0x2) == 0) {	/* if timer disabled, program trigger register immediately */
	    reg_value = REG_READ(PWM_TRIGGER);
	    reg_value &=
		~(PWM_TRIGGER_WIDTH_MSK << 3 * trigger_cfg->chans_id);
	    REG_WRITE(PWM_TRIGGER,
		      (reg_value |
		       (trigger_cfg->
			width << (PWM_TRIGGER_WIDTH_POS +
				  3 * trigger_cfg->chans_id))));
	    reg_value = REG_READ(PWM_TRIGGER);
	    reg_value &=
		~(PWM_TRIGGER_MODE_MSK << 2 * trigger_cfg->chans_id);
	    REG_WRITE(PWM_TRIGGER,
		      (reg_value |
		       (trigger_cfg->
			mode << (PWM_TRIGGER_MODE_POS +
				 2 * trigger_cfg->chans_id))));
	    reg_value = REG_READ(PWM_TRIGGER);
	    reg_value &= ~(0x1 << trigger_cfg->chans_id);
	    REG_WRITE(PWM_TRIGGER,
		      (reg_value |
		       (trigger_cfg->is_enabled << trigger_cfg->
			chans_id)));
	    /* enable/disable trigger interrupt */
	    reg_value = REG_READ(PWM_INTCTRL);
	    reg_value &=
		~(0x1 << (PWM_TRIGGER_INT_POS + trigger_cfg->chans_id));
	    REG_WRITE(PWM_INTCTRL,
		      (reg_value |
		       (trigger_cfg->
			is_enabled << (PWM_TRIGGER_INT_POS +
				       trigger_cfg->chans_id))));
	} else {		/* if timer enable, (at most 4) new trigger values will be effectuated when servicing the nperiods interrupt */
	    if (dev->new_trig_cnt != 4) {
		dev->new_triggers[dev->new_trig_cnt].chans_id =
		    trigger_cfg->chans_id;
		dev->new_triggers[dev->new_trig_cnt].width =
		    trigger_cfg->width;
		dev->new_triggers[dev->new_trig_cnt].mode =
		    trigger_cfg->mode;
		dev->new_triggers[dev->new_trig_cnt].is_enabled =
		    trigger_cfg->is_enabled;
		dev->new_trig_cnt++;
	    }
	}
	break;

    case IO_PWM_ENABLE_TRIGGER:
	chan_cfg = (io_pwm_chan_cfg_pt) arg;
	reg_value = REG_READ(PWM_TRIGGER);
	reg_value &= ~(0x1 << chan_cfg->chan_id);
	REG_WRITE(PWM_TRIGGER,
		  (reg_value | (chan_cfg->value << chan_cfg->chan_id)));
	/* enable/disable trigger interrupt */
	reg_value = REG_READ(PWM_INTCTRL);
	reg_value &= ~(0x1 << (PWM_TRIGGER_INT_POS + chan_cfg->chan_id));
	REG_WRITE(PWM_INTCTRL,
		  (reg_value |
		   (chan_cfg->
		    value << (PWM_TRIGGER_INT_POS + chan_cfg->chan_id))));
	break;

    case IO_PWM_SET_FAULT_INPUT_POL:
	chan_cfg = (io_pwm_chan_cfg_pt) arg;
	reg_value = REG_READ(PWM_FAULT);
	reg_value &= ~(PWM_FAULT_INPUT_POL_MSK << chan_cfg->chan_id);
	REG_WRITE(PWM_FAULT,
		  (reg_value |
		   (chan_cfg->
		    value << (PWM_FAULT_INPUT_POL_POS +
			      chan_cfg->chan_id))));
	break;

    case IO_PWM_SET_FAULT_CFG:
	fault_cfg = (io_pwm_fault_cfg_pt) arg;
	reg_value = REG_READ(PWM_FAULT);
	reg_value &= ~(PWM_FAULT_MODE_MSK << 2 * fault_cfg->chans_id);
	REG_WRITE(PWM_FAULT,
		  (reg_value |
		   (fault_cfg->
		    mode << (PWM_FAULT_MODE_POS +
			     2 * fault_cfg->chans_id))));
	reg_value = REG_READ(PWM_FAULT);
	reg_value &= ~(PWM_FAULT_MASK_MSK << 3 * fault_cfg->chans_id);
	REG_WRITE(PWM_FAULT,
		  (reg_value |
		   (fault_cfg->
		    mask << (PWM_FAULT_MASK_POS +
			     3 * fault_cfg->chans_id))));
	/* enable/disable fault interrupt when any mask enables it */
	for (h = 0; h != 3; h++) {
	    reg_value = REG_READ(PWM_FAULT);
	    reg_value2 = REG_READ(PWM_INTCTRL);
	    if ((reg_value & (PWM_FAULT_0_MASK_MSK << h)) == 0) {
		reg_value2 &= ~(0x1 << (PWM_FAULT_INT_POS + h));
	    } else {
		reg_value2 |= (0x1 << (PWM_FAULT_INT_POS + h));
	    }
	    REG_WRITE(PWM_INTCTRL, reg_value2);
	}
	break;

    default:
	break;
    }
}


static void pwm_tx_ISR_proc(uint32_t dev_id)
{
    pwm_info_pt dev = pwm_handles[dev_id];
    io_pwm_trigger_cfg_pt trigger_cfg = 0;
    uint32_t reg_value = 0, reg_value2 = 0;
    uint32_t h = 0;
    uint32_t cb_arg = IO_PWM_CB_DEV(dev_id);

    /* nperiod, missed update, or trigger interrupt that has impact on a channel pair */
    reg_value = REG_READ(PWM_INTSTAT);
    while ((reg_value & PWM_TX_INT_MSK) != 0) {
	/* nperiod */
	if ((reg_value & 0x1) != 0) {
	    /* update trigger configuration, if available */
	    for (h = 0; h != dev->new_trig_cnt; h++) {
		trigger_cfg = &(dev->new_triggers[h]);
		reg_value2 = REG_READ(PWM_TRIGGER);
		reg_value2 &=
		    ~(PWM_TRIGGER_WIDTH_MSK << 3 * trigger_cfg->chans_id);
		REG_WRITE(PWM_TRIGGER,
			  (reg_value2 |
			   (trigger_cfg->
			    width << (PWM_TRIGGER_WIDTH_POS +
				      3 * trigger_cfg->chans_id))));
		reg_value2 = REG_READ(PWM_TRIGGER);
		reg_value2 &=
		    ~(PWM_TRIGGER_MODE_MSK << 2 * trigger_cfg->chans_id);
		REG_WRITE(PWM_TRIGGER,
			  (reg_value2 |
			   (trigger_cfg->
			    mode << (PWM_TRIGGER_MODE_POS +
				     2 * trigger_cfg->chans_id))));
		reg_value2 = REG_READ(PWM_TRIGGER);
		reg_value2 &= ~(0x1 << trigger_cfg->chans_id);
		REG_WRITE(PWM_TRIGGER,
			  (reg_value2 |
			   (trigger_cfg->is_enabled << trigger_cfg->
			    chans_id)));
		/* clear missed update interrupt for this channel, if any */
		REG_WRITE(PWM_INTCLR,
			  (0x1 <<
			   (PWM_UPDM_INT_POS + trigger_cfg->chans_id)));
	    }
	    dev->new_trig_cnt = 0;
	    /* notify application */
	    if (dev->tx_cb != NULL) {
		dev->tx_cb(cb_arg | IO_PWM_CB_NPERIODS);
	    }
	    /* clear interrupt */
	    REG_WRITE(PWM_INTCLR, 0x1);
	    reg_value = REG_READ(PWM_INTSTAT);
	}
	/* missed update */
	for (h = 0; h != 4; h++) {
	    if ((reg_value & (0x1 << (PWM_UPDM_INT_POS + h))) != 0) {
		/* notify application */
		if (dev->tx_cb != NULL) {
		    dev->tx_cb(cb_arg | IO_PWM_CB_UPD_MISSED(h));
		}
		/* clear interrupt */
		REG_WRITE(PWM_INTCLR, (0x1 << (PWM_UPDM_INT_POS + h)));
	    }
	}
	/* trigger */
	for (h = 0; h != 4; h++) {
	    if ((reg_value & (0x1 << (PWM_TRIGGER_INT_POS + h))) != 0) {
		/* notify application */
		if (dev->tx_cb != NULL) {
		    dev->tx_cb(cb_arg | IO_PWM_CB_TRIGGER(h));
		}
		/* clear interrupt */
		REG_WRITE(PWM_INTCLR, (0x1 << (PWM_TRIGGER_INT_POS + h)));
	    }
	}
	reg_value = REG_READ(PWM_INTSTAT);
    }
}


static void pwm_err_ISR_proc(uint32_t dev_id)
{
    pwm_info_pt dev = pwm_handles[dev_id];
    uint32_t reg_value = 0;
    uint32_t h = 0;
    uint32_t cb_arg = IO_PWM_CB_DEV(dev_id);

    /* fault interrupt that has impact on a channel pair */
    reg_value = REG_READ(PWM_INTSTAT);
    while ((reg_value & PWM_ERR_INT_MSK) != 0) {
	for (h = 0; h != 3; h++) {
	    if ((reg_value & (0x1 << (PWM_FAULT_INT_POS + h))) != 0) {
		/* notify application */
		if (dev->err_cb != NULL) {
		    dev->err_cb(cb_arg | IO_PWM_CB_FAULT(h));
		}
		/* clear interrupt */
		REG_WRITE(PWM_INTCLR, (0x1 << (PWM_FAULT_INT_POS + h)));
	    }
	}
	reg_value = REG_READ(PWM_INTSTAT);
    }
}


#endif
