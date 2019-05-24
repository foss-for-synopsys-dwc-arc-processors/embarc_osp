#include <stdio.h>
#include <ctype.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "emsdp/drivers/mux/mux.h"
#include "pcm1865.h"
#include "dw_i2s.h"

//For reading key
static   DEV_UART_PTR            uart_console;
static   DEV_PWM_TIMER_PTR       pwm_timer;
static   DEV_GPIO_PTR            gpio;
static   DEV_I2S_PTR             i2s;

/**
* Mux
*/
static int set_arduinoMux (void)
{
   //Set I2C on AD4 and AD5      ->CFG6
   //Set PWM0_CH2 on D6          ->CFG3
   //Set GPIO0[4] on D4 (int2)   ->CFG2
   //Set GPIO0[0] on D0 (int)    ->CFG0
	set_arduino_mux(  ARDUINO_CFG0_GPIO	   \
				      |	ARDUINO_CFG1_GPIO	   \
				      |	ARDUINO_CFG2_GPIO	   \
      				|	ARDUINO_CFG3_PWM	   \
      				|	ARDUINO_CFG4_GPIO	   \
      				|	ARDUINO_CFG5_GPIO	   \
		      		|	ARDUINO_CFG6_I2C	
                  );
            
   return (0);            
}

/**
* PWM om Arduino D6
*/
static int set_pwmSignal (void)
{
   int errcnt = 0;

	pwm_timer = pwm_timer_get_dev(DW_PWM_TIMER_0_ID);
	errcnt += pwm_timer->pwm_timer_open();
   //Program channel2 with PWM signal 2 KHz, Duty Cycle 50%
   errcnt += pwm_timer->pwm_timer_write(2, DEV_PWM_TIMER_MODE_PWM, 2000, 50);
   //Enable timer 2
   errcnt += pwm_timer->pwm_timer_control(2, PWM_TIMER_CMD_ENA_CH, CONV2VOID(0));

   return (errcnt);
}

/**
* Interrupts from audio devices
*/
static int enable_int (Pcm1865_Device_t id)
{
   return (pcm1865_writeRamAddr(id, PCM1865_LOSS_THRESHOLD, 0x040C37));
}

//Back to reset value
static int disable_int (Pcm1865_Device_t id)
{
   return (pcm1865_writeRamAddr(id, PCM1865_LOSS_THRESHOLD, 0x000008));
}

/**
* Gpio pins on Arduino
*/
#define  GPIO_INT          0x00000011U //Gpio0.0 (D0), Gpio0.4 (D4)

static DEV_GPIO_HANDLER gpio00_hdlr (void)
{
   EMBARC_PRINTF("@@ Gpio00_hdlr() @@\n\r");
   
   return (0);   
}

static DEV_GPIO_HANDLER gpio04_hdlr (void)
{
   EMBARC_PRINTF("@@ Gpio04_hdlr() @@\n\r");
   
   return (0);   
}

static int set_gpio (void)
{
   int errcnt = 0;

   DEV_GPIO_INT_CFG param;
   DEV_GPIO_BIT_ISR hdlr;
   
   //Setup Gpio
   gpio = gpio_get_dev(DFSS_GPIO_0_ID);   
	errcnt += gpio->gpio_open(GPIO_INT);
	errcnt += gpio->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, CONV2VOID(GPIO_INT));

	param.int_bit_mask      = 0;        //1=masked
	param.int_bit_type      = GPIO_INT; //1=edge triggered
	param.int_bit_polarity  = GPIO_INT; //1=active high 
	param.int_bit_debounce  = GPIO_INT; //1=enable debounce
   errcnt += gpio->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, &param); 
   
	hdlr.int_bit_ofs     = 0; //bitpos 0
	hdlr.int_bit_handler = CONV2VOID(gpio00_hdlr);
   errcnt += gpio->gpio_control(GPIO_CMD_SET_BIT_ISR, &hdlr); 
	hdlr.int_bit_ofs     = 4; //bitpos 4
	hdlr.int_bit_handler = CONV2VOID(gpio04_hdlr);
   errcnt += gpio->gpio_control(GPIO_CMD_SET_BIT_ISR, &hdlr); 
   //Final commands, enable
	errcnt += gpio->gpio_control(GPIO_CMD_ENA_BIT_INT, CONV2VOID(GPIO_INT)); 

   return (errcnt);
}

/**
* I2S part
*/
typedef void (*DEV_I2S_HANDLER) (void *ptr);

#define NUM_OF_I2S_SAMPLES       (1024*128)

