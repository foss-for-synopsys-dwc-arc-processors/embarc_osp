#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "board.h"
#include "i2s_rx_slave.h"
//-- old code
//#include "dmac.h"
//-- New code
#include "embARC.h"
#include "embARC_debug.h"

#define IO_I2S_RX_SLV0_PRESENT
#define IO_I2S_RX_SLV0_FS           8
//dma
#undef __Xdmac
#define IO_I2S_RX_SLV0_DMA          13 //EMSDP_I2S_SLV0_DMA_RX
//-- end of mod

//-- Old code
//extern void _setvecti(int vector, _Interrupt void(*target)());
//-- end of mod

/* I2S Rx Slave device registers */
#define I2S_IER		0x000 
#define I2S_IRER		0x004
#define I2S_CER		0x00c
#define I2S_CCR		0x010 
#define I2S_RXFFR		0x014 
//Channel0
#define I2S_LRBR0		0x020 // Left Receive Buffer 
#define I2S_RRBR0		0x024 // Right Receive Buffer 
#define I2S_RER0		0x028
#define I2S_RCR0		0x030 // Receive  configuration register
#define I2S_ISR0		0x038 // Interrupt status register 
#define I2S_IMR0		0x03C // Interrupt mask register
#define I2S_ROR0		0x040
#define I2S_RFCR0		0x048
#define I2S_RFF0		0x050
//Channel1
#define I2S_LRBR1		0x060 // Left Receive Buffer 
#define I2S_RRBR1		0x064 // Right Receive Buffer  
#define I2S_RER1		0x068
#define I2S_RCR1		0x070 // Receive  configuration register
#define I2S_ISR1		0x078 // Interrupt status register 
#define I2S_IMR1		0x07C // Interrupt mask register
#define I2S_ROR1		0x080
#define I2S_RFCR1		0x088
#define I2S_RFF1		0x090
//Channel2
#define I2S_LRBR2		0x0A0 // Left Receive Buffer 
#define I2S_RRBR2		0x0A4 // Right Receive Buffer  
#define I2S_RER2		0x0A8
#define I2S_RCR2		0x0B0 // Receive  configuration register
#define I2S_ISR2		0x0B8 // Interrupt status register 
#define I2S_IMR2		0x0BC // Interrupt mask register
#define I2S_ROR2		0x0C0
#define I2S_RFCR2		0x0C8
#define I2S_RFF2		0x0D0
//Channel3
#define I2S_LRBR3		0x0E0 // Left Receive Buffer
#define I2S_RRBR3		0x0E4 // Right Receive Buffer  
#define I2S_RER3		0x0E8
#define I2S_RCR3		0x0F0 // Receive  configuration register
#define I2S_ISR3		0x0F8 // Interrupt status register 
#define I2S_IMR3		0x0FC // Interrupt mask register
#define I2S_ROR3		0x100
#define I2S_RFCR3		0x108
#define I2S_RFF3		0x110

#define I2S_RXDMA		0x1c0

/* I2S Rx Master device specific macros */
#define I2S_RX_MASTER_DMA_CTRL				   (0xa0000077) 	/* am=b10, i=b1, dw/inc=b011, dtt=b10, r=b1, op=b11 */
#define I2S_RX_MASTER_DMA_CTRL_SIZE_POS		(8) 	
#define I2S_RX_MASTER_DMA_CTRL_XFER_POS		(21) 	

#define DMA_NONE		(0xff)

typedef _Interrupt void (*ISR) ();

typedef struct i2s_rx_slave_info_struct
{
	/* I2S Rx slave configuration settings */
const uint32_t       reg_base; 		   // base address of device register set
const uint8_t		   instID;			   // module instance ID
const uint16_t       fifo_size;		   // FIFO depth 
const uint16_t		   dmachanid;		   // DMA channel ID
	/* I2S data buffer related fields */
   uint32_t          xfr_len;		      // number of samples transferred in current buffer
   uint32_t		      xfr_started;	   // transfer started flag
   uint32_t *        buffer[2];		   // two pointers to the user' buffers
   uint32_t *        bufsize[2];		   // two sizes of user buffers
volatile uint32_t 	usr_cnt;	   	   // user' buffer access counter (incremented from read service)
volatile uint32_t 	sys_cnt;		      // system' buffer access counter (incremented from ISR)

#ifdef __Xdmac	
_Uncached uint32_t   *dmadescriptor;	// pointer to the DMA descriptor location
#endif	
   /* User' callbacks */
   IO_CB_FUNC        rx_cb;			   // user' RX completion callback
   IO_CB_FUNC        err_cb;			   // user' error callback 
    /* Interrupt numbers and handlers */
const uint8_t        rx_vector; 		   // RX interrupt vector number 
const uint8_t        err_vector;		   // RX Error inetrrupt vector number
   ISR               rx_isr; 		      // I2S data available interrupt handler
   ISR               err_isr;		      // I2S FIFO overrun interrupt handler
} i2s_rx_slave_info_t, *i2s_rx_slave_info_pt;


