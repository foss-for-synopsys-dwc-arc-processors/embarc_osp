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
#include "pdm_rx.h"
#include "io_config.h"
#include "apexextensions.h"

/* PDM Rx device register offsets */
#define PDM_EN      0x0000
#define PDM_REN     0x0004
#define PDM_CER     0x000c
#define PDM_RXFFR   0x0014
#define PDM_RER0    0x0028
#define PDM_ISR     0x0038
#define PDM_IMR     0x003c
#define PDM_ROR     0x0040
#define PDM_RFCR    0x0048
#define PDM_RER1    0x0068
#define PDM_RER2    0x00a8
#define PDM_RER3    0x00e8
#define PDM_RXDMA   0x01c0
#define PDM_RR      0x01d0
#define PDM_CIC_N   0x01d4
#define PDM_CIC_D   0x01d8
#define PDM_DCRC    0x01dc
#define PDM_BRC_B0  0x01e0
#define PDM_BRC_B1  0x01e4
#define PDM_BRC_B2  0x01e8
#define PDM_BRC_B3  0x01ec
#define PDM_BRC_CLP 0x01f0

/* PDM Rx device DMA realted macros */
#define PDM_RX_DMA_CTRL             (0xa0000077)    /* am=b10, i=b1, dw/inc=b011, dtt=b10, r=b1, op=b11 */
#define PDM_RX_DMA_CTRL_SIZE_POS    (8)
#define PDM_RX_DMA_CTRL_XFER_POS    (21)

#define DMA_NONE                    (0xff)

/* PDM deffered configuration request masks */
#define PDM_CFG_REQ_RER0    (0x01000000)
#define PDM_CFG_REQ_RER1    (0x02000000)
#define PDM_CFG_REQ_RER2    (0x04000000)
#define PDM_CFG_REQ_RER3    (0x08000000)
#define PDM_CFG_REQ_CIC_N   (0x10000000)
#define PDM_CFG_REQ_CIC_D   (0x20000000)
#define PDM_CFG_REQ_RR      (0x40000000)
#define PDM_CFG_REQ_DCRC    (0x80000000)

/* PDM deffered configuration value masks */
#define PDM_CFG_MSK_REQ     (0xFF000000)    // full request mask
#define PDM_CFG_MSK_REQEN   (0x70000000)    // REN required request mask
#define PDM_CFG_MSK_RER0    (0x00000001)    // 1
#define PDM_CFG_MSK_RER1    (0x00000002)    // 1
#define PDM_CFG_MSK_RER2    (0x00000004)    // 1
#define PDM_CFG_MSK_RER3    (0x00000008)    // 1
#define PDM_CFG_MSK_CIC_N   (0x00000010)    // 1
#define PDM_CFG_MSK_CIC_D   (0x00000020)    // 1
#define PDM_CFG_MSK_RR      (0x00000F00)    // 4
#define PDM_CFG_MSK_DCRC    (0x0000F000)    // 4

/* PDM deffered configuration shift values */
#define PDM_CFG_SFT_RER0    (0x00000000)    //  0
#define PDM_CFG_SFT_RER1    (0x00000001)    //  1
#define PDM_CFG_SFT_RER2    (0x00000002)    //  2
#define PDM_CFG_SFT_RER3    (0x00000003)    //  3
#define PDM_CFG_SFT_CIC_D   (0x00000004)    //  4
#define PDM_CFG_SFT_CIC_N   (0x00000005)    //  5
#define PDM_CFG_SFT_RR      (0x00000008)    //  8..11
#define PDM_CFG_SFT_DCRC    (0x0000000c)    // 12..15

/* PDM driver internal types */

typedef _Interrupt void (*ISR) ();

