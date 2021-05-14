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

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "adc.h"
#include "io_config.h"
#include "apexextensions.h"

/* ADC device registers */
#define ADC_SET         (0x00)
#define ADC_DIVSEQSTAT  (0x01)
#define ADC_SEQ         (0x02)
#define ADC_CTRL        (0x03)
#define ADC_INTSTAT     (0x04)
#define ADC_SAMPLE      (0x05)

#define ADC_SEQMODE_POS     (13)
#define ADC_SEQMODE_MSK     (0x1 << ADC_SEQMODE_POS)
#define ADC_SEQENTRIES_POS  (16)
#define ADC_SEQENTRIES_MSK  (0x3f << ADC_SEQENTRIES_POS)
#define ADC_THRESHOLD_POS   (24)
#define ADC_THRESHOLD_MSK   (0x3f << ADC_THRESHOLD_POS)

/* ADC Specific macros */
#define ADC_FLUSH_RX                (0x40000000)
#define ADC_FTL_SET_MASK            (0x00ffffff)    /* FIFO threshold level */
#define ADC_SEQ_SIZE_SET_MASK       (0x3fc0ffff)
#define ADC_SEQ_MODE_SET_MASK       (0x3fffdfff)
#define ADC_CONFIG_SET_MASK         (0x3fffe000)
#define ADC_CLK_RATIO_MASK          (0x1fffff)
#define ADC_CLR_EOSINGLE            (1 << 20)
#define ADC_CLR_UNDRFLOW            (1 << 18)
#define ADC_CLR_OVERFLOW            (1 << 17)
#define ADC_CLR_DATA_A              (1 << 16)
#define ADC_SEQ_TABLE_RST           (0x0040)
#define ADC_SEQ_PTR_RST             (0x0020)
#define ADC_SEQ_START               (0x0010)
#define ADC_SEQ_STOP_MASK           (0x078ec)
#define ADC_INT_ENA_MASK            (0x001e)
#define ADC_INT_DSB                 (0x0700)
#define ADC_INT_DSB_EOSINGLE        (0x1000)
#define ADC_INT_DSB_DATA_A          (0x0100)
#define ADC_CLK_ENABLE              (0x0004)
#define ADC_ENABLE                  (0x0002)
#define ADC_DISABLE                 (0x0)
#define ADC_RESET                   (0x1)
#define ADC_INT_DATA_A              (0x1)
#define ADC_INT_ERR                 (0x6)
#define ADC_INT_EOSINGLE            (0x10)

#define ADC_DMA_CTRL                (0xa00000b7)    /* am=b10, i=b1, dw/inc=b101, dtt=b10, r=b1, op=b11 */
#define ADC_DMA_CTRL_SIZE_POS       (8)
#define ADC_DMA_CTRL_XFER_POS       (21)

#define DMA_NONE    (0xff)

typedef _Interrupt void (*ISR) ();

typedef struct adc_info_struct {
	uint32_t reg_base;  /* base address of device register set */
	uint8_t instID;
	uint16_t fifosize;
	uint16_t dmachanid;

	uint32_t rx_len;
	uint32_t *buffer[2];
	uint32_t *bufsize[2];
#ifdef __Xdmac
	_Uncached uint32_t *dmadescriptor;
	uint8_t dmaidx;
#endif
	uint32_t bufcount;
	uint8_t singlerun_count; /* store number of samples that are left to read for a single run */
	/* Callbacks */
	IO_CB_FUNC rx_cb;
	IO_CB_FUNC err_cb;
	/* Interrupt numbers and handlers */
	uint8_t rx_vector;      /* ISR vectors */
	uint8_t err_vector;
	ISR rx_isr;             /* ADC device ISRs */
	ISR err_isr;
} adc_info_t, *adc_info_pt;

