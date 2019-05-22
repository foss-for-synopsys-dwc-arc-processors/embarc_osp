#include <stdio.h>
#include <stdint.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board.h"
#include "pcm1865.h"
#include "emsdp/drivers/mux/mux.h"
//I2C driver

typedef struct Pcm1865_VirtualRam_Table 
{
   char                        str[15];   
   Pcm1865_VirtualRamAddress_t addr;
} Pcm1865_VirtualRam_Table_t;

const Pcm1865_VirtualRam_Table_t 	pcm1865_vram_table[] = 
{ //0123456789012345
   {"MIX1_CH1_L    ", PCM1865_MIX1_CH1L},         //0x00
   {"MIX1_CH1_R    ", PCM1865_MIX1_CH1R},         //0x01
   {"MIX1_CH2_L    ", PCM1865_MIX1_CH2L},         //0x02
   {"MIX1_CH2_R    ", PCM1865_MIX1_CH2R},         //0x03
   {"MIX1_I2S_L    ", PCM1865_MIX1_I2SL},         //0x04
   {"MIX1_I2S_R    ", PCM1865_MIX1_I2SR},         //0x05
   //Mixer-2 
   {"MIX2_CH1_L    ", PCM1865_MIX2_CH1L},         //0x06
   {"MIX2_CH1_R    ", PCM1865_MIX2_CH1R},         //0x07
   {"MIX2_CH2_L    ", PCM1865_MIX2_CH2L},         //0x08
   {"MIX2_CH2_R    ", PCM1865_MIX2_CH2R},         //0x09
   {"MIX2_I2S_L    ", PCM1865_MIX2_I2SL},         //0x0A
   {"MIX2_I2S_R    ", PCM1865_MIX2_I2SR},         //0x0B
   //Mixer-3 
   {"MIX3_CH1_L    ", PCM1865_MIX3_CH1L},         //0x0C
   {"MIX3_CH1_R    ", PCM1865_MIX3_CH1R},         //0x0D
   {"MIX3_CH2_L    ", PCM1865_MIX3_CH2L},         //0x0E
   {"MIX3_CH2_R    ", PCM1865_MIX3_CH2R},         //0x0F
   {"MIX3_I2S_L    ", PCM1865_MIX3_I2SL},         //0x10
   {"MIX3_I2S_R    ", PCM1865_MIX3_I2SR},         //0x11
   //Mixer-4 
   {"MIX4_CH1_L    ", PCM1865_MIX4_CH1L},         //0x12
   {"MIX4_CH1_L    ", PCM1865_MIX4_CH1R},         //0x13
   {"MIX4_CH2_L    ", PCM1865_MIX4_CH2L},         //0x14
   {"MIX4_CH2_L    ", PCM1865_MIX4_CH2R},         //0x15
   {"MIX4_I2S_L    ", PCM1865_MIX4_I2SL},         //0x16
   {"MIX4_I2S_R    ", PCM1865_MIX4_I2SR},         //0x17
   //Secondary ADC LPF and HPF Coefficients
   {"LPF_B0        ", PCM1865_LPF_B0},            //0x20
   {"LPF_B1        ", PCM1865_LPF_B1},            //0x21
   {"LPF_B2        ", PCM1865_LPF_B2},            //0x22
   {"LPF_A1        ", PCM1865_LPF_A1},            //0x23
   {"LPF_A2        ", PCM1865_LPF_A2},            //0x24
   {"HPF_B0        ", PCM1865_HPF_B0},            //0x25
   {"HPF_B1        ", PCM1865_HPF_B1},            //0x26
   {"HPF_B2        ", PCM1865_HPF_B2},            //0x27
   {"HPF_A1        ", PCM1865_HPF_A1},            //0x28
   {"HPF_A2        ", PCM1865_HPF_A2},            //0x29
   //Energysense 
   {"LOSS-THRS_HLD ", PCM1865_LOSS_THRESHOLD},    //0x2C
   {"RES-THRS_HLD  ", PCM1865_RESUME_THRESHOLD},  //0x2D   
};

#define  PCM1865_VRAM_TABLE_LENGTH     (sizeof(pcm1865_vram_table) / sizeof(pcm1865_vram_table[0]))

/**
* For show_Reg function
*/
typedef struct Pcm1865_Reg_Table 
{
   char        str[15];   
   uint16_t    reg;
} Pcm1865_Reg_Table_t;