#ifdef IO_I2S_RX_SLV0_PRESENT
#define I2S_RX_SLAVE_DEV_PRESENT
#endif
#ifdef IO_I2S_RX_SLV1_PRESENT
#define I2S_RX_SLAVE_DEV_PRESENT
#endif
#ifdef IO_I2S_RX_SLV2_PRESENT
#define I2S_RX_SLAVE_DEV_PRESENT
#endif
#ifdef IO_I2S_RX_SLV3_PRESENT
#define I2S_RX_SLAVE_DEV_PRESENT
#endif
#ifdef IO_I2S_RX_SLV4_PRESENT
#define I2S_RX_SLAVE_DEV_PRESENT
#endif

#ifdef I2S_RX_SLAVE_DEV_PRESENT

#define I2S_RX_SLAVE_MAX_CNT	(4)

static void i2s_rx_slave_rx_ISR_proc( uint32_t dev_id );
static void i2s_rx_slave_err_ISR_proc( uint32_t dev_id );

#ifdef IO_I2S_RX_SLV0_PRESENT
static _Interrupt void i2s_rx_slave0_rx_ISR(void)
{
    i2s_rx_slave_rx_ISR_proc(0);
}
static _Interrupt void i2s_rx_slave0_err_ISR(void)
{
    i2s_rx_slave_err_ISR_proc(0);
}
#endif 

/* I2S RX Slave devices private data structures */
static i2s_rx_slave_info_pt i2s_rx_slave_handles[I2S_RX_SLAVE_MAX_CNT] = { 0 };
#ifdef __Xdmac		
   static __attribute__((aligned(32))) _Uncached uint32_t i2s_rx_slave_dma_descriptors[8 * I2S_RX_SLAVE_MAX_CNT]; 
#endif

static i2s_rx_slave_info_t   i2s_rx_slave_devs[] = {
#ifdef IO_I2S_RX_SLV0_PRESENT
   {  .instID     = 0,
      .reg_base   = EMSDP_I2S_BASE,
//-- Old code: Channel0
//      .rx_vector  = EMSDP_I2S_RX_DA0_INTR,
//      .err_vector = EMSDP_I2S_RX_OR0_INTR,
//-- New code: Channel3
      .rx_vector  = EMSDP_I2S_RX_DA3_INTR,
      .err_vector = EMSDP_I2S_RX_OR3_INTR,
//-- end of mod      
      .rx_isr     = i2s_rx_slave0_rx_ISR,
      .err_isr    = i2s_rx_slave0_err_ISR,
   #ifdef IO_I2S_RX_SLV0_DMA		  
      .dmachanid  = IO_I2S_RX_SLV0_DMA,
   #else
      .dmachanid  = DMA_NONE,
   #endif			  
      .fifo_size  = IO_I2S_RX_SLV0_FS 
   },
#endif
};


//-- Old code
//Auxilary
//#define  REG_WRITE( reg, x )  _sr( (uint32_t)((x)), (uint32_t)(dev->reg_base + reg) )
//#define  REG_READ( reg )      _lr( (uint32_t)(dev->reg_base + (reg)) )
//-- New code
//Memory Mapped
#define  REG_WRITE(reg,val)   (*(volatile uint32_t *)(dev->reg_base + reg) = val)
#define  REG_READ(reg)        (*(volatile uint32_t *)(dev->reg_base + reg))
//-- end of mod

