#ifndef _PCM1865_H_
#define _PCM1865_H_

#define I2C_DEVICE_ADDRESS_PCM1865_MST    (0x94 >> 1)
#define I2C_DEVICE_ADDRESS_PCM1865_SLV    (0x96 >> 1)
#define I2C_TIMEOUT                       (20000)

/**
* Devices on the board
*/
typedef enum
{
   Pcm1865_Device_Master = 0,
   Pcm1865_Device_Slave,
   Pcm1865_Device_None
} Pcm1865_Device_t;

/**
* Virtual RAM addresses
*/
typedef enum
{
//Mixer-1
   PCM1865_MIX1_CH1L = 0x0,         //0x00
   PCM1865_MIX1_CH1R,               //0x01
   PCM1865_MIX1_CH2L,               //0x02
   PCM1865_MIX1_CH2R,               //0x03
   PCM1865_MIX1_I2SL,               //0x04
   PCM1865_MIX1_I2SR,               //0x05
//Mixer-2 
   PCM1865_MIX2_CH1L,               //0x06
   PCM1865_MIX2_CH1R,               //0x07
   PCM1865_MIX2_CH2L,               //0x08
   PCM1865_MIX2_CH2R,               //0x09
   PCM1865_MIX2_I2SL,               //0x0A
   PCM1865_MIX2_I2SR,               //0x0B
////Mixer-3 
   PCM1865_MIX3_CH1L,               //0x0C
   PCM1865_MIX3_CH1R,               //0x0D
   PCM1865_MIX3_CH2L,               //0x0E
   PCM1865_MIX3_CH2R,               //0x0F
   PCM1865_MIX3_I2SL,               //0x10
   PCM1865_MIX3_I2SR,               //0x11
////Mixer-4 
   PCM1865_MIX4_CH1L,               //0x12
   PCM1865_MIX4_CH1R,               //0x13
   PCM1865_MIX4_CH2L,               //0x14
   PCM1865_MIX4_CH2R,               //0x15
   PCM1865_MIX4_I2SL,               //0x16
   PCM1865_MIX4_I2SR,               //0x17
//Secondary ADC LPF and HPF Coefficients
   PCM1865_LPF_B0 = 0x20,           //0x20
   PCM1865_LPF_B1,                  //0x21
   PCM1865_LPF_B2,                  //0x22
   PCM1865_LPF_A1,                  //0x23
   PCM1865_LPF_A2,                  //0x24
   PCM1865_HPF_B0,                  //0x25
   PCM1865_HPF_B1,                  //0x26
   PCM1865_HPF_B2,                  //0x27
   PCM1865_HPF_A1,                  //0x28
   PCM1865_HPF_A2,                  //0x29
//Energysense 
   PCM1865_LOSS_THRESHOLD = 0x2C,   //0x2C
   PCM1865_RESUME_THRESHOLD,        //0x2D
} Pcm1865_VirtualRamAddress_t;

/**
* Register map:
* => Copied from Linux driver 
*/
#define PCM186X_PAGE_LEN		            0x0100
#define PCM186X_PAGE_BASE(n)		         (PCM186X_PAGE_LEN * n)