const Pcm1865_Reg_Table_t 	pcm1865_page0_reg_table[] = 
{ //0123456789012345
   {"PGAVALCH1_L   ", PCM186X_PGA_VAL_CH1_L},   //0x01
   {"PGAVALCH1_R   ", PCM186X_PGA_VAL_CH1_R},
   {"PGAVALCH2_L   ", PCM186X_PGA_VAL_CH2_L},
   {"PGAVALCH2_R   ", PCM186X_PGA_VAL_CH2_R},
   {"PGA_CTRL      ", PCM186X_PGA_CTRL},
   {"ADC1_INP_SEL_L", PCM186X_ADC1_INPUT_SEL_L},
   {"ADC1_INP_SEL_R", PCM186X_ADC1_INPUT_SEL_R},
   {"ADC2_INP_SEL_L", PCM186X_ADC2_INPUT_SEL_L},
   {"ADC1_INP_SEL_R", PCM186X_ADC2_INPUT_SEL_R},
   {"AUXADC_INP_SEL", PCM186X_AUXADC_INPUT_SEL},
   {"PCM_CFG       ", PCM186X_PCM_CFG},
   {"TDM_TX_SEL    ", PCM186X_TDM_TX_SEL},
   {"TDM_TX_OFFSET ", PCM186X_TDM_TX_OFFSET},
   {"TDM_RX_OFFSET ", PCM186X_TDM_RX_OFFSET},
   {"DPGA_CH1_L    ", PCM186X_DPGA_VAL_CH1_L},
   {"GPIO1_0_CTRL  ", PCM186X_GPIO1_0_CTRL},
   {"GPIO3_2_CTRL  ", PCM186X_GPIO3_2_CTRL},
   {"GPIO1_0_DIR   ", PCM186X_GPIO1_0_DIR_CTRL},
   {"GPIO3_2_DIR   ", PCM186X_GPIO3_2_DIR_CTRL},
   {"GPIO_IN_OUT   ", PCM186X_GPIO_IN_OUT},
   {"GPIO_PULL_CTRL", PCM186X_GPIO_PULL_CTRL},
   {"DPGA_CH1_R    ", PCM186X_DPGA_VAL_CH1_R},
   {"DPGA_CH2_L    ", PCM186X_DPGA_VAL_CH2_L},
   {"DPGA_CH2_R    ", PCM186X_DPGA_VAL_CH2_R},
   {"DPGA_GAIN_CTRL", PCM186X_DPGA_GAIN_CTRL},
   {"DPGA_MIC_CTRL ", PCM186X_DPGA_MIC_CTRL},
   {"DIN_RESAMP_CTR", PCM186X_DIN_RESAMP_CTRL},
   {"CLK_CTRL      ", PCM186X_CLK_CTRL},
   {"DSP1_CLK_DIV  ", PCM186X_DSP1_CLK_DIV},
   {"DSP2_CLK_DIV  ", PCM186X_DSP2_CLK_DIV},
   {"ADC_CLK_DIV   ", PCM186X_ADC_CLK_DIV},
   {"PLL_SCK_DIV   ", PCM186X_PLL_SCK_DIV},
   {"BCK_DIV       ", PCM186X_BCK_DIV},
   {"LRK_DIV       ", PCM186X_LRK_DIV},
   {"PLL_CTRL      ", PCM186X_PLL_CTRL},
   {"PLL_P_DIV     ", PCM186X_PLL_P_DIV},
   {"PLL_R_DIV     ", PCM186X_PLL_R_DIV},
   {"PLL_J_DIV     ", PCM186X_PLL_J_DIV},
   {"PLL_D_DIV_LSB ", PCM186X_PLL_D_DIV_LSB},
   {"PLL_D_DIV_MSB ", PCM186X_PLL_D_DIV_MSB},
   {"SIGDET_MODE   ", PCM186X_SIGDET_MODE},
   {"SIGDET_MASK   ", PCM186X_SIGDET_MASK},
   {"SIGDET_STAT   ", PCM186X_SIGDET_STAT},
   {"SIGDET_LOSS   ", PCM186X_SIGDET_LOSS_TIME},
   {"SIGDIT_SCAN   ", PCM186X_SIGDET_SCAN_TIME},
   {"SIGDIT_INTLVL ", PCM186X_SIGDET_INT_INTVL},

   {"SIGDET_RF_CH1L", PCM186X_SIGDET_DC_REF_CH1_L},
   {"SIGDET_DF_CH1L", PCM186X_SIGDET_DC_DIFF_CH1_L},
   {"SIGDET_LV_CH1L", PCM186X_SIGDET_DC_LEV_CH1_L},
   {"SIGDET_RF_CH1R", PCM186X_SIGDET_DC_REF_CH1_R},
   {"SIGDET_DF_CH1R", PCM186X_SIGDET_DC_DIFF_CH1_R},
   {"SIGDET_LV_CH1R", PCM186X_SIGDET_DC_LEV_CH1_R},

   {"SIGDET_RF_CH2L", PCM186X_SIGDET_DC_REF_CH2_L},
   {"SIGDET_DF_CH2L", PCM186X_SIGDET_DC_DIFF_CH2_L},
   {"SIGDET_LV_CH2L", PCM186X_SIGDET_DC_LEV_CH2_L},
   {"SIGDET_RF_CH2R", PCM186X_SIGDET_DC_REF_CH2_R},
   {"SIGDET_DF_CH2R", PCM186X_SIGDET_DC_DIFF_CH2_R},
   {"SIGDET_LV_CH2R", PCM186X_SIGDET_DC_LEV_CH2_R},
   
   {"SIGDET_RF_CH3L", PCM186X_SIGDET_DC_REF_CH3_L},
   {"SIGDET_DF_CH3L", PCM186X_SIGDET_DC_DIFF_CH3_L},
   {"SIGDET_LV_CH3L", PCM186X_SIGDET_DC_LEV_CH3_L},
   {"SIGDET_RF_CH3R", PCM186X_SIGDET_DC_REF_CH3_R},
   {"SIGDET_DF_CH3R", PCM186X_SIGDET_DC_DIFF_CH3_R},
   {"SIGDET_LV_CH3R", PCM186X_SIGDET_DC_LEV_CH3_R},

   {"SIGDET_RF_CH4L", PCM186X_SIGDET_DC_REF_CH4_L},
   {"SIGDET_DF_CH4L", PCM186X_SIGDET_DC_DIFF_CH4_L},
   {"SIGDET_LV_CH4L", PCM186X_SIGDET_DC_LEV_CH4_L},
   {"SIGDET_RF_CH4R", PCM186X_SIGDET_DC_REF_CH4_R},
   {"SIGDET_DF_CH4R", PCM186X_SIGDET_DC_DIFF_CH4_R},
   {"SIGDET_LV_CH4R", PCM186X_SIGDET_DC_LEV_CH4_R},

   {"AUXADC_DT_CTRL", PCM186X_AUXADC_DATA_CTRL},
   {"AUXADC_DT_LSB ", PCM186X_AUXADC_DATA_LSB},
   {"AUXADC_DT_MSB ", PCM186X_AUXADC_DATA_MSB},
   {"INT_ENABLE    ", PCM186X_INT_ENABLE},
   {"INT_FLAG      ", PCM186X_INT_FLAG},
   {"INT_POL_WIDTH ", PCM186X_INT_POL_WIDTH},
   {"POWER_CTRL    ", PCM186X_POWER_CTRL},
   {"FILTER_MUTE   ", PCM186X_FILTER_MUTE_CTRL},
   {"DEVICE_STATUS ", PCM186X_DEVICE_STATUS},
   {"FSAMPLE_STATUS", PCM186X_FSAMPLE_STATUS},
   {"DIV_STATUS    ", PCM186X_DIV_STATUS},
   {"CLK_STATUS    ", PCM186X_CLK_STATUS},
   {"SUPPLY_STATUS ", PCM186X_SUPPLY_STATUS}
};