/* API functions */
uint32_t io_i2s_rx_slave_open( uint32_t dev_id )
{
	i2s_rx_slave_info_pt	dev;
	uint32_t      			h = 0;
	
	/* check device descriptor availability */
	while ((i2s_rx_slave_devs[h].instID != dev_id) && (i2s_rx_slave_devs[h].instID != I2S_RX_SLAVE_MAX_CNT)) 
	{
		h++;
	}
	if ((i2s_rx_slave_devs[h].instID == I2S_RX_SLAVE_MAX_CNT) || (0 != i2s_rx_slave_handles[dev_id]))
	{   /* dev_id not part of design, or still open */
	    return 1;
	}
	i2s_rx_slave_handles[dev_id] = &i2s_rx_slave_devs[h];
	dev = i2s_rx_slave_handles[dev_id];
	
	/* initialize driver internal variables */
	dev->xfr_started = 0;				// clear transfer started flag
   dev->xfr_len 	 = 0;				   // clear transfer length 
	dev->usr_cnt = dev->sys_cnt = 0;	// reset buffer access counters 

#ifdef __Xdmac		
	/* initialize DMA descriptors */
   dev->dmadescriptor = &i2s_rx_slave_dma_descriptors[8 * h];

	if (dev->dmachanid != DMA_NONE)
	{
		dev->dmadescriptor[0] = dev->dmadescriptor[4] = 0;
		dev->dmadescriptor[1] = dev->dmadescriptor[5] = dev->reg_base + I2S_RXDMA;
		dev->dmadescriptor[2] = dev->dmadescriptor[6] = 0;
		dev->dmadescriptor[3] = (uint32_t)&(dev->dmadescriptor[4]);
		dev->dmadescriptor[7] = (uint32_t)&(dev->dmadescriptor[0]);

		/* assign DMA interrupt handlers */
		_setvecti( dev->err_vector, dev->err_isr );

		_setvecti( DMAC_INT_BASE+dev->dmachanid, dev->rx_isr);
		_setvecti( DMAC_ERR_BASE+dev->dmachanid, dev->err_isr);
	}
	else
	{
#endif
		/* assign non DMA interrupt handlers */
		_setvecti( dev->rx_vector, dev->rx_isr );
		_setvecti( dev->err_vector, dev->err_isr );
#ifdef __Xdmac		
	}
#endif

	REG_WRITE( I2S_IER, 0x01 ); // I2S: enable device 
   
   return( 0 );
}


void io_i2s_rx_slave_close ( uint32_t dev_id )
{
	i2s_rx_slave_info_pt	  dev = i2s_rx_slave_handles[dev_id];

	/* reset I2S hardware */
   //-- Old code: Channel0   
   //REG_WRITE( I2S_IMR0,  0x03 );		// mask RX interrupts	
   //REG_WRITE( I2S_RER0,  0x01 );		// reset channel enable to default (1)
   //REG_WRITE( I2S_CER,   0x00 );		// disable clk 	
   //REG_WRITE( I2S_IRER,  0x00 );		// disable receive block
   //REG_WRITE( I2S_RCR0,  0x02 );		// reset wlen to default (2) 	
   //REG_WRITE( I2S_RFCR0, 0x00 );		// reset threshold 
   //-- New code: Channel3
	REG_WRITE( I2S_IMR3,  0x03 );		// mask RX interrupts	
	REG_WRITE( I2S_RER3,  0x01 );		// reset channel enable to default (1)
	REG_WRITE( I2S_CER,   0x00 );		// disable clk 	
	REG_WRITE( I2S_IRER,  0x00 );		// disable receive block
	REG_WRITE( I2S_RCR3,  0x02 );		// reset wlen to default (2) 	
	REG_WRITE( I2S_RFCR3, 0x00 );		// reset threshold 
   //-- end of mod
   
	REG_WRITE( I2S_IER,   0x00 );		// disable device and flushes fifo
	
#ifdef __Xdmac		
	/* reset DMA channel */
	_dma_chan_enable((0x1 << dev->dmachanid), 0); 		// aux-DMA channel reset bug fix	
	_dma_chan_reset(0x1 << dev->dmachanid);  
	/* deinitialize DMA interrupt handlers */
	_setvecti( DMAC_INT_BASE+dev->dmachanid, NULL);
	_setvecti( DMAC_ERR_BASE+dev->dmachanid, NULL);
#endif
	/* deinitialize non DMA interrupt handlers */
	_setvecti( dev->rx_vector, NULL );
	_setvecti( dev->err_vector, NULL );

	/* reset driver internal variables */
   dev->rx_cb  = NULL;
   dev->err_cb = NULL;	
	dev->xfr_started = 0;
	
	/* reset device handler */	
   i2s_rx_slave_handles[dev_id] = 0;
}