typedef struct pdm_rx_info_s {
    /* PDM HW configuration */
    const uint32_t reg_base;    /* base address of device register set */
    const uint8_t instID;
    const uint8_t chan_max; /* available number of stereo channels */
    const uint8_t fifo_size;    /* fifo size                             */
    const uint8_t dmachanid;
    /* Data buffer related fields */
    uint32_t xfr_chn;       /* number of channels enabled */
    uint32_t xfr_len;
    uint32_t xfr_started;
    uint32_t *buffer[2];
    uint32_t *bufsize[2];
    volatile uint32_t usr_cnt;
    volatile uint32_t sys_cnt;
#ifdef __Xdmac
    _Uncached uint32_t *dmadescriptor;
#endif
    /* Callbacks */
    IO_CB_FUNC rx_cb;
    IO_CB_FUNC err_cb;
    IO_CB_FUNC clip_cb;
    /* Interrupt numbers and handlers */
    const uint8_t rx_vector;    /* Receive data available interrupt vector */
    const uint8_t err_vector;   /* Receive data overrun error interrupt vector */
    const uint8_t clip_vector;  /* Clipping vector */
    ISR rx_isr;         /* PDM data available ISR */
    ISR err_isr;        /* PDM FIFO overrun ISR */
    ISR clip_isr;       /* PDM data clipping ISR */
} pdm_rx_info_t, *pdm_rx_info_pt;

#ifdef IO_PDM_RX0_PRESENT
#define PDM_RX_DEV_PRESENT
#endif
#ifdef IO_PDM_RX1_PRESENT
#define PDM_RX_DEV_PRESENT
#endif
#ifdef IO_PDM_RX2_PRESENT
#define PDM_RX_DEV_PRESENT
#endif
#ifdef IO_PDM_RX3_PRESENT
#define PDM_RX_DEV_PRESENT
#endif
#ifdef IO_PDM_RX4_PRESENT
#define PDM_RX_DEV_PRESENT
#endif

#ifdef PDM_RX_DEV_PRESENT

#define PDM_RX_MAX_CNT  (4)

static void pdm_rx_ISR_proc(uint32_t dev_id);
static void pdm_rx_err_ISR_proc(uint32_t dev_id);
static void pdm_rx_clip_ISR_proc(uint32_t dev_id);

#ifdef IO_PDM_RX0_PRESENT
static _Interrupt void pdm_rx0_rx_ISR()
{
    pdm_rx_ISR_proc(0);
}

static _Interrupt void pdm_rx0_err_ISR()
{
    pdm_rx_err_ISR_proc(0);
}

static _Interrupt void pdm_rx0_clip_ISR()
{
    pdm_rx_clip_ISR_proc(0);
}
#endif
#ifdef IO_PDM_RX1_PRESENT
static _Interrupt void pdm_rx1_rx_ISR()
{
    pdm_rx_ISR_proc(1);
}

static _Interrupt void pdm_rx1_err_ISR()
{
    pdm_rx_err_ISR_proc(1);
}

static _Interrupt void pdm_rx1_clip_ISR()
{
    pdm_rx_clip_ISR_proc(1);
}
#endif
#ifdef IO_PDM_RX2_PRESENT
static _Interrupt void pdm_rx2_rx_ISR()
{
    pdm_rx_ISR_proc(2);
}

static _Interrupt void pdm_rx2_err_ISR()
{
    pdm_rx_err_ISR_proc(2);
}

static _Interrupt void pdm_rx2_clip_ISR()
{
    pdm_rx_clip_ISR_proc(2);
}
#endif
#ifdef IO_PDM_RX3_PRESENT
static _Interrupt void pdm_rx3_rx_ISR()
{
    pdm_rx_ISR_proc(3);
}

static _Interrupt void pdm_rx3_err_ISR()
{
    pdm_rx_err_ISR_proc(3);
}

static _Interrupt void pdm_rx3_clip_ISR()
{
    pdm_rx_clip_ISR_proc(3);
}
#endif

/* PDM devices private data structures */
static pdm_rx_info_pt pdm_rx_handles[PDM_RX_MAX_CNT] = { 0 };

#ifdef __Xdmac
static __attribute__ ((aligned(32)))
_Uncached uint32_t pdm_rx_dma_descriptors[8 * PDM_RX_MAX_CNT];  // pull of PDM buffer descriptors
#endif