#define  PCM1865_PAGE0_REG_TABLE_LENGTH     (sizeof(pcm1865_page0_reg_table) / sizeof(pcm1865_page0_reg_table[0]))

/**
* For show_Reg function
*/
const Pcm1865_Reg_Table_t 	pcm1865_page1_reg_table[] = 
{ //0123456789012345
   {"MMAP_STAT_CTRL", PCM186X_MMAP_STAT_CTRL},
   {"MMAP_ADDRESS  ", PCM186X_MMAP_ADDRESS},
   {"MEM_WDATA0    ", PCM186X_MEM_WDATA0},
   {"MEM_WDATA1    ", PCM186X_MEM_WDATA1},
   {"MEM_WDATA2    ", PCM186X_MEM_WDATA2},
   {"MEM_WDATA3    ", PCM186X_MEM_WDATA3},
   {"MEM_RDATA0    ", PCM186X_MEM_RDATA0},
   {"MEM_RDATA1    ", PCM186X_MEM_RDATA1},
   {"MEM_RDATA2    ", PCM186X_MEM_RDATA2},
   {"MEM_RDATA3    ", PCM186X_MEM_RDATA3}
};

#define  PCM1865_PAGE1_REG_TABLE_LENGTH     (sizeof(pcm1865_page1_reg_table) / sizeof(pcm1865_page1_reg_table[0]))