#ifdef IO_ADC0_PRESENT
#define ADC_DEV_PRESENT
#endif
#ifdef IO_ADC1_PRESENT
#define ADC_DEV_PRESENT
#endif
#ifdef IO_ADC2_PRESENT
#define ADC_DEV_PRESENT
#endif
#ifdef IO_ADC3_PRESENT
#define ADC_DEV_PRESENT
#endif
#ifdef IO_ADC4_PRESENT
#define ADC_DEV_PRESENT
#endif
#ifdef IO_ADC5_PRESENT
#define ADC_DEV_PRESENT
#endif
#ifdef IO_ADC6_PRESENT
#define ADC_DEV_PRESENT
#endif
#ifdef IO_ADC7_PRESENT
#define ADC_DEV_PRESENT
#endif
#ifdef IO_ADC8_PRESENT
#define ADC_DEV_PRESENT
#endif
#ifdef IO_ADC9_PRESENT
#define ADC_DEV_PRESENT
#endif

#ifdef ADC_DEV_PRESENT

#define ADC_MAX_CNT (10)

static void adc_rx_ISR_proc(uint32_t dev_id);
static void adc_err_ISR_proc(uint32_t dev_id);

#ifdef IO_ADC0_PRESENT
static _Interrupt void adc0_rx_ISR()
{
	adc_rx_ISR_proc(0);
}

static _Interrupt void adc0_err_ISR()
{
	adc_err_ISR_proc(0);
}
#endif
#ifdef IO_ADC1_PRESENT
static _Interrupt void adc1_rx_ISR()
{
	adc_rx_ISR_proc(1);
}

static _Interrupt void adc1_err_ISR()
{
	adc_err_ISR_proc(1);
}
#endif
#ifdef IO_ADC2_PRESENT
static _Interrupt void adc2_rx_ISR()
{
	adc_rx_ISR_proc(2);
}

static _Interrupt void adc2_err_ISR()
{
	adc_err_ISR_proc(2);
}
#endif
#ifdef IO_ADC3_PRESENT

static _Interrupt void adc3_rx_ISR()
{
	adc_rx_ISR_proc(3);
}

static _Interrupt void adc3_err_ISR()
{
	adc_err_ISR_proc(3);
}
#endif
#ifdef IO_ADC4_PRESENT
static _Interrupt void adc4_rx_ISR()
{
	adc_rx_ISR_proc(4);
}

static _Interrupt void adc4_err_ISR()
{
	adc_err_ISR_proc(4);
}
#endif
#ifdef IO_ADC5_PRESENT
static _Interrupt void adc5_rx_ISR()
{
	adc_rx_ISR_proc(5);
}

static _Interrupt void adc5_err_ISR()
{
	adc_err_ISR_proc(5);
}
#endif
#ifdef IO_ADC6_PRESENT
static _Interrupt void adc6_rx_ISR()
{
	adc_rx_ISR_proc(6);
}

static _Interrupt void adc6_err_ISR()
{
	adc_err_ISR_proc(6);
}
#endif
#ifdef IO_ADC7_PRESENT
static _Interrupt void adc7_rx_ISR()
{
	adc_rx_ISR_proc(7);
}

static _Interrupt void adc7_err_ISR()
{
	adc_err_ISR_proc(7);
}
#endif
#ifdef IO_ADC8_PRESENT
static _Interrupt void adc8_rx_ISR()
{
	adc_rx_ISR_proc(8);
}

static _Interrupt void adc8_err_ISR()
{
	adc_err_ISR_proc(8);
}
#endif
#ifdef IO_ADC9_PRESENT
static _Interrupt void adc9_rx_ISR()
{
	adc_rx_ISR_proc(9);
}

static _Interrupt void adc9_err_ISR()
{
	adc_err_ISR_proc(9);
}
#endif

/* ADC devices private data structures */
static adc_info_pt adc_handles[ADC_MAX_CNT] = { 0 };

#ifdef __Xdmac
static _Uncached uint32_t adc_dma_descriptors[8 * ADC_MAX_CNT];
#endif