/* The page selection register address is the same on all pages */
#define PCM186X_PAGE0			            0
#define PCM186X_CHANGE_PAGE               (PCM186X_PAGE_BASE(0) +   0)
#define PCM186X_PGA_VAL_CH1_L		         (PCM186X_PAGE_BASE(0) +   1)
#define PCM186X_PGA_VAL_CH1_R		         (PCM186X_PAGE_BASE(0) +   2)
#define PCM186X_PGA_VAL_CH2_L		         (PCM186X_PAGE_BASE(0) +   3)
#define PCM186X_PGA_VAL_CH2_R		         (PCM186X_PAGE_BASE(0) +   4)
#define PCM186X_PGA_CTRL		            (PCM186X_PAGE_BASE(0) +   5)
#define PCM186X_ADC1_INPUT_SEL_L	         (PCM186X_PAGE_BASE(0) +   6)
#define PCM186X_ADC1_INPUT_SEL_R	         (PCM186X_PAGE_BASE(0) +   7)
#define PCM186X_ADC2_INPUT_SEL_L	         (PCM186X_PAGE_BASE(0) +   8)
#define PCM186X_ADC2_INPUT_SEL_R	         (PCM186X_PAGE_BASE(0) +   9)
#define PCM186X_AUXADC_INPUT_SEL	         (PCM186X_PAGE_BASE(0) +  10)
#define PCM186X_PCM_CFG			            (PCM186X_PAGE_BASE(0) +  11)
#define PCM186X_TDM_TX_SEL		            (PCM186X_PAGE_BASE(0) +  12)
#define PCM186X_TDM_TX_OFFSET		         (PCM186X_PAGE_BASE(0) +  13)
#define PCM186X_TDM_RX_OFFSET		         (PCM186X_PAGE_BASE(0) +  14)
#define PCM186X_DPGA_VAL_CH1_L		      (PCM186X_PAGE_BASE(0) +  15)
#define PCM186X_GPIO1_0_CTRL		         (PCM186X_PAGE_BASE(0) +  16)
#define PCM186X_GPIO3_2_CTRL		         (PCM186X_PAGE_BASE(0) +  17)
#define PCM186X_GPIO1_0_DIR_CTRL	         (PCM186X_PAGE_BASE(0) +  18)
#define PCM186X_GPIO3_2_DIR_CTRL	         (PCM186X_PAGE_BASE(0) +  19)
#define PCM186X_GPIO_IN_OUT		         (PCM186X_PAGE_BASE(0) +  20)
#define PCM186X_GPIO_PULL_CTRL		      (PCM186X_PAGE_BASE(0) +  21)
#define PCM186X_DPGA_VAL_CH1_R		      (PCM186X_PAGE_BASE(0) +  22)
#define PCM186X_DPGA_VAL_CH2_L		      (PCM186X_PAGE_BASE(0) +  23)
#define PCM186X_DPGA_VAL_CH2_R		      (PCM186X_PAGE_BASE(0) +  24)
#define PCM186X_DPGA_GAIN_CTRL		      (PCM186X_PAGE_BASE(0) +  25)
#define PCM186X_DPGA_MIC_CTRL		         (PCM186X_PAGE_BASE(0) +  26)
#define PCM186X_DIN_RESAMP_CTRL		      (PCM186X_PAGE_BASE(0) +  27)
#define PCM186X_CLK_CTRL		            (PCM186X_PAGE_BASE(0) +  32)
#define PCM186X_DSP1_CLK_DIV		         (PCM186X_PAGE_BASE(0) +  33)
#define PCM186X_DSP2_CLK_DIV		         (PCM186X_PAGE_BASE(0) +  34)
#define PCM186X_ADC_CLK_DIV		         (PCM186X_PAGE_BASE(0) +  35)
#define PCM186X_PLL_SCK_DIV		         (PCM186X_PAGE_BASE(0) +  37)
#define PCM186X_BCK_DIV			            (PCM186X_PAGE_BASE(0) +  38)
#define PCM186X_LRK_DIV			            (PCM186X_PAGE_BASE(0) +  39)
#define PCM186X_PLL_CTRL		            (PCM186X_PAGE_BASE(0) +  40)
#define PCM186X_PLL_P_DIV		            (PCM186X_PAGE_BASE(0) +  41)
#define PCM186X_PLL_R_DIV		            (PCM186X_PAGE_BASE(0) +  42)
#define PCM186X_PLL_J_DIV		            (PCM186X_PAGE_BASE(0) +  43)
#define PCM186X_PLL_D_DIV_LSB	            (PCM186X_PAGE_BASE(0) +  44)
#define PCM186X_PLL_D_DIV_MSB		         (PCM186X_PAGE_BASE(0) +  45)
#define PCM186X_SIGDET_MODE		         (PCM186X_PAGE_BASE(0) +  48)
#define PCM186X_SIGDET_MASK		         (PCM186X_PAGE_BASE(0) +  49)
#define PCM186X_SIGDET_STAT		         (PCM186X_PAGE_BASE(0) +  50)
#define PCM186X_SIGDET_LOSS_TIME	         (PCM186X_PAGE_BASE(0) +  52)
#define PCM186X_SIGDET_SCAN_TIME	         (PCM186X_PAGE_BASE(0) +  53)
#define PCM186X_SIGDET_INT_INTVL	         (PCM186X_PAGE_BASE(0) +  54)
#define PCM186X_SIGDET_DC_REF_CH1_L	      (PCM186X_PAGE_BASE(0) +  64)
#define PCM186X_SIGDET_DC_DIFF_CH1_L      (PCM186X_PAGE_BASE(0) +  65)
#define PCM186X_SIGDET_DC_LEV_CH1_L	      (PCM186X_PAGE_BASE(0) +  66)
#define PCM186X_SIGDET_DC_REF_CH1_R	      (PCM186X_PAGE_BASE(0) +  67)
#define PCM186X_SIGDET_DC_DIFF_CH1_R     	(PCM186X_PAGE_BASE(0) +  68)
#define PCM186X_SIGDET_DC_LEV_CH1_R	      (PCM186X_PAGE_BASE(0) +  69)
#define PCM186X_SIGDET_DC_REF_CH2_L	      (PCM186X_PAGE_BASE(0) +  70)
#define PCM186X_SIGDET_DC_DIFF_CH2_L     	(PCM186X_PAGE_BASE(0) +  71)
#define PCM186X_SIGDET_DC_LEV_CH2_L	      (PCM186X_PAGE_BASE(0) +  72)
#define PCM186X_SIGDET_DC_REF_CH2_R	      (PCM186X_PAGE_BASE(0) +  73)
#define PCM186X_SIGDET_DC_DIFF_CH2_R     	(PCM186X_PAGE_BASE(0) +  74)
#define PCM186X_SIGDET_DC_LEV_CH2_R	      (PCM186X_PAGE_BASE(0) +  75)
#define PCM186X_SIGDET_DC_REF_CH3_L	      (PCM186X_PAGE_BASE(0) +  76)
#define PCM186X_SIGDET_DC_DIFF_CH3_L     	(PCM186X_PAGE_BASE(0) +  77)
#define PCM186X_SIGDET_DC_LEV_CH3_L	      (PCM186X_PAGE_BASE(0) +  78)
#define PCM186X_SIGDET_DC_REF_CH3_R	      (PCM186X_PAGE_BASE(0) +  79)
#define PCM186X_SIGDET_DC_DIFF_CH3_R	   (PCM186X_PAGE_BASE(0) +  80)
#define PCM186X_SIGDET_DC_LEV_CH3_R	      (PCM186X_PAGE_BASE(0) +  81)
#define PCM186X_SIGDET_DC_REF_CH4_L	      (PCM186X_PAGE_BASE(0) +  82)
#define PCM186X_SIGDET_DC_DIFF_CH4_L	   (PCM186X_PAGE_BASE(0) +  83)
#define PCM186X_SIGDET_DC_LEV_CH4_L	      (PCM186X_PAGE_BASE(0) +  84)
#define PCM186X_SIGDET_DC_REF_CH4_R	      (PCM186X_PAGE_BASE(0) +  85)
#define PCM186X_SIGDET_DC_DIFF_CH4_R	   (PCM186X_PAGE_BASE(0) +  86)
#define PCM186X_SIGDET_DC_LEV_CH4_R	      (PCM186X_PAGE_BASE(0) +  87)
#define PCM186X_AUXADC_DATA_CTRL	         (PCM186X_PAGE_BASE(0) +  88)
#define PCM186X_AUXADC_DATA_LSB		      (PCM186X_PAGE_BASE(0) +  89)
#define PCM186X_AUXADC_DATA_MSB		      (PCM186X_PAGE_BASE(0) +  90)
#define PCM186X_INT_ENABLE		            (PCM186X_PAGE_BASE(0) +  96)
#define PCM186X_INT_FLAG		            (PCM186X_PAGE_BASE(0) +  97)
#define PCM186X_INT_POL_WIDTH		         (PCM186X_PAGE_BASE(0) +  98)
#define PCM186X_POWER_CTRL		            (PCM186X_PAGE_BASE(0) + 112)
#define PCM186X_FILTER_MUTE_CTRL	         (PCM186X_PAGE_BASE(0) + 113)
#define PCM186X_DEVICE_STATUS		         (PCM186X_PAGE_BASE(0) + 114)
#define PCM186X_FSAMPLE_STATUS		      (PCM186X_PAGE_BASE(0) + 115)
#define PCM186X_DIV_STATUS		            (PCM186X_PAGE_BASE(0) + 116)
#define PCM186X_CLK_STATUS		            (PCM186X_PAGE_BASE(0) + 117)
#define PCM186X_SUPPLY_STATUS		         (PCM186X_PAGE_BASE(0) + 120)