static pdm_rx_info_t pdm_rx_devs[] = {
#ifdef IO_PDM_RX0_PRESENT
    {
     .instID = 0,
     .reg_base = AR_IO_PDM_RX0_PDM_EN,
     .chan_max = IO_PDM_RX0_CH,
     .rx_vector = IO_PDM_RX0_INT_RX_AVAIL,
     .err_vector = IO_PDM_RX0_INT_ERR,
     .clip_vector = IO_PDM_RX0_INT_CLP,
     .rx_isr = pdm_rx0_rx_ISR,
     .err_isr = pdm_rx0_err_ISR,
     .clip_isr = pdm_rx0_clip_ISR,
#ifdef IO_PDM_RX0_DMA
     .dmachanid = IO_PDM_RX0_DMA,
#else
     .dmachanid = DMA_NONE,
#endif
     .fifo_size = IO_PDM_RX0_FS,
     },
#endif
#ifdef IO_PDM_RX1_PRESENT
    {
     .instID = 1,
     .reg_base = AR_IO_PDM_RX1_PDM_EN,
     .chan_max = IO_PDM_RX1_CH,
     .rx_vector = IO_PDM_RX1_INT_RX_AVAIL,
     .err_vector = IO_PDM_RX1_INT_ERR,
     .rx_isr = pdm_rx1_rx_ISR,
     .err_isr = pdm_rx1_err_ISR,
     .clip_isr = pdm_rx1_clip_ISR,
#ifdef IO_PDM_RX1_DMA
     .dmachanid = IO_PDM_RX1_DMA,
#else
     .dmachanid = DMA_NONE,
#endif
     .fifo_size = IO_PDM_RX1_FS,
     },
#endif
#ifdef IO_PDM_RX2_PRESENT
    {
     .instID = 2,
     .reg_base = AR_IO_PDM_RX2_PDM_EN,
     .chan_max = IO_PDM_RX2_CH,
     .rx_vector = IO_PDM_RX2_INT_RX_AVAIL,
     .err_vector = IO_PDM_RX2_INT_ERR,
     .rx_isr = pdm_rx2_rx_ISR,
     .err_isr = pdm_rx2_err_ISR,
     .clip_isr = pdm_rx2_clip_ISR,
#ifdef IO_PDM_RX2_DMA
     .dmachanid = IO_PDM_RX2_DMA,
#else
     .dmachanid = DMA_NONE,
#endif
     .fifo_size = IO_PDM_RX2_FS,
     },
#endif
#ifdef IO_PDM_RX3_PRESENT
    {
     .instID = 3,
     .reg_base = AR_IO_PDM_RX3_PDM_EN,
     .chan_max = IO_PDM_RX3_CH,
     .rx_vector = IO_PDM_RX3_INT_RX_AVAIL,
     .err_vector = IO_PDM_RX3_INT_ERR,
     .rx_isr = pdm_rx3_rx_ISR,
     .err_isr = pdm_rx3_err_ISR,
     .clip_isr = pdm_rx3_clip_ISR,
#ifdef IO_PDM_RX3_DMA
     .dmachanid = IO_PDM_RX3_DMA,
#else
     .dmachanid = DMA_NONE,
#endif
     .fifo_size = IO_PDM_RX3_FS,
     },
#endif
    {
     .instID = PDM_RX_MAX_CNT}
};

#define     REG_WRITE( reg, x )   _sr( (uint32_t)((x)), (uint32_t)(dev->reg_base + reg) )
#define     REG_READ( reg )       _lr( (uint32_t)(dev->reg_base + (reg)) )

/* API functions */
uint32_t io_pdm_rx_open(uint32_t dev_id)
{
    pdm_rx_info_pt dev;
    uint32_t h = 0;

    while ((pdm_rx_devs[h].instID != dev_id)
       && (pdm_rx_devs[h].instID != PDM_RX_MAX_CNT)) {
    h++;
    }
    if ((pdm_rx_devs[h].instID == PDM_RX_MAX_CNT) || (0 != pdm_rx_handles[dev_id])) {   /* dev_id not part of design, or still open */
    return 1;
    }
    pdm_rx_handles[dev_id] = &pdm_rx_devs[h];
    dev = pdm_rx_handles[dev_id];

    dev->xfr_chn = dev->chan_max;   /* all available channels are enabled by default */
    dev->xfr_started = 0;
    dev->xfr_len = 0;
    dev->usr_cnt = dev->sys_cnt = 0;    // No buffers available for the system

#ifdef __Xdmac
    dev->dmadescriptor = &pdm_rx_dma_descriptors[8 * h];

    if (dev->dmachanid != DMA_NONE) {
    dev->dmadescriptor[0] = dev->dmadescriptor[4] = 0;
    dev->dmadescriptor[1] = dev->dmadescriptor[5] =
        dev->reg_base + PDM_RXDMA;
    dev->dmadescriptor[2] = dev->dmadescriptor[6] = 0;
    dev->dmadescriptor[3] = (uint32_t) & (dev->dmadescriptor[4]);
    dev->dmadescriptor[7] = (uint32_t) & (dev->dmadescriptor[0]);

    _setvecti(DMAC_INT_BASE + dev->dmachanid, dev->rx_isr);
    _setvecti(DMAC_ERR_BASE + dev->dmachanid, dev->err_isr);    // ????
    _setvecti(dev->err_vector, dev->err_isr);
    _setvecti(dev->clip_vector, dev->clip_isr);
    } else {
#endif
    _setvecti(dev->rx_vector, dev->rx_isr);
    _setvecti(dev->err_vector, dev->err_isr);
    _setvecti(dev->clip_vector, dev->clip_isr);
#ifdef __Xdmac
    }
#endif

    REG_WRITE(PDM_EN, 0x01);    // PDM: enable device
    while ((REG_READ(PDM_EN) & 0x0002) == 0)    // whait for PDM enable readback bit
    {
    }
    return (0);
}