/**
* For show_Reg function
*/
const Pcm1865_Reg_Table_t 	pcm1865_page3_reg_table[] = 
{ //0123456789012345
   {"OSC_PWRDOWN_CT", PCM186X_OSC_PWR_DOWN_CTRL},
   {"MIC_BIAS_CTRL ", PCM186X_MIC_BIAS_CTRL}
};   

#define  PCM1865_PAGE3_REG_TABLE_LENGTH     (sizeof(pcm1865_page3_reg_table) / sizeof(pcm1865_page3_reg_table[0]))

/**
* For show_Reg function
*/
const Pcm1865_Reg_Table_t 	pcm1865_page253_reg_table[] = 
{ //0123456789012345
   {"CURR_TRIM_CTRL", PCM186X_CURR_TRIM_CTRL},
};

#define  PCM1865_PAGE253_REG_TABLE_LENGTH   (sizeof(pcm1865_page253_reg_table) / sizeof(pcm1865_page253_reg_table[0]))

/*******************************************************************************
* Different tables with settings
*/
typedef struct Pcm1865_Settings
{
   uint16_t reg;
   uint8_t  val;
} Pcm1865_Settings_t;

/**
* TDM 
*/
const Pcm1865_Settings_t settings_tdm_master[] = 
{
   //CLK
   {PCM186X_CLK_CTRL,         0x91},   //reg 32, Set as timing master on PCM1865 (Xtal + Master + Enable)
   {PCM186X_BCK_DIV,          0x00},   //reg 38, Set BCLK Divider from SCK to 1 on PCM1865 96 kHz for LR CLK
   {PCM186X_LRK_DIV,	         0xFF},   //reg 39, Set BCK to LRCLK ratio to 256 
   //GPIO
   {PCM186X_GPIO1_0_CTRL,     0x05},   //reg 16, Gpio0 = DOUT2, polarity normal, Gpio1 = gpio
   //TDM mode
   {PCM186X_PCM_CFG,          0x03},   //reg 11, set for TDM mode with 32 bit data slots
   {PCM186X_TDM_TX_SEL,       0x01},   //reg 12, Set for 4 channel TDM 
   {PCM186X_TDM_TX_OFFSET,    0x01},   //reg 13, Data offset from LRCLK rising edge. Set to 0x81 for the second four words, 0x01 for the first four
};

#define  SETTINGS_TDM_MASTER_LENGTH    (sizeof(settings_tdm_master) / sizeof(settings_tdm_master[0]))

const Pcm1865_Settings_t settings_tdm_slave[] = 
{
   {PCM186X_GPIO1_0_CTRL,     0x05},   //reg 16, Gpio0 = DOUT2, polarity normal, Gpio1 = gpio
   {PCM186X_PCM_CFG,          0x03},   //reg 11, set for TDM mode with 32 bit data slots
   {PCM186X_TDM_TX_SEL,       0x01},   //reg 12, Set for 4 channel TDM 
   {PCM186X_TDM_TX_OFFSET,    0x81},   //reg 13, Data offset from LRCLK rising edge. Set to 0x81 for the second four words, 0x01 for the first four
};

#define  SETTINGS_TDM_SLAVE_LENGTH     (sizeof(settings_tdm_slave) / sizeof(settings_tdm_slave[0]))