/* Register Definitions - Page 1 */
#define PCM186X_PAGE1			            1
#define PCM186X_MMAP_STAT_CTRL		      (PCM186X_PAGE_BASE(1) +   1)
#define PCM186X_MMAP_ADDRESS		         (PCM186X_PAGE_BASE(1) +   2)
#define PCM186X_MEM_WDATA0		            (PCM186X_PAGE_BASE(1) +   4)
#define PCM186X_MEM_WDATA1		            (PCM186X_PAGE_BASE(1) +   5)
#define PCM186X_MEM_WDATA2		            (PCM186X_PAGE_BASE(1) +   6)
#define PCM186X_MEM_WDATA3		            (PCM186X_PAGE_BASE(1) +   7)
#define PCM186X_MEM_RDATA0		            (PCM186X_PAGE_BASE(1) +   8)
#define PCM186X_MEM_RDATA1		            (PCM186X_PAGE_BASE(1) +   9)
#define PCM186X_MEM_RDATA2		            (PCM186X_PAGE_BASE(1) +  10)
#define PCM186X_MEM_RDATA3		            (PCM186X_PAGE_BASE(1) +  11)

/* Register Definitions - Page 3 */
#define PCM186X_PAGE3			            3
#define PCM186X_OSC_PWR_DOWN_CTRL	      (PCM186X_PAGE_BASE(3) +  18)
#define PCM186X_MIC_BIAS_CTRL		         (PCM186X_PAGE_BASE(3) +  21)

/* Register Definitions - Page 253 */
#define PCM186X_PAGE253 		            253
#define PCM186X_CURR_TRIM_CTRL		      (PCM186X_PAGE_BASE(253) +  20)

/**
* Different configurations
*/
#define PCM1865_CONFIG_TDM_MASTER         (0)
#define PCM1865_CONFIG_TDM_SLAVE          (1)

extern int  pcm1865_initI2c               (void);

extern int  pcm1865_init                  (Pcm1865_Device_t id, int config);

extern int  pcm1865_reset                 (Pcm1865_Device_t id);

extern int  pcm1865_writeRamAddr          (Pcm1865_Device_t id, Pcm1865_VirtualRamAddress_t addr, uint32_t value);

extern int  pcm1865_readRamAddr           (Pcm1865_Device_t id, Pcm1865_VirtualRamAddress_t addr, uint32_t *value);

extern int  pcm1865_showPage              (Pcm1865_Device_t id, int page);

extern int  pcm1865_showVRam              (Pcm1865_Device_t id);

#endif


