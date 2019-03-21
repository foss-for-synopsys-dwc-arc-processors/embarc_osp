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
#include "dac.h"
#include "io_config.h"
#include "apexextensions.h"


/* EIA DAC device registers  */
#define DAC_SET             (0x00)  /* DAC Settings Register */
#define DAC_CTRL            (0x01)  /* DAC Control Register */
#define DAC_INTSTAT         (0x02)  /* DAC Status Register */
#define DAC_TX_DATA         (0x03)  /* DAC Data Register */


/* DAC specific macros */
#define DAC_FLUSH_TX                (0x1 << 30)
#define DAC_CLK_ENABLE              (0x1 << 2)
#define DAC_TX_ENABLE               (0x1 << 4)
#define DAC_IRQ_MSK                 (0x00000300)
#define DAC_IRQ_MSK_THRESHOLD       (0x1 << 8)
#define DAC_IRQ_MSK_EMPTY           (0x1 << 9)
#define DAC_IRQ_MSK_UNDERFLOW       (0x1 << 11)
#define DAC_IRQ_CLR_THRESHOLD       (0x1 << 16)
#define DAC_IRQ_CLR_EMPTY           (0x1 << 17)
#define DAC_IRQ_THRESHOLD           (0x1 << 0)
#define DAC_IRQ_EMPTY               (0x1 << 1)
#define DAC_TXFLR_MSK               (0x00000ff0)    /* sufficient bits to read FIFO level */
#define DAC_TXFLR_POS               (4)

#define DAC_DMA_CTRL                (0x600000af)    /* am=b01, i=b1, dw/inc=b101, dtt=b01, r=b1, op=b11 */
#define DAC_DMA_CTRL_SIZE_POS       (8)
#define DAC_DMA_CTRL_XFER_POS       (21)

#define  DMA_NONE    (0xff)

typedef _Interrupt void (*ISR) ();

/* Private data structure maintained by the driver. */
typedef struct dac_info_struct {
    uint32_t reg_base;      /* base address of device register set */
    /* dac instance data */
    uint8_t instID;
    uint16_t fifosize;
    uint16_t fifowidth;
    uint16_t dmachanid;

    uint32_t tx_len;
    uint32_t *buffer[2];
    uint32_t *bufsize[2];
#ifdef __Xdmac
    _Uncached uint32_t *dmadescriptor;
    uint8_t dmaidx, dmaactiveidx;
    uint8_t dmastarted;
#endif
    uint32_t bufcount;

    /* Callbacks */
    IO_CB_FUNC tx_cb;
    IO_CB_FUNC err_cb;
    /* Interrupt numbers and handlers */
    uint8_t tx_vector;      /* ISR vectors */
    uint8_t err_vector;
    ISR tx_isr;         /* SPI device ISRs */
    ISR err_isr;
    ISR dmatx_isr;
    uint16_t fifo_depth;
} dac_info_t, *dac_info_pt;

#ifdef IO_DAC0_PRESENT
#define DAC_DEV_PRESENT
#endif
#ifdef IO_DAC1_PRESENT
#define DAC_DEV_PRESENT
#endif
#ifdef IO_DAC2_PRESENT
#define DAC_DEV_PRESENT
#endif
#ifdef IO_DAC3_PRESENT
#define DAC_DEV_PRESENT
#endif

#ifdef DAC_DEV_PRESENT

#define DAC_MAX_CNT (4)

static void dac_tx_ISR_proc(uint32_t dev_id);
static void dac_err_ISR_proc(uint32_t dev_id);
static void dac_dmatx_ISR_proc(uint32_t dev_id);

#ifdef IO_DAC0_PRESENT
static _Interrupt void dac0_tx_ISR()
{
    dac_tx_ISR_proc(0);
}

static _Interrupt void dac0_err_ISR()
{
    dac_err_ISR_proc(0);
}

static _Interrupt void dac0_dmatx_ISR()
{
    dac_dmatx_ISR_proc(0);
}
#endif
#ifdef IO_DAC1_PRESENT
static _Interrupt void dac1_tx_ISR()
{
    dac_tx_ISR_proc(1);
}

static _Interrupt void dac1_err_ISR()
{
    dac_err_ISR_proc(1);
}

static _Interrupt void dac1_dmatx_ISR()
{
    dac_dmatx_ISR_proc(1);
}
#endif
#ifdef IO_DAC2_PRESENT
static _Interrupt void dac2_tx_ISR()
{
    dac_tx_ISR_proc(2);
}

static _Interrupt void dac2_err_ISR()
{
    dac_err_ISR_proc(2);
}