void io_pdm_rx_close(uint32_t dev_id)
{
    pdm_rx_info_pt dev = pdm_rx_handles[dev_id];

#ifdef __Xdmac
    _dma_chan_enable(0x1 << dev->dmachanid, 0); // aux-DMA channel reset bug-fix
    _dma_chan_reset(0x1 << dev->dmachanid);

    _setvecti(DMAC_INT_BASE + dev->dmachanid, NULL);
    _setvecti(DMAC_ERR_BASE + dev->dmachanid, NULL);
#endif

    REG_WRITE(PDM_RXFFR, 0x0001);   // PDM: Reset Rx FIFO
    REG_WRITE(PDM_CER, 0x0000); // PDM: disable clock
    /* restore default registers values */
    REG_WRITE(PDM_DCRC, 0x0000);
    REG_WRITE(PDM_CIC_D, 0x0001);
    REG_WRITE(PDM_CIC_N, 0x0001);
    REG_WRITE(PDM_BRC_CLP, 0x0000);
    switch (dev->chan_max)  // PDM: restore default channel settings
    {
    case 4:
    REG_WRITE(PDM_RER3, 1);
    REG_WRITE(PDM_BRC_B3, 0);   /* falls through */
    case 3:
    REG_WRITE(PDM_RER2, 1);
    REG_WRITE(PDM_BRC_B2, 0);   /* falls through */
    case 2:
    REG_WRITE(PDM_RER1, 1);
    REG_WRITE(PDM_BRC_B1, 0);   /* falls through */
    default:
    REG_WRITE(PDM_RER0, 1);
    REG_WRITE(PDM_BRC_B0, 0);
    }
    REG_WRITE(PDM_REN, 0x0000); // PDM: disable Rx
    REG_WRITE(PDM_EN, 0x0000);  // PDM: disable device

    _setvecti(dev->rx_vector, NULL);
    _setvecti(dev->err_vector, NULL);
    _setvecti(dev->clip_vector, NULL);


    dev->rx_cb = NULL;
    dev->err_cb = NULL;
    dev->clip_cb = NULL;

    pdm_rx_handles[dev_id] = 0;
}