/**
* I2S
*/
const Pcm1865_Settings_t settings_i2s_master[] = 
{
//Example1: Bit/Ws clk
   {PCM186X_LRK_DIV,          0x3F},   //reg 39. LRCK = BCLK/64
   {PCM186X_BCK_DIV,          0x07},   //reg 38, BCLK = 1/8 XTal = 3.07 MHz, LRCK = BCLK/64 = ~48 KHz (value 64 = default of 0x27/39)

//Example2:  Bit/Ws clk, but loosing: channel0 and 1
//   {PCM186X_LRK_DIV,          0x3F},   //reg 39. LRCK = BCLK/64
//   {PCM186X_BCK_DIV,	        0x0F},   //reg 38, BCLK = 1/16 XTal = 1.53 MHz, LRCK = BCLK/64 = ~24 KHz -> Losing channel0 and 1 !!!!!

//Example3: Bit/Ws clk
//   {PCM186X_LRK_DIV,          0x1F},   //reg 39. LRCK = BCLK/32
//   {PCM186X_BCK_DIV,	        0x0F},   //reg 38, BCLK = 1/16 XTal = 1.53 MHz, LRCK = BCLK/32 = ~48 KHz

   {PCM186X_PGA_VAL_CH1_L,    0x40},   //reg 1, PGA CH1_L=32 dB
   {PCM186X_PGA_VAL_CH1_R,    0x40},   //reg 2, PGA CH1_R=32 dB
   {PCM186X_PGA_VAL_CH2_L,    0x40},   //reg 3, PGA CH2_L=32 dB
   {PCM186X_PGA_VAL_CH2_R,    0x40},   //reg 4, PGA CH2_R=32 dB
   {PCM186X_ADC1_INPUT_SEL_L, 0x41},   //reg 6, CH1_L=normal
   {PCM186X_ADC1_INPUT_SEL_R, 0x41},   //reg 7, CH1_R=normal
   {PCM186X_ADC2_INPUT_SEL_L, 0x42},   //reg 8, CH2_L=normal
   {PCM186X_ADC2_INPUT_SEL_R, 0x43},   //reg 9, CH2_R=normal
   //GPIO
   {PCM186X_GPIO1_0_CTRL,     0x05},   //reg 16, Gpio0 = DOUT2, polarity normal, Gpio1 = gpio
   //I2S mode
   {PCM186X_PCM_CFG,          0x4C},   //reg 11, set FMT=I2S, 16bit
   //Enable
   {PCM186X_CLK_CTRL,         0x91},   //reg 32, Set as timing master on PCM1865 (Xtal, Master, Enable)
}; 

#define  SETTINGS_I2S_MASTER_LENGTH    (sizeof(settings_i2s_master) / sizeof(settings_i2s_master[0]))

const Pcm1865_Settings_t settings_i2s_slave[] = 
{
   {PCM186X_PGA_VAL_CH1_L,    0x40},   //reg 1, PGA CH1_L=32 dB
   {PCM186X_PGA_VAL_CH1_R,    0x40},   //reg 2, PGA CH1_R=32 dB
   {PCM186X_PGA_VAL_CH2_L,    0x40},   //reg 3, PGA CH2_L=32 dB
   {PCM186X_PGA_VAL_CH2_R,    0x40},   //reg 4, PGA CH2_R=32 dB
   {PCM186X_ADC1_INPUT_SEL_L, 0x41},   //reg 6, CH1_L
   {PCM186X_ADC1_INPUT_SEL_R, 0x41},   //reg 7, CH1_R
   {PCM186X_ADC2_INPUT_SEL_L, 0x42},   //reg 8, CH2_L
   {PCM186X_ADC2_INPUT_SEL_R, 0x43},   //reg 9, CH2_R
   //GPIO
   {PCM186X_GPIO1_0_CTRL,     0x05},   //reg 16, Gpio0 = DOUT2, polarity normal, Gpio1 = gpio
   //I2S mode
   {PCM186X_PCM_CFG,          0x4C},   //reg 11, set FMT=I2S, 16bit 
}; 

#define  SETTINGS_I2S_SLAVE_LENGTH     (sizeof(settings_i2s_slave) / sizeof(settings_i2s_slave[0]))

/**
* Locals
*/
static char       strng[64];

const int i2c_address_table[Pcm1865_Device_None] =
{
   I2C_DEVICE_ADDRESS_PCM1865_MST,
   I2C_DEVICE_ADDRESS_PCM1865_SLV
};   

static uint8_t    current_page[Pcm1865_Device_None] = {0,0};

/**
* I2C bus
*/
static   DEV_IIC_PTR    i2c_arduino;

static int init_i2c_slave (Pcm1865_Device_t id)
{
   //Switch I2C slave address
	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(i2c_address_table[id])) != E_OK)
   {
      return (1);
   }
   
   return (0); //ok 
}