static _Interrupt void dac2_dmatx_ISR()
{
    dac_dmatx_ISR_proc(2);
}
#endif
#ifdef IO_DAC3_PRESENT
static _Interrupt void dac3_tx_ISR()
{
    dac_tx_ISR_proc(3);
}

static _Interrupt void dac3_err_ISR()
{
    dac_err_ISR_proc(3);
}

static _Interrupt void dac3_dmatx_ISR()
{
    dac_dmatx_ISR_proc(3);
}
#endif


/* SPI master devices private data structures */
static dac_info_pt dac_handles[DAC_MAX_CNT] = { 0 };

#ifdef __Xdmac
static _Uncached uint32_t dac_dma_descriptors[8 * DAC_MAX_CNT];
#endif

static dac_info_t dac_devs[] = {
#ifdef IO_DAC0_PRESENT
    {.instID = 0,
     .reg_base = AR_IO_DAC0_SET,
     .tx_vector = IO_DAC0_INT_IRQ,
     .err_vector = IO_DAC0_INT_ERR,
     .tx_isr = dac0_tx_ISR,
     .err_isr = dac0_err_ISR,
     .dmatx_isr = dac0_dmatx_ISR,
#ifdef IO_DAC0_DMA
     .dmachanid = IO_DAC0_DMA,
#else
     .dmachanid = DMA_NONE,
#endif
     .fifosize = IO_DAC0_FS,
     .fifowidth = IO_DAC0_FW},
#endif
#ifdef IO_DAC1_PRESENT
    {.instID = 1,
     .reg_base = AR_IO_DAC1_SET,
     .tx_vector = IO_DAC1_INT_IRQ,
     .err_vector = IO_DAC1_INT_ERR,
     .tx_isr = dac1_tx_ISR,
     .err_isr = dac1_err_ISR,
     .dmatx_isr = dac1_dmatx_ISR,
#ifdef IO_DAC1_DMA
     .dmachanid = IO_DAC1_DMA,
#else
     .dmachanid = DMA_NONE,
#endif
     .fifosize = IO_DAC1_FS,
     .fifowidth = IO_DAC1_FW},
#endif
#ifdef IO_DAC2_PRESENT
    {.instID = 2,
     .reg_base = AR_IO_DAC2_SET,
     .tx_vector = IO_DAC2_INT_IRQ,
     .err_vector = IO_DAC2_INT_ERR,
     .tx_isr = dac2_tx_ISR,
     .err_isr = dac2_err_ISR,
     .dmatx_isr = dac2_dmatx_ISR,
#ifdef IO_DAC2_DMA
     .dmachanid = IO_DAC2_DMA,
#else
     .dmachanid = DMA_NONE,
#endif
     .fifosize = IO_DAC2_FS,
     .fifowidth = IO_DAC2_FW},
#endif
#ifdef IO_DAC3_PRESENT
    {.instID = 3,
     .reg_base = AR_IO_DAC3_SET,
     .tx_vector = IO_DAC3_INT_IRQ,
     .err_vector = IO_DAC3_INT_ERR,
     .tx_isr = dac3_tx_ISR,
     .err_isr = dac3_err_ISR,
     .dmatx_isr = dac3_dmatx_ISR,
#ifdef IO_DAC3_DMA
     .dmachanid = IO_DAC3_DMA,
#else
     .dmachanid = DMA_NONE,
#endif
     .fifosize = IO_DAC3_FS,
     .fifowidth = IO_DAC3_FW},
#endif
    {.instID = DAC_MAX_CNT}
};


#define REG_WRITE( reg, x )   _sr( (unsigned)(x), (unsigned)(dev->reg_base + reg) )
#define REG_READ( reg )       _lr( (unsigned)(dev->reg_base + reg) )