void io_pdm_rx_read(uint32_t dev_id, uint32_t * data, uint32_t * size)
{
    pdm_rx_info_pt dev = pdm_rx_handles[dev_id];

    uint32_t idx = dev->usr_cnt;    // copy user access counter to the temporary variable

    if (((idx - dev->sys_cnt) != 2) && (*size != 0)) {
    idx = idx & 0x0001; // current index in dev->buffer[] & dev->bufsize[]
#ifdef __Xdmac
    if (dev->dmachanid != DMA_NONE) {
        uint32_t burst_size = (REG_READ(PDM_RFCR) + 1) << 1;    // actual FIFO trigger level
        uint32_t xfer_size = (dev->xfr_chn * (*size)) << 2; // transfer size in bytes (ToDo: limited by 8KB)
        idx = idx << 2; // calculate available buffer' index

        /* DMA: update DMACTRL and DMADAR part of descriptor */
        dev->dmadescriptor[idx + 0] = PDM_RX_DMA_CTRL |
        (burst_size << PDM_RX_DMA_CTRL_XFER_POS) |
        ((xfer_size - 1) << PDM_RX_DMA_CTRL_SIZE_POS);
        dev->dmadescriptor[idx + 2] = (uint32_t) data + xfer_size - 2;

        if (dev->xfr_started == 0) {    /* PDM interface had not been started yet */
        dev->xfr_started = 1;

        if (dev->dmachanid < DMAC_MEM_CHAN_CNT) {   /* memDMA: init mem based DMA if required */
            _dma_chan_desc(dev->dmachanid,
                   &(dev->dmadescriptor[0]));
        } else {    /* auxDMA: init aux based DMA if required */
            _dma_chan_desc_aux(dev->dmachanid,
                       &(dev->dmadescriptor[0]));
        }

        REG_WRITE(PDM_RXFFR, 0x0001);   // PDM: Reset Rx FIFO
        REG_WRITE(PDM_CER, 0x0001); // PDM: enable clock
        REG_WRITE(PDM_REN, 0x0001); // PDM: enable receiver
        REG_WRITE(PDM_IMR, 0x0003); // PDM: Unmask clipping ISRs only

        /* Start DMA channel */
        _dma_chan_enable((0x1 << dev->dmachanid), 1);
        }
    } else {
#endif
        /* Update data buffer parameters */
        dev->buffer[idx] = data;
        dev->bufsize[idx] = size;   // size value in multicahnnel samples!!!

        if (dev->xfr_started == 0) {    /* I2S transfer not yet started -> init I2S Rx */
        dev->xfr_started = 1;
        /* PDM interface had not been started yet */
        REG_WRITE(PDM_RXFFR, 0x0001);   // PDM: Reset Rx FIFO
        REG_WRITE(PDM_CER, 0x0001); // PDM: enable clock
        REG_WRITE(PDM_REN, 0x0001); // PDM: enable receiver
        REG_WRITE(PDM_IMR, 0x0000); // PDM: unmask all RX interrupts
        }
#ifdef __Xdmac
    }
#endif
    dev->usr_cnt++;     // update number of buffers available for system
    }
}