void io_i2s_rx_slave_read ( uint32_t dev_id, uint32_t * data, uint32_t * size )
{
	i2s_rx_slave_info_pt	dev = i2s_rx_slave_handles[dev_id];
	uint32_t				   idx = dev->usr_cnt;		// copy user access counter to the temporary variable

//-- New code
//   EMBARC_PRINTF("idx: %d, syscnt: %d, *s: 0x%x\n\r", idx, dev->sys_cnt, *size);
//-- end of mod
   
	if ( ((idx - dev->sys_cnt) != 2) && (*size != 0) ) 
	{
		idx = idx & 0x0001;							// current index in dev->buffer[] & dev->bufsize[] 
#ifdef __Xdmac
		if (dev->dmachanid != DMA_NONE)	{
			
         //-- Old code: Channel0
		   //uint32_t burst_size = (REG_READ(I2S_RFCR0) + 1) << 1;	// actual FIFO trigger level
         //-- New code: Channel3
			uint32_t burst_size  = (REG_READ(I2S_RFCR3) + 1) << 1;	// actual FIFO trigger level
         //-- end of mod
			uint32_t xfer_size   = (*size) << 2;						   // transfer size in bytes (ToDo: limited by 8KB)
			idx 				      = idx << 2;							      // calculate available buffer' index
			
			/* DMA: update DMACTRL and DMADAR part of descriptor */
			dev->dmadescriptor[idx + 0] =                   		      \
         I2S_RX_MASTER_DMA_CTRL 							               |  \
			( burst_size << I2S_RX_MASTER_DMA_CTRL_XFER_POS )	      |  \
			((xfer_size - 1)  << I2S_RX_MASTER_DMA_CTRL_SIZE_POS );
			dev->dmadescriptor[idx + 2] = (uint32_t)data + xfer_size - 2;
			
			if (dev->xfr_started == 0)
			{	/* I2S transfer not yet started -> init DMA and I2S Rx */
				dev->xfr_started = 1;

            //-- Old code: Channel0
				//REG_WRITE( I2S_RXFFR, 0x01 ); 	// I2S: reset RX FIFO
				//REG_WRITE( I2S_RFF0, 0x01 ); 		// I2S: flush RX FIFO
				//REG_WRITE( I2S_CER,  0x01 );		// I2S: start the clock
				//REG_WRITE( I2S_IRER, 0x01 );		// I2S: enable receive block						
				//REG_WRITE( I2S_IMR0, ~0x02 );		// I2S: Unmask RX overrun interrupt
            //-- New code: Channel3
				REG_WRITE( I2S_RXFFR, 0x01 ); 	// I2S: reset RX FIFO
				REG_WRITE( I2S_RFF3, 0x01 ); 		// I2S: flush RX FIFO
				REG_WRITE( I2S_CER,  0x01 );		// I2S: start the clock
				REG_WRITE( I2S_IRER, 0x01 );		// I2S: enable receive block						
				REG_WRITE( I2S_IMR3, ~0x02 );		// I2S: Unmask RX overrun interrupt
            //-- end of mod
				
				if (dev->dmachanid < DMAC_MEM_CHAN_CNT) 
				{	
					_dma_chan_desc(dev->dmachanid, &(dev->dmadescriptor[0]));		// dev->dmaidx must be 0 !!!
				}
				else
				{	
					_dma_chan_desc_aux(dev->dmachanid, &(dev->dmadescriptor[0]));	// dev->dmaidx must be 0 !!!
				}

				/* Start DMA channel / renew descriptor */
				_dma_chan_enable((0x1 << dev->dmachanid), 1);
			}
		} 
		else 
		{
#endif /* __Xdmac */
			/* Update data buffer parameters */
			dev->buffer [idx] = data;
			dev->bufsize[idx] = size;

			if ( dev->xfr_started == 0 )
			{	/* I2S transfer not yet started -> init I2S Rx */	
				dev->xfr_started = 1;
				
				/* Start I2S non DMA interrupt flow */
            //-- Old code: Channel0
				//REG_WRITE( I2S_RXFFR, 0x01 ); 	// I2S: reset RX FIFO
				//REG_WRITE( I2S_RFF0, 0x01 ); 		// I2S: flush RX FIFO
				//REG_WRITE( I2S_CER,  0x01 );		// I2S: start the clock
				//REG_WRITE( I2S_IRER, 0x01 );		// I2S: enable receive block
				//REG_WRITE( I2S_IMR0, ~0x03 );		// I2S: Unmask all RX interrupts
            //-- New code: Channel3
				REG_WRITE( I2S_RXFFR, 0x01 ); 	// I2S: reset RX FIFO
				REG_WRITE( I2S_RFF3, 0x01 ); 		// I2S: flush RX FIFO
				REG_WRITE( I2S_CER,  0x01 );		// I2S: start the clock
				REG_WRITE( I2S_IRER, 0x01 );		// I2S: enable receive block
				REG_WRITE( I2S_IMR3, ~0x03 );		// I2S: Unmask all RX interrupts
            //-- end of mod
			}
#ifdef __Xdmac		
		}
#endif /* __Xdmac */		

		dev->usr_cnt++;			// update user' buffer access counter
	}
}