uint32_t io_dac_open(uint32_t dev_id)
{
    uint32_t h = 0;
    dac_info_pt dev;

    h = 0;
    while ((dac_devs[h].instID != dev_id)
       && (dac_devs[h].instID != DAC_MAX_CNT)) {
    h++;
    }
    if ((dac_devs[h].instID == DAC_MAX_CNT) || (0 != dac_handles[dev_id])) {    /* dev_id not part of design, or still open */
    return 1;
    }
    dac_handles[dev_id] = &dac_devs[h];
    dev = dac_handles[dev_id];

    dev->bufcount = 0;
    dev->tx_len = 0;

#ifdef __Xdmac
    dev->dmadescriptor = &dac_dma_descriptors[8 * h];

    /* workaround for lack of channel-reset */
    if (dev->dmastarted != 1) {
    dev->dmastarted = 0;
    dev->dmaidx = 0;
    }

    if (dev->dmachanid != DMA_NONE) {
    dev->dmadescriptor[2] = dev->dmadescriptor[6] =
        dev->reg_base + DAC_TX_DATA;
    dev->dmadescriptor[3] = (uint32_t) & (dev->dmadescriptor[4]);
    dev->dmadescriptor[7] = (uint32_t) & (dev->dmadescriptor[0]);
    }

    if (dev->dmachanid != DMA_NONE) {
    /* just enable the IP clock; mask threshold interrupt */
    REG_WRITE(DAC_CTRL, DAC_CLK_ENABLE);
    REG_WRITE(DAC_CTRL, DAC_CLK_ENABLE | DAC_IRQ_MSK_THRESHOLD);

    _setvecti(DMAC_INT_BASE + dev->dmachanid, dev->dmatx_isr);
    _setvecti(DMAC_ERR_BASE + dev->dmachanid, dev->err_isr);
    } else {
#endif
    /* just enable the IP clock; it's up to the user to enable the ADs */
    REG_WRITE(DAC_CTRL, DAC_CLK_ENABLE);
#ifdef __Xdmac
    }
#endif

    _setvecti(dev->tx_vector, dev->tx_isr);
    _setvecti(dev->err_vector, dev->err_isr);

    return 0;
}


void io_dac_close(uint32_t dev_id)
{
    dac_info_pt dev = dac_handles[dev_id];
    uint32_t reg_val = 0;

    /* flush fifo */
    REG_WRITE(DAC_SET, DAC_FLUSH_TX);
    /* reset */
    reg_val = REG_READ(DAC_CTRL);
    REG_WRITE(DAC_CTRL, (reg_val | 0x1));
    /* disable clock */
    REG_WRITE(DAC_CTRL, 0);

    dev->bufcount = 0;
    dev->tx_len = 0;

    _setvecti(dev->tx_vector, NULL);
    _setvecti(dev->err_vector, NULL);

#ifdef __Xdmac
    _setvecti(DMAC_INT_BASE + dev->dmachanid, NULL);
    _setvecti(DMAC_ERR_BASE + dev->dmachanid, NULL);
    _dma_chan_enable((0x1 << dev->dmachanid), 0);
#endif

    dev->tx_cb = NULL;
    dev->err_cb = NULL;

    dac_handles[dev_id] = 0;
}