static adc_info_t adc_devs[] = {
#ifdef IO_ADC0_PRESENT
	{ .instID = 0,
	  .reg_base = AR_IO_ADC0_SET,
	  .rx_vector = IO_ADC0_INT_IRQ,
	  .err_vector = IO_ADC0_INT_ERR,
	  .rx_isr = adc0_rx_ISR,
	  .err_isr = adc0_err_ISR,
#ifdef IO_ADC0_DMA
	  .dmachanid = IO_ADC0_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifosize = IO_ADC0_FS },
#endif
#ifdef IO_ADC1_PRESENT
	{ .instID = 1,
	  .reg_base = AR_IO_ADC1_SET,
	  .rx_vector = IO_ADC1_INT_IRQ,
	  .err_vector = IO_ADC1_INT_ERR,
	  .rx_isr = adc1_rx_ISR,
	  .err_isr = adc1_err_ISR,
#ifdef IO_ADC1_DMA
	  .dmachanid = IO_ADC1_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifosize = IO_ADC1_FS },
#endif
#ifdef IO_ADC2_PRESENT
	{ .instID = 2,
	  .reg_base = AR_IO_ADC2_SET,
	  .rx_vector = IO_ADC2_INT_IRQ,
	  .err_vector = IO_ADC2_INT_ERR,
	  .rx_isr = adc2_rx_ISR,
	  .err_isr = adc2_err_ISR,
#ifdef IO_ADC2_DMA
	  .dmachanid = IO_ADC2_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifosize = IO_ADC2_FS },
#endif
#ifdef IO_ADC3_PRESENT
	{ .instID = 3,
	  .reg_base = AR_IO_ADC3_SET,
	  .rx_vector = IO_ADC3_INT_IRQ,
	  .err_vector = IO_ADC3_INT_ERR,
	  .rx_isr = adc3_rx_ISR,
	  .err_isr = adc3_err_ISR,
#ifdef IO_ADC3_DMA
	  .dmachanid = IO_ADC3_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifosize = IO_ADC3_FS },
#endif
#ifdef IO_ADC4_PRESENT
	{ .instID = 4,
	  .reg_base = AR_IO_ADC4_SET,
	  .rx_vector = IO_ADC4_INT_IRQ,
	  .err_vector = IO_ADC4_INT_ERR,
	  .rx_isr = adc4_rx_ISR,
	  .err_isr = adc4_err_ISR,
#ifdef IO_ADC4_DMA
	  .dmachanid = IO_ADC4_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifosize = IO_ADC4_FS },
#endif
#ifdef IO_ADC5_PRESENT
	{ .instID = 5,
	  .reg_base = AR_IO_ADC5_SET,
	  .rx_vector = IO_ADC5_INT_IRQ,
	  .err_vector = IO_ADC5_INT_ERR,
	  .rx_isr = adc5_rx_ISR,
	  .err_isr = adc5_err_ISR,
#ifdef IO_ADC5_DMA
	  .dmachanid = IO_ADC5_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifosize = IO_ADC5_FS },
#endif
#ifdef IO_ADC6_PRESENT
	{ .instID = 6,
	  .reg_base = AR_IO_ADC6_SET,
	  .rx_vector = IO_ADC6_INT_IRQ,
	  .err_vector = IO_ADC6_INT_ERR,
	  .rx_isr = adc6_rx_ISR,
	  .err_isr = adc6_err_ISR,
#ifdef IO_ADC6_DMA
	  .dmachanid = IO_ADC6_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifosize = IO_ADC6_FS },
#endif
#ifdef IO_ADC7_PRESENT
	{ .instID = 7,
	  .reg_base = AR_IO_ADC7_SET,
	  .rx_vector = IO_ADC7_INT_IRQ,
	  .err_vector = IO_ADC7_INT_ERR,
	  .rx_isr = adc7_rx_ISR,
	  .err_isr = adc7_err_ISR,
#ifdef IO_ADC7_DMA
	  .dmachanid = IO_ADC7_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifosize = IO_ADC7_FS },
#endif
#ifdef IO_ADC8_PRESENT
	{ .instID = 8,
	  .reg_base = AR_IO_ADC8_SET,
	  .rx_vector = IO_ADC8_INT_IRQ,
	  .err_vector = IO_ADC8_INT_ERR,
	  .rx_isr = adc8_rx_ISR,
	  .err_isr = adc8_err_ISR,
#ifdef IO_ADC8_DMA
	  .dmachanid = IO_ADC8_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifosize = IO_ADC8_FS },
#endif
#ifdef IO_ADC9_PRESENT
	{ .instID = 9,
	  .reg_base = AR_IO_ADC9_SET,
	  .rx_vector = IO_ADC9_INT_IRQ,
	  .err_vector = IO_ADC9_INT_ERR,
	  .rx_isr = adc9_rx_ISR,
	  .err_isr = adc9_err_ISR,
#ifdef IO_ADC9_DMA
	  .dmachanid = IO_ADC9_DMA,
#else
	  .dmachanid = DMA_NONE,
#endif
	  .fifosize = IO_ADC9_FS },
#endif
	{ .instID = ADC_MAX_CNT }
};