void io_pdm_rx_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
    pdm_rx_info_pt dev = pdm_rx_handles[dev_id];
    uint32_t reg_addr = 0;
    uint32_t reg_val = 0;

    switch (cmd) {
    case IO_SET_CB_RX:
    dev->rx_cb = ((io_cb_t *) arg)->cb;
    break;
    case IO_SET_CB_ERR:
    dev->err_cb = ((io_cb_t *) arg)->cb;
    break;
    case IO_SET_CB_CLIP:
    dev->clip_cb = ((io_cb_t *) arg)->cb;
    break;

    /*
     ** PDM device specific I/O controls
     */

    /*
       ToDo: cahnnel enable/disable - select between two APIs:
       ioctl( dev_id, IO_PDM_RX_CHAN_ENABLE, [0..3] )
       ioctl( dev_id, IO_PDM_RX_CHAN_DISABLE, [0..3] )
       or
       ioctl( dev_id, IO_PDM_RX_SET_RERx, [0|1] )
       Note: now both APIs are implemented!!!
     */

    // ioctl( dev_id, IO_PDM_RX_CHAN_ENABLE, [0..3] )
    case IO_PDM_RX_CHAN_ENABLE:
    if (*(uint32_t *) arg < dev->chan_max) {
        switch (*(uint32_t *) arg) {
        case 3:
        reg_addr = PDM_RER3;
        break;
        case 2:
        reg_addr = PDM_RER2;
        break;
        case 1:
        reg_addr = PDM_RER1;
        break;
        default:
        reg_addr = PDM_RER0;
        break;
        }
        reg_val = REG_READ(reg_addr);
        if (reg_val == 0) {
        REG_WRITE(reg_addr, 1);
        dev->xfr_chn += 1;
        }
    }
    break;
    // ioctl( dev_id, IO_PDM_RX_CHAN_DISABLE, [0..3] )
    case IO_PDM_RX_CHAN_DISABLE:
    if (*(uint32_t *) arg < dev->chan_max) {
        switch (*(uint32_t *) arg) {
        case 3:
        reg_addr = PDM_RER3;
        break;
        case 2:
        reg_addr = PDM_RER2;
        break;
        case 1:
        reg_addr = PDM_RER1;
        break;
        default:
        reg_addr = PDM_RER0;
        break;
        }
        reg_val = REG_READ(reg_addr);
        if (reg_val == 1) {
        REG_WRITE(reg_addr, 0);
        dev->xfr_chn -= 1;
        }
    }
    break;
    // ioctl( dev_id, IO_PDM_RX_SET_RERx, [0|1] )
    case IO_PDM_RX_SET_RER0:    // enable/disable stereo channel #0
    reg_val = REG_READ(PDM_RER0);
    if ((*(uint32_t *) arg == 0) && (reg_val == 1)) {
        REG_WRITE(PDM_RER0, 0);
        dev->xfr_chn -= 1;
    }
    if ((*(uint32_t *) arg != 0) && (reg_val == 0)) {
        REG_WRITE(PDM_RER0, 1);
        dev->xfr_chn += 1;
    }
    break;
    case IO_PDM_RX_SET_RER1:    // enable/disable stereo channel #1
    reg_val = REG_READ(PDM_RER1);
    if ((*(uint32_t *) arg == 0) && (reg_val == 1)) {
        REG_WRITE(PDM_RER1, 0);
        dev->xfr_chn -= 1;
    }
    if ((*(uint32_t *) arg != 0) && (reg_val == 0)) {
        REG_WRITE(PDM_RER1, 1);
        dev->xfr_chn += 1;
    }
    break;
    case IO_PDM_RX_SET_RER2:    // enable/disable stereo channel #2
    reg_val = REG_READ(PDM_RER2);
    if ((*(uint32_t *) arg == 0) && (reg_val == 1)) {
        REG_WRITE(PDM_RER2, 0);
        dev->xfr_chn -= 1;
    }
    if ((*(uint32_t *) arg != 0) && (reg_val == 0)) {
        REG_WRITE(PDM_RER2, 1);
        dev->xfr_chn += 1;
    }
    break;
    case IO_PDM_RX_SET_RER3:    // enable/disable stereo channel #3
    reg_val = REG_READ(PDM_RER3);
    if ((*(uint32_t *) arg == 0) && (reg_val == 1)) {
        REG_WRITE(PDM_RER3, 0);
        dev->xfr_chn -= 1;
    }
    if ((*(uint32_t *) arg != 0) && (reg_val == 0)) {
        REG_WRITE(PDM_RER3, 1);
        dev->xfr_chn += 1;
    }
    break;
    case IO_PDM_RX_SET_RR:  // set oversampling ratio (RR)
    REG_WRITE(PDM_RR, *(uint32_t *) arg);
    break;
    case IO_PDM_RX_SET_CIC_N:   // set number of stages
    REG_WRITE(PDM_CIC_N, *(uint32_t *) arg);
    break;
    case IO_PDM_RX_SET_CIC_D:   // set CIC delay
    REG_WRITE(PDM_CIC_D, *(uint32_t *) arg);
    break;
    case IO_PDM_RX_SET_DCRC:    // DC removal control
    REG_WRITE(PDM_DCRC, *(uint32_t *) arg);
    break;
    /*
     ** IMMEDIATE controls
     */
    case IO_PDM_RX_SET_REN: // set clock (RCC)
    REG_WRITE(PDM_REN, 1);  // ignore argument!
    break;
    case IO_PDM_RX_SET_BRC_B0:  // ch#0: BRC bit range shift
    REG_WRITE(PDM_BRC_B0, *(uint32_t *) arg);
    break;
    case IO_PDM_RX_SET_BRC_B1:  // ch#1: BRC bit range shift
    REG_WRITE(PDM_BRC_B1, *(uint32_t *) arg);
    break;
    case IO_PDM_RX_SET_BRC_B2:  // ch#2: BRC bit range shift
    REG_WRITE(PDM_BRC_B2, *(uint32_t *) arg);
    break;
    case IO_PDM_RX_SET_BRC_B3:  // ch#3: BRC bit range shift
    REG_WRITE(PDM_BRC_B3, *(uint32_t *) arg);
    break;
    case IO_PDM_RX_SET_BRC_CLP: // BRC clipping detection (CLPS = step & CLPT = threshold)
    REG_WRITE(PDM_BRC_CLP, *(uint32_t *) arg);
    break;
    case IO_PDM_RX_SET_FIFO_THRESHOLD:
    REG_WRITE(PDM_RFCR, (*(uint32_t *) arg));
    break;
    default:
    break;
    }
}