void io_dac_write(uint32_t dev_id, uint32_t * data, uint32_t * size)
{
    dac_info_pt dev = dac_handles[dev_id];
    uint32_t reg_val = 0;
    uint32_t level = 0;
    uint32_t k = 0;

    if (dev->bufcount == 0) {   /* first data - start tx */
    dev->buffer[0] = data;
    dev->bufsize[0] = size;
    dev->bufcount++;

#ifdef __Xdmac
    if (dev->dmachanid == DMA_NONE) {
#endif
        dev->tx_len = 0;

        /* write data info FIFO */
        level = REG_READ(DAC_INTSTAT);
        level = (level & DAC_TXFLR_MSK) >> DAC_TXFLR_POS;
        for (k = level;
         ((k != dev->fifosize)
          && (dev->tx_len != *(dev->bufsize[0]))); k++) {
        REG_WRITE(DAC_TX_DATA, data[dev->tx_len++]);
        }

        if (dev->tx_len == *(dev->bufsize[0])) {    /* buffer has been processed */
        if (dev->tx_cb != NULL) {
            dev->tx_cb(dev_id);
        }
        dev->bufcount--;
        dev->tx_len = 0;
        }

        /* enable TX and enable interrupts, if not already done */
        reg_val =
        REG_READ(DAC_CTRL) & ~DAC_IRQ_MSK_THRESHOLD &
        ~DAC_IRQ_MSK_EMPTY & ~DAC_IRQ_MSK_UNDERFLOW;
        reg_val |= DAC_TX_ENABLE;
        REG_WRITE(DAC_CTRL, reg_val);
#ifdef __Xdmac
    } else {        /* DMA: create descriptor */
        uint32_t threshold = (REG_READ(DAC_SET) & (0x3f << 24)) >> 24;

        if (*size > 1) {
        /* write first element in fifo and remainder by DMA */
        REG_WRITE(DAC_TX_DATA, data[0]);

        dev->dmadescriptor[dev->dmaidx * 4 + 0] =
            DAC_DMA_CTRL | ((threshold + 1) <<
                    DAC_DMA_CTRL_XFER_POS) | (((*size - 1)
                                   * 4 -
                                   1) <<
                                  DAC_DMA_CTRL_SIZE_POS);
        dev->dmadescriptor[dev->dmaidx * 4 + 1] =
            (uint32_t) (&data[1]) + ((*size - 2) * 4);

        if (dev->dmachanid < DMAC_MEM_CHAN_CNT) {   /* start mem based DMA , if not done yet */
            if (dev->dmastarted == 0) {
            dev->dmastarted = 1;

            _dma_chan_desc(dev->dmachanid,
                       &(dev->
                     dmadescriptor[dev->dmaidx * 4]));

            /* invalidate this descriptor is the DMAC will not do this */
            dev->dmadescriptor[dev->dmaidx * 4] = 0;
            }
        } else {    /* init aux based DMA */
            dev->dmaactiveidx = dev->dmaidx;
            _dma_chan_desc_aux(dev->dmachanid,
                       &(dev->
                     dmadescriptor[dev->dmaactiveidx *
                               4]));
        }

        dev->dmaidx = 1 - dev->dmaidx;

        _dma_chan_enable((0x1 << dev->dmachanid), 1);

        /* enable TX, enable error interrupt, disable empty interrupt, if not already done */
        reg_val = REG_READ(DAC_CTRL) & ~DAC_IRQ_MSK_UNDERFLOW;
        reg_val |= (DAC_TX_ENABLE | DAC_IRQ_MSK_EMPTY);
        REG_WRITE(DAC_CTRL, reg_val);
        } else {        /* *size == 1, put in empty fifo and do not use DMA */
        REG_WRITE(DAC_TX_DATA, data[0]);

        if (dev->tx_cb != NULL) {
            dev->tx_cb(dev_id);
        }
        dev->bufcount--;

        /* enable TX, enable error interrupt, enable empty interrupt, if not already done */
        reg_val =
            REG_READ(DAC_CTRL) & ~DAC_IRQ_MSK_THRESHOLD &
            ~DAC_IRQ_MSK_EMPTY & ~DAC_IRQ_MSK_UNDERFLOW;
        reg_val |= DAC_TX_ENABLE;
        REG_WRITE(DAC_CTRL, reg_val);
        }
    }
#endif
    } else if (dev->bufcount != 2) {    /* new buffer */
    dev->buffer[dev->bufcount] = data;
    dev->bufsize[dev->bufcount] = size;
    dev->bufcount++;

#ifdef __Xdmac
    if (dev->dmachanid != DMA_NONE) {   /* DMA: create descriptor */
        uint32_t threshold = (REG_READ(DAC_SET) & (0x3f << 24)) >> 24;

        dev->dmadescriptor[dev->dmaidx * 4 + 0] =
        DAC_DMA_CTRL | ((threshold + 1) << DAC_DMA_CTRL_XFER_POS) |
        ((*size * 4 - 1) << DAC_DMA_CTRL_SIZE_POS);
        dev->dmadescriptor[dev->dmaidx * 4 + 1] =
        (uint32_t) data + ((*size - 1) * 4);

        dev->dmaidx = 1 - dev->dmaidx;

        _dma_chan_enable((0x1 << dev->dmachanid), 1);
    }
#endif
    }
}


void io_dac_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
    dac_info_pt dev = dac_handles[dev_id];
    uint32_t reg_val = 0;

    switch (cmd) {
    case IO_SET_CB_TX:
    dev->tx_cb = ((io_cb_t *) arg)->cb;
    break;

    case IO_SET_CB_ERR:
    dev->err_cb = ((io_cb_t *) arg)->cb;
    break;

    case IO_DAC_SET_DAC_ENA:
    reg_val = REG_READ(DAC_CTRL);
    reg_val &= ~(0x1 << 1);
    reg_val |= (*((uint32_t *) arg) << 1);
    REG_WRITE(DAC_CTRL, reg_val);
    break;

    case IO_DAC_SET_THRESHOLD:
    reg_val = REG_READ(DAC_SET);
    reg_val &= ~(0x3f << 24);
    reg_val |= (*((uint32_t *) arg) << 24);
    REG_WRITE(DAC_SET, reg_val);
    break;

    case IO_DAC_SET_TX_EDGE:
    reg_val = REG_READ(DAC_SET);
    reg_val &= ~(0x1 << 6);
    reg_val |= (*((uint32_t *) arg) << 6);
    REG_WRITE(DAC_SET, reg_val);
    break;

    default:
    break;
    }
}