void io_i2s_rx_slave_ioctl ( uint32_t dev_id, uint32_t cmd, void * arg )
{
	i2s_rx_slave_info_pt	dev = i2s_rx_slave_handles[dev_id];

   switch (cmd)
	{
	   case IO_SET_CB_RX:
         dev->rx_cb = ((io_cb_t *) arg)->cb;
         break;

      case IO_SET_CB_ERR:
         dev->err_cb = ((io_cb_t *) arg)->cb;
         break;

		case IO_I2S_RX_SLAVE_SET_SAMPLE_WIDTH:
			/* channel must be disabled before programming I2S_RCR0 */
         //-- Old code: Channel0
			//REG_WRITE( I2S_RCR0, (*(uint32_t *)arg) );
         //-- New code: Channel3
			REG_WRITE( I2S_RCR3, (*(uint32_t *)arg) );
         //-- end of mod
         break;

		case IO_I2S_RX_SLAVE_SET_FIFO_THRESHOLD:
			/* channel must be disabled before programming I2S_RFCR0 */
         //-- Old code: Channel0
			//REG_WRITE( I2S_RFCR0, (*(uint32_t *)arg) );
         //-- New code: Channel3
			REG_WRITE( I2S_RFCR3, (*(uint32_t *)arg) );
         //-- end of mod
         break;

      //-- still to do: Master functionality ???
		case IO_I2S_RX_SLAVE_SET_BITCLOCK:
			REG_WRITE( I2S_CER, *((uint32_t *)arg) );			
         break;

      default:
         break;
	}
}

static uint32_t i2s_rx_slave_read_samples(uint32_t dev_id, uint32_t *count)
{
	i2s_rx_slave_info_pt	dev = i2s_rx_slave_handles[dev_id];
    uint32_t        		free_cnt = 0;

	if (dev->usr_cnt == dev->sys_cnt)
	{   /* no more data buffer available (buffer underflow error) */
		/* if no buffer will be available until FIFO is full then error callback will be called from error ISR */
		return( 1 );
	}
	else
	{
		uint32_t idx 	= dev->sys_cnt & 0x0001;
		uint32_t *buff = dev->buffer [ idx ];
		uint32_t *size	= dev->bufsize[ idx ];
		
		free_cnt = (*size - dev->xfr_len);
		if (*count <= free_cnt)
		{   /* enough space in buffer[0] */
			free_cnt = *count;
			*count = 0;	     
		}
		else
		{   /* *count > free_cnt: there will be samples remaining for the next buffer */
			*count -= free_cnt;
		}
		/* fill current buffer  */
		for (int32_t i=0; i!=free_cnt; i++)
		{
         //-- Old code: Channel0
			//buff[dev->xfr_len+i]  = REG_READ( I2S_LRBR0 ); 
			//buff[dev->xfr_len+i] |= (REG_READ( I2S_RRBR0 ) << 16); 
         //-- New code: Channel0+1+2+3
			buff[dev->xfr_len + 4*i + 0]  = REG_READ( I2S_LRBR0 ); 
			buff[dev->xfr_len + 4*i + 0] |= (REG_READ( I2S_RRBR0 ) << 16); 

			buff[dev->xfr_len + 4*i + 1]  = REG_READ( I2S_LRBR1 ); 
			buff[dev->xfr_len + 4*i + 1] |= (REG_READ( I2S_RRBR1 ) << 16); 

			buff[dev->xfr_len + 4*i + 2]  = REG_READ( I2S_LRBR2 ); 
			buff[dev->xfr_len + 4*i + 2] |= (REG_READ( I2S_RRBR2 ) << 16); 
         
			buff[dev->xfr_len + 4*i + 3]  = REG_READ( I2S_LRBR3 ); 
			buff[dev->xfr_len + 4*i + 3] |= (REG_READ( I2S_RRBR3 ) << 16); 
         //-- end of mod
		}
      //-- Old code
		//dev->xfr_len += free_cnt;
      //-- New code: collect 4x samples
		dev->xfr_len += 4*free_cnt;
      //EMBARC_PRINTF("Len: 0x%x, size: 0x%x\n\r", dev->xfr_len, *size);
      //-- end of mod

		/* if current buffer is full: rx callback, and increment system counter */
		if ( dev->xfr_len == *size ) 
		{	
			dev->sys_cnt++;
			dev->xfr_len = 0;
         
			if (NULL != dev->rx_cb) 
         { 
            dev->rx_cb(dev_id); 
         }
		}
	}
	
	return( 0 );
}