//Enable not all channels only channel3-ISR is connected to core
#define I2S_CHANNEL              (DW_I2S_CHANNEL3)

#ifdef DW_I2S_DATA_LENGTH_16
   static uint16_t         i2s_sample_buffer[NUM_OF_I2S_SAMPLES];
#else
   static uint32_t         i2s_sample_buffer[NUM_OF_I2S_SAMPLES];
#endif
static DW_I2S_BUFFER       i2s_dev_buffer;

//Handler
volatile uint32_t i2s_hdlr_cnt = 0;

////////////////////////////////////////////////////////////////////////////////
//REMARK !!!!!
//I2S Interrupt is blocking the while (1) loop !!!!!!!!!!!!!!!
////////////////////////////////////////////////////////////////////////////////

static DEV_I2S_HANDLER i2s_hdlr (void)
{
//   EMBARC_PRINTF("@@ I2S_hdlr() @@\n\r");
   i2s_hdlr_cnt++;
   
   return (0);   
}

static int set_i2s (void)
{
   int errcnt = 0;

#ifdef DW_I2S_DATA_LENGTH_16
   i2s_dev_buffer.buf = (uint16_t *)&i2s_sample_buffer[0];
#else   
   i2s_dev_buffer.buf = (uint32_t *)&i2s_sample_buffer[0];
#endif   
   i2s_dev_buffer.ofs = 0;
   i2s_dev_buffer.len = NUM_OF_I2S_SAMPLES;

   i2s = i2s_get_dev(DW_I2S_RX_ID);   
   errcnt += i2s->i2s_open(DEV_SLAVE_MODE, I2S_DEVICE_RECEIVER);
   errcnt += i2s->i2s_control(I2S_CMD_SET_RXINT, CONV2VOID(I2S_CHANNEL));  //Install int
   errcnt += i2s->i2s_control(I2S_CMD_SET_RXCB, &i2s_hdlr);                //Install handler
   errcnt += i2s->i2s_control(I2S_CMD_SET_RXINT_BUF, &i2s_dev_buffer);     //Install buffers

   return (errcnt);
}

static int enable_i2s (void)
{
   int errcnt = 0;

	errcnt += i2s->i2s_control(I2S_CMD_ENA_DEV, CONV2VOID(I2S_CHANNEL));    //Start channel0 (0) / channel3 (3)
   
   return (errcnt);
}

static void show_help (void)
{
   EMBARC_PRINTF("==== Help ====\n\r");
   EMBARC_PRINTF("press 1: Show content master PCM page0\n\r");
   EMBARC_PRINTF("press 2: Show content master PCM page1\n\r");
   EMBARC_PRINTF("press 3: Show content master PCM page3\n\r");
   EMBARC_PRINTF("press 4: Show content master PCM page253\n\r");
   EMBARC_PRINTF("press 5: Show content master PCM Vram\n\r");
   EMBARC_PRINTF("press r: Reset master & slave PCM\n\r");
   EMBARC_PRINTF("press n: Show number of interrupts received\n\r");
   EMBARC_PRINTF("press i: Init  master & slave PCM\n\r");
   EMBARC_PRINTF("press h: This help\n\r");
}