#define     REG_WRITE(reg, x)   _sr((uint32_t)((x)), (uint32_t)(dev->reg_base + reg))
#define     REG_READ(reg)       _lr((uint32_t)(dev->reg_base + (reg)))

/* API functions */
uint32_t io_adc_open(uint32_t dev_id)
{
	uint32_t h = 0;
	adc_info_pt dev;

	h = 0;
	while ((adc_devs[h].instID != dev_id)
	       && (adc_devs[h].instID != ADC_MAX_CNT)) {
		h++;
	}
	if ((adc_devs[h].instID == ADC_MAX_CNT) || (0 != adc_handles[dev_id])) { /* dev_id not part of design, or still open */
		return 1;
	}
	adc_handles[dev_id] = &adc_devs[h];
	dev = adc_handles[dev_id];

	dev->bufcount = 0;
	dev->rx_len = 0;
	dev->singlerun_count = 0;

#ifdef __Xdmac
	dev->dmadescriptor = &adc_dma_descriptors[8 * h];
	dev->dmaidx = 0;

	if (dev->dmachanid != DMA_NONE) {
		dev->dmadescriptor[1] = dev->dmadescriptor[5] =
			dev->reg_base + ADC_SAMPLE;
		dev->dmadescriptor[3] = (uint32_t) &(dev->dmadescriptor[4]);
		dev->dmadescriptor[7] = (uint32_t) &(dev->dmadescriptor[0]);

		/* just enable the IP clock; it's up to the user to enable the ADs; disable appropriate interrupts */
		REG_WRITE(ADC_CTRL, ADC_CLK_ENABLE);
		REG_WRITE(ADC_CTRL,
			  ADC_CLK_ENABLE | ADC_INT_DSB_DATA_A |
			  ADC_INT_DSB_EOSINGLE);
		_setvecti(dev->err_vector, dev->err_isr);

		_setvecti(DMAC_INT_BASE + dev->dmachanid, dev->rx_isr);
		_setvecti(DMAC_ERR_BASE + dev->dmachanid, dev->err_isr);
	} else {
#endif
	/* just enable the IP clock; it's up to the user to enable the ADs */
	REG_WRITE(ADC_CTRL, ADC_CLK_ENABLE);
	_setvecti(dev->rx_vector, dev->rx_isr);
	_setvecti(dev->err_vector, dev->err_isr);
#ifdef __Xdmac
}
#endif

	return (0);
}

void io_adc_close(uint32_t dev_id)
{
	adc_info_pt dev = adc_handles[dev_id];

	/* sw reset */
	REG_WRITE(ADC_CTRL, 0x1);

	_setvecti(dev->rx_vector, NULL);
	_setvecti(dev->err_vector, NULL);

#ifdef __Xdmac
	_setvecti(DMAC_INT_BASE + dev->dmachanid, NULL);
	_setvecti(DMAC_ERR_BASE + dev->dmachanid, NULL);
	_dma_chan_reset(0x1 << dev->dmachanid);
#endif

	dev->rx_cb = NULL;
	dev->err_cb = NULL;

	adc_handles[dev_id] = 0;
}

