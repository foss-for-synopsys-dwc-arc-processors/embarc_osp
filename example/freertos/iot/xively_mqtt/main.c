/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \version 2017.03
 * \date 2016-04-20
 * \author Qiang Gu(Qiang.Gu@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_FREERTOS_IOT_XIVELY_MQTT	embARC Xively MQTT demo
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \ingroup	EMBARC_APPS_MID_LWIP
 * \ingroup	EMBARC_APPS_MID_MQTT
 * \brief	embARC Smart Home Demo. The emsk node will connect to the MQTT server of
 * 		Xivle.com to simulate a simple smart home scenario
 *
 * \details
 * ### Extra Required Tools
 *     * A brower to access xively.com
 *
 * ### Extra Required Peripherals
 *     * Digilent PMOD WIFI(MRF24WG0MA)
 *     * Digilent PMOD TMP2
 *
 * ### Design Concept
 *     The Pmod modules should be connected to \ref EMBARC_BOARD_CONNECTION "EMSK".
 *     This example is designed to show how MQTT protocol works in embARC, how an EMSK node connects to cloud.
 *     In the simulated smart home, you can monitor the temperature at home, and set lights on EMSK (right four leds to simulate
 *     bedroom light, left four leds to simulate parlor light) via buttons and observe changes on dashboard
 *     (http://fanghuaqi.github.io/xively-js/sample-app/)
 *
 * ### Usage Manual
 * 	* setup the network enviroment
 * 	* let EMSK connect to xively mqtt server
 * 	* control the lights by buttons or the dashboard for brower
 * 	* view the temperature from the dashboard
 * 	![ScreenShot of smarthome demo](pic/images/example/emsk/emsk_lwip_freertos_smarthome.jpg)
 *
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_IOT_XIVELY_MQTT
 * \brief	embARC smart home demo
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_IOT_XIVELY_MQTT
 * @{
 */
#include "embARC.h"
#include "embARC_debug.h"

#include "MQTTClient.h"


#define min(x,y) ((x) < (y) ? (x) : (y))


#define TSKPRI_XIVELYRX		(configMAX_PRIORITIES-2)	/**< xively rx task priority */
#define TSKPRI_XIVELYTX		(configMAX_PRIORITIES-3)	/**< xilvely tx task priority */


#define XI_FEED_CSV	"/v2/feeds/1125809548.csv" 			   // set xively feed csv relative path
#define XI_API_KEY	"6l20ldnHOuxUUQi5lwdCydj4iV2mDse7QggFMuDvKZhUlDLC" // set Xively API key (double-quoted string)
#define XI_HOST		"api.xively.com"
#define BEDROOM_LIGHT	"/v2/feeds/1125809548/datastreams/bedroom_light.csv"
#define PARLOR_LIGHT	"/v2/feeds/1125809548/datastreams/parlor_light.csv"

#define MSG_BEDROOM_LIGHT	0x10
#define MSG_PARLOR_LIGHT	0x20

static void task_xively_rx(void *par);
static void task_xively_tx(void *par);

static TaskHandle_t task_xivelytx_handle = NULL;
static TaskHandle_t task_xivelyrx_handle = NULL;

static uint32_t bedroom_light_on = 0;
static uint32_t parlor_light_on = 0;
static QueueHandle_t device_msg;
static char msg_string[128];


/* interrupt service routine for bedroom light button */
static void bedroom_button_isr(void *ptr)
{
	BaseType_t task_switch = pdFALSE;
	uint32_t msg = MSG_BEDROOM_LIGHT;

	if (bedroom_light_on) {
		bedroom_light_on = 0;
		led_write(0x0, 0xf0);
	} else {
		bedroom_light_on = 1;
		led_write(0xf0, 0xf0);
	}

	xQueueSendFromISR(device_msg, &msg, &task_switch );

	if(task_switch)
	{
		portYIELD_FROM_ISR ();
	}
}

/* interrupt service routine for parlor light button */
static void parlor_button_isr(void *ptr)
{
	BaseType_t task_switch = pdFALSE;
	uint32_t msg = MSG_PARLOR_LIGHT;

	if (parlor_light_on) {
		parlor_light_on = 0;
		led_write(0x0, 0xf);
	} else {
		parlor_light_on = 1;
		led_write(0xf, 0xf);
	}

	xQueueSendFromISR(device_msg, &msg, &task_switch );

	if(task_switch)
	{
		portYIELD_FROM_ISR ();
	}
}

static void lights_init(void) {
	DEV_GPIO_BIT_ISR bit_isr;
	DEV_GPIO_INT_CFG int_cfg;

	DEV_GPIO_PTR port = gpio_get_dev(EMSK_BUTTON_PORT);
	int_cfg.int_bit_mask = 0x3;
	int_cfg.int_bit_type = 0x3; /* edge trigger */
	int_cfg.int_bit_polarity = 0x0; /* falling sensitive */
	int_cfg.int_bit_debounce = 0x3; /* with debounce */

	port->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, &int_cfg);

	bit_isr.int_bit_ofs = 0;
	bit_isr.int_bit_handler = bedroom_button_isr;
	port->gpio_control(GPIO_CMD_SET_BIT_ISR, &bit_isr);

	bit_isr.int_bit_ofs = 1;
	bit_isr.int_bit_handler = parlor_button_isr;
	port->gpio_control(GPIO_CMD_SET_BIT_ISR, &bit_isr);

	port->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)0x3);
	led_write(0x0, 0xff);
}