static void i2s_rx_slave_rx_ISR_proc( uint32_t dev_id )
{
	i2s_rx_slave_info_pt	dev = i2s_rx_slave_handles[dev_id];
	uint32_t        		sample_cnt 	= 0;
	uint32_t        		error 		= 0;

	/* Rx FIFO trigger level reached - data available interrupt */
#ifdef __Xdmac				
	if ( dev->dmachanid == DMA_NONE )
	{	/* DMA either disabled or does not exist */
#endif /* __Xdmac */

      //-- Old code: Channel0
		//sample_cnt = REG_READ(I2S_RFCR0) + 1;	// actual FIFO trigger level
      //-- New code: Channel3
		sample_cnt = REG_READ(I2S_RFCR3) + 1;	// actual FIFO trigger level
      //-- end of mod

		while (sample_cnt != 0) 
		{
			error = i2s_rx_slave_read_samples(dev_id, &sample_cnt);
			if ( error != 0 ) 
			{	/* buffer underrun - no user' buffer left to receive data */
            //-- Old code: Channel0
				//REG_WRITE( I2S_IMR0, 0x0001 );		// I2S: mask RXDA interrupt
            //-- New code: Channel3
				REG_WRITE( I2S_IMR3, 0x0001 );		// I2S: mask RXDA interrupt
            //-- end of mod
				break;
			}
		}		
		
#ifdef __Xdmac				
	}
	else 
	{	/* DMA enabled for I2S peripheral */			
		if (++dev->sys_cnt == dev->usr_cnt) 
		{   /* no data buffers left - disable dma channel and mask data available interrupt */
			_dma_chan_enable((0x1 << dev->dmachanid), 0);
		}
		else
		{
			// update channel descriptor 
			_dma_chan_enable((0x1 << dev->dmachanid), 1);			
		}
		
		if (NULL != dev->rx_cb) 
		{ 	/* call for Rx callback if any  */
			dev->rx_cb(dev_id); 
		}
			
		_dma_int_clear  ((0x1 << dev->dmachanid));		// clear DMA interrupt flag 
	}
#endif /* __Xdmac */
}

static void i2s_rx_slave_err_ISR_proc( uint32_t dev_id )
{
	i2s_rx_slave_info_pt	dev = i2s_rx_slave_handles[dev_id];

	/* Rx FIFO overrun case */
#ifdef __Xdmac	
	if (dev->dmachanid != DMA_NONE)
	{
		/* DMA: disable DMA channel */
		_dma_chan_enable((0x1 << dev->dmachanid), 0);
		_dma_int_clear  ((0x1 << dev->dmachanid));		
	}
#endif

	/* I2S: stop I2S device */
   //-- Old code: Channel0
	//REG_WRITE( I2S_IMR0,  0x03 );		// I2S: mask RX interrupts			
   //-- New code: Channel3
	REG_WRITE( I2S_IMR3,  0x03 );		// I2S: mask RX interrupts			
   //-- end of mod
	REG_WRITE( I2S_CER,   0x00 );		// I2S: disable clk 	
	REG_WRITE( I2S_IER,   0x00 );		// I2S: disable device 

	/* call error callback if any */
	if (NULL != dev->err_cb) { dev->err_cb( dev_id ); }
}


#endif