void io_adc_read(uint32_t dev_id, uint32_t *data, uint32_t *size)
{
	adc_info_pt dev = adc_handles[dev_id];
	uint32_t reg_val;

	if (dev->bufcount != 2) {
		dev->buffer[dev->bufcount] = data;
		dev->bufsize[dev->bufcount] = size;
		dev->bufcount++;

#ifdef __Xdmac
		if (dev->dmachanid != DMA_NONE) {
			uint32_t threshold, seqsize;

			if ((REG_READ(ADC_SET) & ADC_SEQMODE_MSK) == 0) { /* single run mode: update singlerun_count */
				seqsize =
					1 +
					((REG_READ(ADC_SET) & ADC_SEQENTRIES_MSK) >>
					 ADC_SEQENTRIES_POS);
				if ((seqsize - dev->singlerun_count) > *size) {
					dev->singlerun_count += *size;
				} else {
					*size = seqsize - dev->singlerun_count;
					dev->singlerun_count = seqsize;
				}
			}
			if (*size != 0) {
				threshold =
					(REG_READ(ADC_SET) & ADC_THRESHOLD_MSK) >>
					ADC_THRESHOLD_POS;
				dev->dmadescriptor[dev->dmaidx * 4 + 0] =
					ADC_DMA_CTRL | ((threshold + 1) <<
							ADC_DMA_CTRL_XFER_POS) | ((*size * 4 -
										   1) <<
										  ADC_DMA_CTRL_SIZE_POS);
				dev->dmadescriptor[dev->dmaidx * 4 + 2] =
					(uint32_t) data + ((*size - 1) * 4);

				/* init DMA, if not running yet */
				if (dev->bufcount == 1) {
					if (dev->dmachanid < DMAC_MEM_CHAN_CNT) {
						_dma_chan_desc(dev->dmachanid,
							       &(dev->
								 dmadescriptor[dev->dmaidx * 4]));
					} else {
						_dma_chan_desc_aux(dev->dmachanid,
								   &(dev->
								     dmadescriptor[dev->dmaidx *
										   4]));
					}
				}

				_dma_chan_enable((0x1 << dev->dmachanid), 1);

				dev->dmaidx = 1 - dev->dmaidx;
			}
		}
#endif

		/* start the sequencer sampling, if not already done */
		reg_val = REG_READ(ADC_CTRL) | ADC_SEQ_START;
		REG_WRITE(ADC_CTRL, reg_val);
	}
}