static int switch_page (Pcm1865_Device_t id, int page)
{
   if (page != current_page[id])
   {
      uint8_t data[2];
      
      //EMBARC_PRINTF(" id[%d] switch to page: %d\n\r", id, page);

      current_page[id]  = page;
      data[0] = PCM186X_CHANGE_PAGE;
      data[1] = current_page[id];
   	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)) != E_OK)
      {
         return (1);
      }
   	if (i2c_arduino->iic_write((uint8_t *)&data, 2) != 2)
      {
         return (1);
      }
   }

   return (0); //ok, also when no page sw done
}


/*******************************************************************************
* Externals
*/
int pcm1865_initI2c (void)
{
	i2c_arduino = iic_get_dev(DFSS_IIC_1_ID);
	if (i2c_arduino->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD) != E_OK)
   {
      return (1);
   }

   return (0);
}

int pcm1865_init (Pcm1865_Device_t id, int config)
{
   Pcm1865_Settings_t *ptr;
   int      length = 0;

   //EMBARC_PRINTF("@@ %s(%d, %d) @@\n\r", __FUNCTION__, id, config);

   if ((id != Pcm1865_Device_Master) && (id != Pcm1865_Device_Slave))
   {
      return (1);
   }

   //Set slave address correct
   if (init_i2c_slave(id) != 0)
   {
      return (1);
   }

   //Set pointer correct
   switch (config)
   {
      case PCM1865_CONFIG_TDM_MASTER:
         ptr      = (Pcm1865_Settings_t *)settings_tdm_master;
         length   = SETTINGS_TDM_MASTER_LENGTH; 
         break;

      case PCM1865_CONFIG_TDM_SLAVE:
         ptr      = (Pcm1865_Settings_t *)settings_tdm_slave;
         length   = SETTINGS_TDM_SLAVE_LENGTH; 
         break;

      case PCM1865_CONFIG_I2S_MASTER:
         ptr      = (Pcm1865_Settings_t *)settings_i2s_master;
         length   = SETTINGS_I2S_MASTER_LENGTH; 
         break;

      case PCM1865_CONFIG_I2S_SLAVE:
         ptr      = (Pcm1865_Settings_t *)settings_i2s_slave;
         length   = SETTINGS_I2S_SLAVE_LENGTH; 
         break;

      default:
         return (1);
   }
   
   for (int i=0; i<length; i++)
   {
      uint8_t data[2];
      
      //Check if page-swap is necessary
      if (switch_page(id, (uint8_t)(ptr[i].reg >> 8)) != 0)
      {
         return (1);
      }

      data[0] = (uint8_t)(ptr[i].reg  & 0xFF);
      data[1] = (uint8_t)(ptr[i].val);
   	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)) != E_OK)
      {
         return (1);
      }
   	if (i2c_arduino->iic_write((uint8_t *)&data, 2) != 2)
      {
         return (1);
      }
   }   

   return (0); //ok         
}

int pcm1865_reset (Pcm1865_Device_t id)
{
   uint8_t data[2];

   //EMBARC_PRINTF("@@ %s(%d) @@\n\r", __FUNCTION__, id);

   if ((id != Pcm1865_Device_Master) && (id != Pcm1865_Device_Slave))
   {
      return (1);
   }

   //Set slave address correct
   if (init_i2c_slave(id) != 0)
   {
      return (1);
   }
   
   //Switch to page0
   if (switch_page(id, 0) != 0)
   {
      return (1);
   }

   data[0] = (uint8_t)PCM186X_CHANGE_PAGE;
   data[1] = 0xFE;
	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)) != E_OK)
   {
      return (1);
   }
	if (i2c_arduino->iic_write((uint8_t *)&data, 2) != 2)
   {
      return (1);
   }

   return (0); //ok
}