static uint32_t pdm_rx_read_samples(uint32_t dev_id, uint32_t * count)
{
    pdm_rx_info_pt dev = pdm_rx_handles[dev_id];

    if (dev->usr_cnt == dev->sys_cnt) { /* no more data buffer available (buffer underflow error) */
    /* if no buffer will be available until FIFO is full then error callback will be called from error ISR */
    return (1);
    } else {
    uint32_t idx = dev->sys_cnt & 0x0001;
    uint32_t *buff = dev->buffer[idx];
    uint32_t *size = dev->bufsize[idx];
    uint32_t free_cnt = 0;

    free_cnt = (*size - dev->xfr_len);
    if (*count <= free_cnt) {   /* buffer space is more or equal to samples available */
        free_cnt = *count;
        *count = 0;
    } else {        /* buffer space is less than samples available */
        *count -= free_cnt;
    }
    /* fill current buffer */
    for (int32_t i = 0; i != free_cnt; i++) {
        buff[dev->xfr_len + i] = REG_READ(PDM_RXDMA);
        buff[dev->xfr_len + i] |= (REG_READ(PDM_RXDMA) << 16);
    }
    dev->xfr_len += free_cnt;

    if (dev->xfr_len == *size) {    /* current buffer is full -> call user' callback */
        dev->sys_cnt++;
        dev->xfr_len = 0;
        if (NULL != dev->rx_cb) {
        dev->rx_cb(dev_id);
        }
    }
    }
    return (0);
}

/* PDM RX data available interrupt handler */
static void pdm_rx_ISR_proc(uint32_t dev_id)
{
    pdm_rx_info_pt dev = pdm_rx_handles[dev_id];
    uint32_t sample_cnt;
    uint32_t error;

#ifdef __Xdmac
    if (dev->dmachanid == DMA_NONE) {   /* DMA either disabled or does not exist */
#endif              /* __Xdmac */

    sample_cnt = REG_READ(PDM_RFCR) + 1;    // actual FIFO trigger level (in multichannel samples)
    sample_cnt *= dev->xfr_chn; // same value in stereo samples

    while (sample_cnt != 0) {
        error = pdm_rx_read_samples(dev_id, &sample_cnt);
        if (error != 0) {   /* buffer underrun error detected */
        REG_WRITE(PDM_IMR, 0x0001); // PDM: mask RXDA interrupt
        break;
        }
    }
#ifdef __Xdmac
    } else {            /* DMA enabled for PDM peripheral */
    if (++dev->sys_cnt == dev->usr_cnt) {   /* no data buffers left - disable dma channel and mask data available interrupt */
        _dma_chan_enable((0x1 << dev->dmachanid), 0);
    } else {
        // update channel descriptor
        _dma_chan_enable((0x1 << dev->dmachanid), 1);
    }

    if (NULL != dev->rx_cb) {   /* call for Rx callback if any  */
        dev->rx_cb(dev_id);
    }

    _dma_int_clear((0x1 << dev->dmachanid));    // clear DMA interrupt flag
    }
#endif              /* __Xdmac */
}

/* PDM RX error (FIFO overflow) interrupt handler */
static void pdm_rx_err_ISR_proc(uint32_t dev_id)
{
    pdm_rx_info_pt dev = pdm_rx_handles[dev_id];

    /* Rx FIFO overrun case */
#ifdef __Xdmac
    if (dev->dmachanid != DMA_NONE) {
    /* DMA: disable DMA channel */
    _dma_chan_enable((0x1 << dev->dmachanid), 0);
    _dma_int_clear((0x1 << dev->dmachanid));
    }
#endif

    REG_WRITE(PDM_IMR, 0x003F); // PDM: mask RX interrupts
    REG_WRITE(PDM_CER, 0x0000); // PDM: disable clk
    REG_WRITE(PDM_REN, 0x0000); // PDM: disable Rx
    REG_WRITE(PDM_EN, 0x0000);  // PDM: disable device

    /* call error callback if any */
    if (NULL != dev->err_cb) {
    dev->err_cb(dev_id);
    }
}

/* PDM Rx received data clipping ISR*/
static void pdm_rx_clip_ISR_proc(uint32_t dev_id)
{
    pdm_rx_info_pt dev = pdm_rx_handles[dev_id];
    uint32_t val = 0;

    // ToDo: do we really need reset clipping flags here???
    val = REG_READ(PDM_BRC_CLP);
    REG_WRITE(PDM_BRC_CLP, val);    // This write will clear CLIP interrupt flag

    /* call clip callback if any */
    if (NULL != dev->err_cb) {
    dev->clip_cb(dev_id);
    }

    return;
}
#endif