void io_adc_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
	adc_info_pt dev = adc_handles[dev_id];
	uint32_t ctrl, reg_val, val;

	switch (cmd) {
	case IO_SET_CB_RX:
		dev->rx_cb = ((io_cb_t *) arg)->cb;
		break;

	case IO_SET_CB_ERR:
		dev->err_cb = ((io_cb_t *) arg)->cb;
		break;

	case IO_ADC_SET_CLK_DIVIDER:

		REG_WRITE(ADC_DIVSEQSTAT,
			  *((uint32_t *) arg) & ADC_CLK_RATIO_MASK);
		break;

	case IO_ADC_SET_CONFIG:
	{
		io_adc_config_t *cfg = (io_adc_config_t *) arg;

		reg_val = REG_READ(ADC_SET);
		reg_val &= ADC_CONFIG_SET_MASK;
		val = (cfg->sample_width - 1) & 0x1f;
		val |= ((cfg->in_mode & 0x1) << 5);
		val |= ((cfg->capture_mode & 0x1) << 6);
		val |= ((cfg->out_mode & 0x1) << 7);
		val |= ((cfg->serial_dly & 0x1f) << 8);
		REG_WRITE(ADC_SET, reg_val | val);
	}
	break;

	case IO_ADC_SET_SEQ_TABLE:
	{
		io_adc_seq_table_t *seq_tbl = (io_adc_seq_table_t *) arg;
		io_adc_seq_entry_t *entry;
		uint32_t i, num_iters;

		ctrl = REG_READ(ADC_CTRL);
		REG_WRITE(ADC_CTRL, ctrl | ADC_SEQ_TABLE_RST);
		reg_val = REG_READ(ADC_SET);
		reg_val &= ADC_SEQ_SIZE_SET_MASK;
		reg_val |= (((seq_tbl->num_entries - 1) & 0x3f) << 16);
		REG_WRITE(ADC_SET, reg_val);
		num_iters = seq_tbl->num_entries / 2;

		for (i = 0, entry = seq_tbl->entries;
		     i < num_iters; i++, entry += 2) {
			reg_val = ((entry[1].sample_dly & 0x7ff) << 21);
			reg_val |= ((entry[1].channel_id & 0x1f) << 16);
			reg_val |= ((entry[0].sample_dly & 0x7ff) << 5);
			reg_val |= (entry[0].channel_id & 0x1f);
			REG_WRITE(ADC_SEQ, reg_val);
		}
		if (0 != (seq_tbl->num_entries % 2)) {
			reg_val = ((entry[0].sample_dly & 0x7ff) << 5);
			reg_val |= (entry[0].channel_id & 0x1f);
			REG_WRITE(ADC_SEQ, reg_val);
		}
		REG_WRITE(ADC_CTRL, ctrl | ADC_SEQ_PTR_RST);

		/* re-initialize buffer administration (cf. open() call) */
		dev->bufcount = 0;
		dev->rx_len = 0;
		dev->singlerun_count = 0;
	}
	break;

	case IO_ADC_SET_SEQ_MODE:
		reg_val = REG_READ(ADC_SET);
		reg_val &= ADC_SEQ_MODE_SET_MASK;
		ctrl = REG_READ(ADC_CTRL);
		if (0 == *((uint32_t *) arg)) {
			REG_WRITE(ADC_SET, reg_val);
			if (dev->dmachanid == DMA_NONE) {
				REG_WRITE(ADC_CTRL, (ctrl & ~ADC_INT_DSB_EOSINGLE));
			}
		} else {
			REG_WRITE(ADC_SET, (reg_val | (0x1 << 13)));
			REG_WRITE(ADC_CTRL, (ctrl | ADC_INT_DSB_EOSINGLE)); /* already disabled for dma mode */
		}
		dev->singlerun_count = 0;
		break;

	case IO_ADC_SET_SEQ_STOP:

		ctrl = REG_READ(ADC_CTRL);
		REG_WRITE(ADC_CTRL, ctrl & (~(0x1 << 4)));
		reg_val = REG_READ(ADC_SET);
		REG_WRITE(ADC_SET, reg_val | ADC_FLUSH_RX);
		dev->singlerun_count = 0;
		break;

	case IO_ADC_SET_RX_THRESHOLD:

		reg_val = REG_READ(ADC_SET);
		reg_val &= 0xc0ffffff;
		/* assumption threshold in range 0..63, no input value checking; use assert if required */
		reg_val |= (*((uint32_t *) arg) << 24);
		REG_WRITE(ADC_SET, reg_val);
		break;

	case IO_ADC_SET_ADC_ENA:

		reg_val = REG_READ(ADC_CTRL);
		reg_val |= (*((uint32_t *) arg) << 1);
		REG_WRITE(ADC_CTRL, reg_val);
		break;

	default:
		break;
	}
}

static uint32_t adc_read_samples(uint32_t dev_id, uint32_t *count)
{
	adc_info_pt dev = adc_handles[dev_id];
	uint32_t i, free_cnt = 0;
	uint32_t rval = 0;

	if (dev->bufcount == 0) { /* no buffer available (underflow error) */
		if (NULL != dev->err_cb) {
			dev->err_cb(dev_id);
		}
		dev->singlerun_count = 0;
		rval = 1;
	} else {
		free_cnt = (*(dev->bufsize[0]) - dev->rx_len);
		if (*count <= free_cnt) { /* enough space in buffer[0] */
			free_cnt = *count;
			*count = 0;
		} else { /* *count > free_cnt: there will be samples remaining for the next buffer */
			*count -= free_cnt;
		}
		/* fill buffer[0] */
		for (i = 0; i != free_cnt; i++) {
			dev->buffer[0][dev->rx_len + i] = REG_READ(ADC_SAMPLE);
		}
		dev->rx_len += free_cnt;
		/* if buffer[0] is full or is not full but the last sample of a single run has been read: rx callback, and switch buffers, if any */
		if ((((REG_READ(ADC_SET) & ADC_SEQMODE_MSK) == 0)
		     && (REG_READ(ADC_INTSTAT) & ADC_INT_EOSINGLE)
		     && (*count == 0)
		     )
		    || (dev->rx_len == *(dev->bufsize[0]))
		    ) {
			*(dev->bufsize[0]) = dev->rx_len;
			if (NULL != dev->rx_cb) {
				dev->rx_cb(dev_id);
			}
			dev->bufcount--;
			dev->buffer[0] = dev->buffer[1];
			dev->bufsize[0] = dev->bufsize[1];
			dev->rx_len = 0;
		}
	}

	return (rval);
}