/**
* Externals
*/
int main(void)
{
#ifndef BOARD_EMSDP
   #error example only valid on emsdp board
#endif

	EMBARC_PRINTF("Silense Access Test\r\n");

   //Program mux towards PCB
   if (set_arduinoMux() != 0)
   {
      EMBARC_PRINTF(" Error, cannot set Arduino mux correct\n\r");
   }

   //Program PWM signal on D6
   if (set_pwmSignal () != 0)
   {
      EMBARC_PRINTF(" Error, cannot program pwm\n\r");
   }

   //Init/Open I2C bus connected to PCM devices
   if (pcm1865_initI2c() != 0)
   {
      EMBARC_PRINTF(" Error, cannot open I2C bus for pcm\n\r");
   }

   //Reset both audio devices
   if (pcm1865_reset (Pcm1865_Device_Master) != 0)
   {
      EMBARC_PRINTF("Error, cannot reset master pcm device\n\r");
   }
   if (pcm1865_reset (Pcm1865_Device_Slave) != 0)
   {
      EMBARC_PRINTF("Error, cannot reset slave pcm device\n\r");
   }
   board_delay_ms(100, 1);

   //Init I2S
   if (set_i2s () != 0)
   {
      EMBARC_PRINTF("Error, cannot program i2s\n\r");
   }

   //Init both audio devices-> generates interrupts
   if (pcm1865_init (Pcm1865_Device_Master, PCM1865_CONFIG_I2S_MASTER) != 0)
   {
      EMBARC_PRINTF("Error, cannot init pcm-1865 master\r\n");
   }
   if (pcm1865_init (Pcm1865_Device_Slave, PCM1865_CONFIG_I2S_SLAVE) != 0)
   {
      EMBARC_PRINTF("Error, cannot init pcm-1865 slave\r\n");
   }
   board_delay_ms(200, 1);

   //Don't forget this, otherwise no ISR on D4 and D0
   enable_int(Pcm1865_Device_Master);
   enable_int(Pcm1865_Device_Slave);

   //Init Gpio0.0 (D0) and Gpio0.4 (D4) as interrupt
   if (set_gpio() != 0)
   {
      EMBARC_PRINTF("Error, cannot program gpio\n\r");
   }

   //See if connection to audio devices is OK
   //(void)pcm1865_showVRam(Pcm1865_Device_Master);   
   //(void)pcm1865_showPage(Pcm1865_Device_Master, PCM186X_PAGE0);            

   //For keypress HyperTerminal
	uart_console = uart_get_dev(BOARD_CONSOLE_UART_ID);
   //not necessary, is printf, already open in board_main()
	//uart_console->uart_open(BOARD_CONSOLE_UART_BAUD);

   //Final before loop
   show_help();

   //Start I2S
   if (enable_i2s() != 0)
   {
      EMBARC_PRINTF("Error, cannot enable i2s\n\r");
   }
   
   while (1)
   {
      uint32_t value;

      /**
      * Check Hyperterminal for keypressed
      */
	   uart_console->uart_control(UART_CMD_GET_RXAVAIL, &value);
      if (value > 0)
      {
         char c; 
      
         //Keypressed received      
         c = console_getchar();
         switch (c)
         {
            case '1':
               if (pcm1865_showPage(Pcm1865_Device_Master, PCM186X_PAGE0) != 0)            
               {
                  EMBARC_PRINTF("Error, cannot show master pcm device page0\n\r");
               }
               break;

            case '2':
               if (pcm1865_showPage(Pcm1865_Device_Master, PCM186X_PAGE1) != 0)            
               {
                  EMBARC_PRINTF("Error, cannot show master pcm device page1\n\r");
               }
               break;

            case '3':
               if (pcm1865_showPage(Pcm1865_Device_Master, PCM186X_PAGE3) != 0)            
               {
                  EMBARC_PRINTF("Error, cannot show master pcm device page3\n\r");
               }
               break;

            case '4':
               if (pcm1865_showPage(Pcm1865_Device_Master, PCM186X_PAGE253) != 0)
               {
                  EMBARC_PRINTF("Error, cannot show master pcm device page253\n\r");
               }
               break;

            case '5':
               (void)pcm1865_showVRam(Pcm1865_Device_Master);   
               break;

            case 'r':
               EMBARC_PRINTF("Reset\n\r");
               if (pcm1865_reset (Pcm1865_Device_Master) != 0)
               {
                  EMBARC_PRINTF("Error, cannot reset master pcm device\n\r");
               }
               if (pcm1865_reset (Pcm1865_Device_Slave) != 0)
               {
                  EMBARC_PRINTF("Error, cannot reset slave pcm device\n\r");
               }
               //Delay
               board_delay_ms(100, 1);
               break;

            case 'i':
               EMBARC_PRINTF("Init\n\r");
               if (pcm1865_init (Pcm1865_Device_Master, PCM1865_CONFIG_I2S_MASTER) != 0)
               {
                  EMBARC_PRINTF("Error, cannot init pcm-1865 master\r\n");
               }
               if (pcm1865_init (Pcm1865_Device_Slave, PCM1865_CONFIG_I2S_SLAVE) != 0)
               {
                  EMBARC_PRINTF("Error, cannot init pcm-1865 slave\r\n");
               }
               //Delay
               board_delay_ms(100, 1);
               
               //Don't forget this, otherwise no ISR
               enable_int(Pcm1865_Device_Master);
               enable_int(Pcm1865_Device_Slave);
               break;

            case 'n':
               EMBARC_PRINTF("Num of I2S int: %d\r\n", i2s_hdlr_cnt);
               break;

            case 'h':
               show_help();
               break;

            default:
               EMBARC_PRINTF("-- key pressed %c --\r\n", c);
               break;   
         }
      }
   }      
      
	return E_SYS;
}