int pcm1865_writeRamAddr (Pcm1865_Device_t id, Pcm1865_VirtualRamAddress_t addr, uint32_t value)
{
   uint8_t data[2] = {0,0};

   EMBARC_PRINTF("@@ %s(%d, %d, 0x%x) @@\n\r", __FUNCTION__, id, addr, value);

   if ((id != Pcm1865_Device_Master) && (id != Pcm1865_Device_Slave))
   {
      return (1);
   }

   //Set slave address correct
   if (init_i2c_slave(id) != 0)
   {
      return (1);
   }

   //Switch to page1
   if (switch_page(id, 1) != 0)
   {
      return (1);
   }

   //Check busy flag
   data[0] = (uint8_t)PCM186X_MMAP_STAT_CTRL;
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	if (i2c_arduino->iic_read((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
   while (data[0] != 0x00)
   {   
      data[0] = (uint8_t)PCM186X_MMAP_STAT_CTRL;
   	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
   	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
      {
         return (1);
      }
   	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
   	if (i2c_arduino->iic_read((uint8_t *)&data[0], 1) != 1)
      {
         return (1);
      }
   }

   //Write address   
   data[0] = (uint8_t)PCM186X_MMAP_ADDRESS;
   data[1] = (uint8_t)addr;
	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)) != E_OK)
   {
      return (1);
   }
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 2) != 2)
   {
      return (1);
   }

   //Write data
   data[0] = (uint8_t)PCM186X_MEM_WDATA0;
   data[1] = (uint8_t)(value >> 16); //MSB
	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)) != E_OK)
   {
      return (1);
   }
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 2) != 2)
   {
      return (1);
   }

   data[0] = (uint8_t)PCM186X_MEM_WDATA1;
   data[1] = (uint8_t)(value >> 8);
	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)) != E_OK)
   {
      return (1);
   }
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 2) != 2)
   {
      return (1);
   }

   data[0] = (uint8_t)PCM186X_MEM_WDATA2;
   data[1] = (uint8_t)(value >> 0); //LSB
	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)) != E_OK)
   {
      return (1);
   }
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 2) != 2)
   {
      return (1);
   }

   //Execute write (0x01) operation
   data[0] = (uint8_t)PCM186X_MMAP_STAT_CTRL;
   data[1] = 0x01;
	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)) != E_OK)
   {
      return (1);
   }
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 2) != 2)
   {
      return (1);
   }

   //Check busy flag
   data[0] = (uint8_t)PCM186X_MMAP_STAT_CTRL;
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	if (i2c_arduino->iic_read((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
   while ((data[0] & 0x04) == 0x04)
   {   
      data[0] = (uint8_t)PCM186X_MMAP_STAT_CTRL;
   	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
   	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
      {
         return (1);
      }
   	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
   	if (i2c_arduino->iic_read((uint8_t *)&data[0], 1) != 1)
      {
         return (1);
      }
   }

   return (0); //ok
}

int pcm1865_readRamAddr (Pcm1865_Device_t id, Pcm1865_VirtualRamAddress_t addr, uint32_t *value)
{
   uint8_t data[4] = {0,0,0,0};

   EMBARC_PRINTF("@@ %s(%d, %d) @@\n\r", __FUNCTION__, id, addr);

   if ((id != Pcm1865_Device_Master) && (id != Pcm1865_Device_Slave))
   {
      return (1);
   }

   //Set slave address correct
   if (init_i2c_slave(id) != 0)
   {
      return (1);
   }

   //Switch to page1
   if (switch_page(id, 1) != 0)
   {
      return (1);
   }

   //Check busy flag
   data[0] = (uint8_t)PCM186X_MMAP_STAT_CTRL;
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	if (i2c_arduino->iic_read((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
   while (data[0] != 0x00)
   {   
      data[0] = (uint8_t)PCM186X_MMAP_STAT_CTRL;
   	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
   	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
      {
         return (1);
      }
   	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
   	if (i2c_arduino->iic_read((uint8_t *)&data[0], 1) != 1)
      {
         return (1);
      }
   }

   //Write address   
   data[0] = (uint8_t)PCM186X_MMAP_ADDRESS;
   data[1] = (uint8_t)addr;
	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)) != E_OK)
   {
      return (1);
   }
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 2) != 2)
   {
      return (1);
   }

   //Execute read (0x02) operation
   data[0] = (uint8_t)PCM186X_MMAP_STAT_CTRL;
   data[1] = 0x02;
	if (i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)) != E_OK)
   {
      return (1);
   }
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 2) != 2)
   {
      return (1);
   }

   //Check busy flag
   data[0] = (uint8_t)PCM186X_MMAP_STAT_CTRL;
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	if (i2c_arduino->iic_read((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
   while ((data[0] & 0x04) == 0x04)
   {   
      data[0] = (uint8_t)PCM186X_MMAP_STAT_CTRL;
   	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
   	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
      {
         return (1);
      }
   	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
   	if (i2c_arduino->iic_read((uint8_t *)&data[0], 1) != 1)
      {
         return (1);
      }
   }

   //Read data
   data[0] = (uint8_t)PCM186X_MEM_RDATA0;
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	if (i2c_arduino->iic_read((uint8_t *)&data[1], 1) != 1)
   {
      return (1);
   }

   data[0] = (uint8_t)PCM186X_MEM_RDATA1;
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	if (i2c_arduino->iic_read((uint8_t *)&data[2], 1) != 1)
   {
      return (1);
   }

   data[0] = (uint8_t)PCM186X_MEM_RDATA2;
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
   {
      return (1);
   }
	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	if (i2c_arduino->iic_read((uint8_t *)&data[3], 1) != 1)
   {
      return (1);
   }

   //Return value
   *value = ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | ((uint32_t)data[3] << 0);

   return (0); //ok
}

/**
* Show functions
*/
int pcm1865_showPage (Pcm1865_Device_t id, int page)
{
   Pcm1865_Reg_Table_t *ptr;
   int      length = 0;

   //EMBARC_PRINTF("@@ %s(%d, %d) @@\n\r", __FUNCTION__, id, page);

   if ((id != Pcm1865_Device_Master) && (id != Pcm1865_Device_Slave))
   {
      return (1);
   }

   //Set slave address correct
   if (init_i2c_slave(id) != 0)
   {
      return (1);
   }
   
   switch (page)
   {
      case PCM186X_PAGE0:
         ptr      = (Pcm1865_Reg_Table_t *)pcm1865_page0_reg_table;
         length   = PCM1865_PAGE0_REG_TABLE_LENGTH; 
         break;
      case PCM186X_PAGE1:
         ptr      = (Pcm1865_Reg_Table_t *)pcm1865_page1_reg_table;
         length   = PCM1865_PAGE1_REG_TABLE_LENGTH;
         break; 
      case PCM186X_PAGE3:
         ptr      = (Pcm1865_Reg_Table_t *)pcm1865_page3_reg_table;
         length   = PCM1865_PAGE3_REG_TABLE_LENGTH;
         break; 
      case PCM186X_PAGE253:
         ptr      = (Pcm1865_Reg_Table_t *)pcm1865_page253_reg_table;
         length   = PCM1865_PAGE253_REG_TABLE_LENGTH;
         break; 
      
      default:
         return (1);
   }

   //Switch to page
   if (switch_page(id, page) != 0)
   {
      return (1);
   }

   //Read registers
   for (int i=0; i<length; i++)
   {
      uint8_t data[2];

      data[0] = (uint8_t)(ptr[i].reg & 0xFF);
   	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
   	if (i2c_arduino->iic_write((uint8_t *)&data[0], 1) != 1)
      {
         return (1);
      }
   	i2c_arduino->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
   	if (i2c_arduino->iic_read((uint8_t *)&data[0], 1) != 1)
      {
         return (1);
      }

      sprintf(strng, " PCM1865[%d] page[%d] @ [0x%2.2x] (%s) = 0x%2.2x\n\r",   id, page, (ptr[i].reg & 0xFF), ptr[i].str, data[0]);
      EMBARC_PRINTF(strng);   
   }   

   return (0); //ok
}


int pcm1865_showVRam (Pcm1865_Device_t id)
{
   EMBARC_PRINTF("@@ %s(%d) @@\n\r", __FUNCTION__, id);

   if ((id != Pcm1865_Device_Master) && (id != Pcm1865_Device_Slave))
   {
      return (1);
   }

   //Read registers
   for (int i=0; i<PCM1865_VRAM_TABLE_LENGTH; i++)
   {
      uint32_t value;
   
      if (pcm1865_readRamAddr(id, pcm1865_vram_table[i].addr, &value) != 0)
      {
         sprintf(strng, " Error I2C, cannot read PCM1865[%d] @ VRAM [0x%2.2x]\n\r", id, pcm1865_vram_table[i].addr);
         EMBARC_PRINTF(strng);
         return (1);
      }
      else
      {
         sprintf(strng, " PCM1865[%d] VRAM @ [0x%2.2x] (%s) = 0x%8.8x\n\r", id, pcm1865_vram_table[i].addr, pcm1865_vram_table[i].str, value);
         EMBARC_PRINTF(strng);
      }   
   }

   return (0); //ok
}   