static void dac_tx_ISR_proc(uint32_t dev_id)
{
    dac_info_pt dev = dac_handles[dev_id];
    uint32_t status = 0;
    uint32_t reg_val = 0;
    uint32_t level = 0, k = 0;

    if (dev->bufcount == 0) {
    /* no data */
    status = REG_READ(DAC_INTSTAT) & 0xf;
    status &= ~(REG_READ(DAC_CTRL) >> 8);
    if (status & DAC_IRQ_THRESHOLD) {
        /* mask and clear threshold interrupt */
        reg_val = REG_READ(DAC_CTRL);
        reg_val |= DAC_IRQ_MSK_THRESHOLD | DAC_IRQ_CLR_THRESHOLD;
        REG_WRITE(DAC_CTRL, reg_val);
    }
    if (status & DAC_IRQ_EMPTY) {
        /* notify end of transmission to the application */
        if (dev->tx_cb != NULL) {
        dev->tx_cb(dev_id);
        }
        /* mask and clear threshold (also in DMA case, does not hurt) and empty interrupt */
        reg_val = REG_READ(DAC_CTRL);
        reg_val |=
        DAC_IRQ_MSK_THRESHOLD | DAC_IRQ_CLR_THRESHOLD |
        DAC_IRQ_MSK_EMPTY | DAC_IRQ_CLR_EMPTY;
        REG_WRITE(DAC_CTRL, reg_val);
    }
    } else {            /* at least one (non-empty) buffer available */
#ifdef __Xdmac
    if (dev->dmachanid == DMA_NONE) {
#endif
        level = REG_READ(DAC_INTSTAT);
        level = (level & DAC_TXFLR_MSK) >> DAC_TXFLR_POS;
        for (k = level;
         ((k != dev->fifosize)
          && (dev->tx_len != *(dev->bufsize[0]))); k++) {
        REG_WRITE(DAC_TX_DATA, dev->buffer[0][dev->tx_len++]);
        }

        if (dev->tx_len == *(dev->bufsize[0])) {    /* buffer has been processed */
        if (dev->tx_cb != NULL) {
            dev->tx_cb(dev_id);
        }
        dev->buffer[0] = dev->buffer[1];
        dev->bufsize[0] = dev->bufsize[1];
        dev->bufcount--;
        dev->tx_len = 0;
        }
#ifdef __Xdmac
    }
#endif
    /* clear threshold and empty interrupt */
    reg_val = REG_READ(DAC_CTRL);
    reg_val |= DAC_IRQ_CLR_THRESHOLD | DAC_IRQ_CLR_EMPTY;
    REG_WRITE(DAC_CTRL, reg_val);
    }
}


static void dac_err_ISR_proc(uint32_t dev_id)
{
    dac_info_pt dev = dac_handles[dev_id];
    uint32_t reg_val = 0;

    /* must be an underflow error: notify application and disable TX */
    if (dev->err_cb != NULL) {
    dev->err_cb(dev_id);
    }

    /* reset */
    reg_val = REG_READ(DAC_CTRL);
    REG_WRITE(DAC_CTRL, (reg_val | 0x1));
    /* disable clock */
    REG_WRITE(DAC_CTRL, 0);
}

static void dac_dmatx_ISR_proc(uint32_t dev_id)
{
#ifdef __Xdmac
    dac_info_pt dev = dac_handles[dev_id];
    uint32_t reg_val = 0;

    /* at least one (non-empty) buffer available: dev->bufcount > 0 */
    if (dev->tx_cb != NULL) {
    dev->tx_cb(dev_id);
    }
    dev->buffer[0] = dev->buffer[1];
    dev->bufsize[0] = dev->bufsize[1];
    dev->bufcount--;
    if (dev->bufcount == 0) {
    _dma_chan_enable((0x1 << dev->dmachanid), 0);

    /* enable empty interrupt */
    reg_val = REG_READ(DAC_CTRL) & ~DAC_IRQ_MSK_EMPTY;
    REG_WRITE(DAC_CTRL, reg_val);
    } else if (dev->dmachanid >= DMAC_MEM_CHAN_CNT) {
    /* processing not completed - program next descriptor for aux based dma */
    _dma_chan_enable((0x1 << dev->dmachanid), 0);
    _dma_int_clear((0x1 << dev->dmachanid));

    dev->dmaactiveidx = 1 - dev->dmaactiveidx;
    _dma_chan_desc_aux(dev->dmachanid,
               &(dev->dmadescriptor[dev->dmaactiveidx * 4]));

    _dma_chan_enable((0x1 << dev->dmachanid), 1);
    }

    _dma_int_clear((0x1 << dev->dmachanid));
#endif
}

#endif