static void adc_rx_ISR_proc(uint32_t dev_id)
{
	adc_info_pt dev = adc_handles[dev_id];
	uint32_t status = REG_READ(ADC_INTSTAT);
	uint32_t reg_val = 0;
	uint32_t sample_cnt = 0, threshold = 0, seqsize = 0;
	uint32_t error = 0;

#ifdef __Xdmac
	if (dev->dmachanid == DMA_NONE) {
#endif
	/* determine amount of data to be read */
	if (status & ADC_INT_EOSINGLE) {
		seqsize =
			1 +
			((REG_READ(ADC_SET) & ADC_SEQENTRIES_MSK) >>
			 ADC_SEQENTRIES_POS);
		sample_cnt = (seqsize - dev->singlerun_count);
	} else {
		threshold =
			(REG_READ(ADC_SET) & ADC_THRESHOLD_MSK) >>
			ADC_THRESHOLD_POS;
		sample_cnt = 1 + threshold;
	}

	/* repeat filling buffers until all samples processed or no buffers remaining */
	error = 0;
	while ((sample_cnt != 0) && (error == 0)) {
		error = adc_read_samples(dev_id, &sample_cnt);
	}
	if (error == 0) {                                               /* all available samples must have been processed */
		if ((REG_READ(ADC_SET) & ADC_SEQMODE_MSK) == 0) {       /* single run mode: update singlerun_count (not important to check for EOSINGLE) */
			dev->singlerun_count += (1 + threshold);
		}
	}

	/* clear interrupt(s) */
	reg_val = REG_READ(ADC_CTRL);
	if (status & ADC_INT_EOSINGLE) {
		REG_WRITE(ADC_CTRL, reg_val | ADC_CLR_EOSINGLE);
	}
	REG_WRITE(ADC_CTRL, reg_val | ADC_CLR_DATA_A);
#ifdef __Xdmac
} else {                /* DMA enabled */
	if (NULL != dev->rx_cb) {
		dev->rx_cb(dev_id);
	}
	dev->bufcount--;
	dev->buffer[0] = dev->buffer[1];
	dev->bufsize[0] = dev->bufsize[1];
	/* if remaining buffer is of size 0, the descriptor will not lead to an interrupt: process here */
	if ((*(dev->bufsize[0]) == 0) && (dev->bufcount != 0)) {
		if (NULL != dev->rx_cb) {
			dev->rx_cb(dev_id);
		}
		dev->bufcount--;
	}
	if (dev->bufcount == 0) { /* processing done - disable dma channel */
		_dma_chan_enable((0x1 << dev->dmachanid), 0);
	}
	/* clear dma interrupt */
	_dma_int_clear((0x1 << dev->dmachanid));
}
#endif
}

static void adc_err_ISR_proc(uint32_t dev_id)
{
	adc_info_pt dev = adc_handles[dev_id];
	uint32_t h = 0;
	uint32_t status = REG_READ(ADC_INTSTAT);
	uint32_t reg_val = 0;

	/* stop the sequencer */
	reg_val = REG_READ(ADC_CTRL);
	REG_WRITE(ADC_CTRL, reg_val & (~(0x1 << 4)));
	reg_val = REG_READ(ADC_SET);
	REG_WRITE(ADC_SET, reg_val | ADC_FLUSH_RX);
	dev->singlerun_count = 0;

#ifdef __Xdmac
	if (dev->dmachanid != DMA_NONE) {
		if ((status & 0xe) == 0) { /* error raised by DMA */
			if (NULL != dev->err_cb) {
				dev->err_cb(dev_id);
			}
		}
	}
#endif

	/* check error source */
	for (h = 1; h != 4; h++) {
		if (status & (0x1 << h)) {
			/* error source == h */
			if (NULL != dev->err_cb) {
				dev->err_cb(dev_id);
			}

			/* clear interrupt */
			reg_val = REG_READ(ADC_CTRL);
			REG_WRITE(ADC_CTRL, reg_val | (0x1 << (h + 16)));
		}
	}
}

#endif