int main(void)
{
	EMBARC_PRINTF("embARC Xively MQTT Smart Home Demo\r\n");

	lights_init();
	temp_sensor_init(BOARD_TEMP_IIC_SLVADDR);

	device_msg = xQueueCreate(10, sizeof( uint32_t ));

	if (xTaskCreate(task_xively_rx, "xively mqtt client", 512, (void *)1,
		TSKPRI_XIVELYRX, &task_xivelyrx_handle) != pdPASS) {
		EMBARC_PRINTF("create mqtt client failed\r\n");
		return -1;
	}


	return 0;
}

#define MQTT_CLIENT_SND_BUF_SIZE 512
#define MQTT_CLIENT_RCV_BUF_SIZE 512

static unsigned char snd_buf[MQTT_CLIENT_SND_BUF_SIZE];
static unsigned char rcv_buf[MQTT_CLIENT_RCV_BUF_SIZE];

static Network n;
static Client c;

static void parlor_light_handler(MessageData *md)
{
	MQTTMessage* msg;
	uint32_t val;
	char *s;
	msg = md->message;

	s = strchr((char *)(msg->payload), ',');
	s++;
	val = *s - '0';
	EMBARC_PRINTF("from sever: parlor %d\r\n", val);
	if (val) {
		parlor_light_on = 1;
		led_write(0xf, 0xf);
	} else {
		parlor_light_on = 0;
		led_write(0x0, 0xf);
	}
}

static void bedroom_light_handler(MessageData *md)
{
	MQTTMessage* msg;
	uint32_t val;
	char *s;
	msg = md->message;

	s = strchr((char *)(msg->payload), ',');
	s++;
	val = *s - '0';
	EMBARC_PRINTF("from sever: bedroom %d\r\n", val);
	if (val) {
		bedroom_light_on = 1;
		led_write(0xf0, 0xf0);
	} else {
		bedroom_light_on = 0;
		led_write(0x0, 0xf0);
	}
}

static void task_xively_rx(void *par)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	NewNetwork(&n);
	if(ConnectNetwork(&n, XI_HOST, 1883) < 0) {
		EMBARC_PRINTF("TCP connection failed\r\n");
		vTaskDelete(NULL);
	}

	MQTTClient(&c, &n, 5000, snd_buf, MQTT_CLIENT_SND_BUF_SIZE, rcv_buf, MQTT_CLIENT_RCV_BUF_SIZE);

	data.willFlag = 0;
	data.MQTTVersion = 3;
	data.clientID.cstring = "EMSK";
	data.username.cstring = XI_API_KEY;

	data.keepAliveInterval = 60;
	data.cleansession = 1;

	EMBARC_PRINTF("Connecting to xively\r\n");
	if (MQTTConnect(&c, &data) < 0 ) {
		EMBARC_PRINTF("mqtt client connect failed\r\n");
		vTaskDelete(NULL);
	}

	EMBARC_PRINTF("Subscribing topic:bedroom_light\r\n");
	if (MQTTSubscribe(&c, BEDROOM_LIGHT, 1, bedroom_light_handler) < 0) {
		EMBARC_PRINTF("subscribe bedroom_light topic failed\r\n");
		vTaskDelete(NULL);
	}

	EMBARC_PRINTF("Subscribing topic:parlor_light\r\n");
	if (MQTTSubscribe(&c, PARLOR_LIGHT, 1, parlor_light_handler) < 0) {
		EMBARC_PRINTF("subscribe parlor_light topic failed\r\n");
		vTaskDelete(NULL);
	}


	if (xTaskCreate(task_xively_tx, "xively update task", 512, (void *)1,
		TSKPRI_XIVELYTX, &task_xivelytx_handle) != pdPASS) {
		EMBARC_PRINTF("create xively update task failed\r\n");
		vTaskDelete(NULL);
	}


	while (1) {
		MQTTYield(&c, 10000);
	}
	MQTTDisconnect(&c);
	n.disconnect(&n);

	vTaskSuspend(NULL);
}

#define TIME_DELAY_UPDATE		(5)

static void task_xively_tx(void *par)
{
	int32_t val;
	float temp_val;
	uint32_t msg;
	int32_t delay_s = TIME_DELAY_UPDATE;
	MQTTMessage mqtt_msg;


	EMBARC_PRINTF("---update temperature every %ds---\r\n", delay_s);

	while(1) {

		if (xQueueReceive(device_msg, &msg, delay_s*1000)) {
			//EMBARC_PRINTF("msg:%x, parlor:%d bedroom:%d\r\n",msg,parlor_light_on,bedroom_light_on);
			if (msg == MSG_PARLOR_LIGHT) {
				sprintf(msg_string, "parlor_light,%d\r\n", parlor_light_on);
			} else if(msg == MSG_BEDROOM_LIGHT) {
				sprintf(msg_string, "bedroom_light,%d\r\n", bedroom_light_on);
			}

		} else {

			if (temp_sensor_read(&val) != E_OK) {
				temp_val = 0;
			} else {
				temp_val = val / 10.0;
			}
			sprintf(msg_string, "temperature,%.3f\r\n", temp_val);
		}

		mqtt_msg.qos = 0;
		mqtt_msg.retained = 0;
		mqtt_msg.payload = msg_string;
		mqtt_msg.payloadlen = strlen(msg_string);
		mqtt_msg.id = 0; /* fix converity issue : Uninitialized scalar variable */
		EMBARC_PRINTF("to server: %s",msg_string);
		if (MQTTPublish(&c, XI_FEED_CSV, &mqtt_msg) < 0) {
			EMBARC_PRINTF("---data publish failed, try again---\r\n");
		}
	}
}
/** @} */
