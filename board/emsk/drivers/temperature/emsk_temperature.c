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
 * \date 2014-08-28
 * \author Huaqi Fang(Huaqi.Fang@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	BOARD_EMSK_DRV_PMODTEMP	EMSK PMOD-TMP Sensor Driver
 * \ingroup	BOARD_EMSK_DRIVER
 * \brief	EMSK PMOD Temperature Sensor Driver
 * \details
 *		Realize driver for PMOD temperature sensor using Designware iic driver.
 */

/**
 * \file
 * \ingroup	BOARD_EMSK_DRV_PMODTEMP
 * \brief	emsk pmod temperature sensor driver
 */

/**
 * \addtogroup	BOARD_EMSK_DRV_PMODTEMP
 * @{
 */
#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "dev_iic.h"
#include "emsk_temperature.h"
#include "board.h"

/*! ADT7420 registers */
#define ADT7420_REG_TEMP_MSB    0x00 /*!< Temperature value MSB */
#define ADT7420_REG_TEMP_LSB    0x01 /*!< Temperature value LSB */
#define ADT7420_REG_STATUS      0x02 /*!< Status */
#define ADT7420_REG_CONFIG      0x03 /*!< Configuration */
#define ADT7420_REG_T_HIGH_MSB  0x04 /*!< Temperature HIGH set point MSB */
#define ADT7420_REG_T_HIGH_LSB  0x05 /*!< Temperature HIGH set point LSB */
#define ADT7420_REG_T_LOW_MSB   0x06 /*!< Temperature LOW set point MSB */
#define ADT7420_REG_T_LOW_LSB   0x07 /*!< Temperature LOW set point LSB */
#define ADT7420_REG_T_CRIT_MSB  0x08 /*!< Temperature CRIT set point MSB */
#define ADT7420_REG_T_CRIT_LSB  0x09 /*!< Temperature CRIT set point LSB */
#define ADT7420_REG_HIST        0x0A /*!< Temperature HYST set point */
#define ADT7420_REG_ID          0x0B /*!< ID */
#define ADT7420_REG_RESET       0x2F /*!< Software reset */

/* ADT7420_REG_STATUS definition */
#define ADT7420_STATUS_T_LOW    (1 << 4)
#define ADT7420_STATUS_T_HIGH   (1 << 5)
#define ADT7420_STATUS_T_CRIT   (1 << 6)
#define ADT7420_STATUS_RDY      (1 << 7)

/* ADT7420_REG_CONFIG definition */
#define ADT7420_CONFIG_FAULT_QUEUE(x)   (x & 0x3)
#define ADT7420_CONFIG_CT_POL           (1 << 2)
#define ADT7420_CONFIG_INT_POL          (1 << 3)
#define ADT7420_CONFIG_INT_CT_MODE      (1 << 4)
#define ADT7420_CONFIG_OP_MODE(x)       ((x & 0x3) << 5)
#define ADT7420_CONFIG_RESOLUTION       (1 << 7)

/* ADT7420_CONFIG_FAULT_QUEUE(x) options */
#define ADT7420_FAULT_QUEUE_1_FAULT     0
#define ADT7420_FAULT_QUEUE_2_FAULTS    1
#define ADT7420_FAULT_QUEUE_3_FAULTS    2
#define ADT7420_FAULT_QUEUE_4_FAULTS    3

/* ADT7420_CONFIG_OP_MODE(x) options */
#define ADT7420_OP_MODE_CONT_CONV   0
#define ADT7420_OP_MODE_ONE_SHOT    1
#define ADT7420_OP_MODE_1_SPS       2
#define ADT7420_OP_MODE_SHUTDOWN    3

#define ADT7420_RESOLUTION_13BIT	(13)
#define ADT7420_RESOLUTION_16BIT	(16)


/* ADT7420 default ID */
#define ADT7420_DEFAULT_ID  0xCB

static int32_t temp_resolution = ADT7420_RESOLUTION_13BIT;
static DEV_IIC *emsk_temp_sensor;
static uint32_t temp_sensor_slvaddr;

#define EMSK_TEMP_SENSOR_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)

/**
 * \brief	write adt7420 register
 * \param 	regaddr register address
 * \param 	val     value to be written
 * \retval	>=0	write success, return bytes written
 * \retval	!E_OK	write failed
 */
int32_t adt7420_reg_write(uint8_t regaddr, uint8_t val)
{
	int32_t ercd = E_PAR;
	uint8_t data[2];

	emsk_temp_sensor = iic_get_dev(BOARD_TEMP_SENSOR_IIC_ID);

	EMSK_TEMP_SENSOR_CHECK_EXP_NORTN(emsk_temp_sensor!=NULL);

	data[0] = (uint8_t)(regaddr & 0xff);
	data[1] = (uint8_t)(val & 0xff);

	/** make sure set the temp sensor's slave address */
	emsk_temp_sensor->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(temp_sensor_slvaddr));

	ercd = emsk_temp_sensor->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = emsk_temp_sensor->iic_write(data, 2);

error_exit:
	return ercd;
}

/**
 * \brief	read adt7420 register
 * \param 	regaddr register address
 * \param 	val     returned value
 * \retval	>=0	read success, return bytes read
 * \retval	!E_OK	read failed
 */
int32_t adt7420_reg_read(uint8_t regaddr, uint8_t *val)
{
	int32_t ercd = E_PAR;
	uint8_t data[1];

	emsk_temp_sensor = iic_get_dev(BOARD_TEMP_SENSOR_IIC_ID);

	EMSK_TEMP_SENSOR_CHECK_EXP_NORTN(emsk_temp_sensor!=NULL);

	data[0] = (uint8_t)(regaddr & 0xff);

	/** make sure set the temp sensor's slave address */
	emsk_temp_sensor->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(temp_sensor_slvaddr));
	/* write register address then read register value */
	ercd = emsk_temp_sensor->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = emsk_temp_sensor->iic_write(data, 1);
	emsk_temp_sensor->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = emsk_temp_sensor->iic_read(data, 1);

	*val = data[0];

error_exit:
	return ercd;
}


/**
 * \brief	temperature sensor init
 * \param[in]	slv_addr	temperature sensor iic slave address
 * \retval	E_OK	init success
 * \retval	!E_OK	init failed
 */
int32_t temp_sensor_init(uint32_t slv_addr)
{
	int32_t ercd = E_OK;
	uint8_t config;

	emsk_temp_sensor = iic_get_dev(BOARD_TEMP_SENSOR_IIC_ID);

	EMSK_TEMP_SENSOR_CHECK_EXP_NORTN(emsk_temp_sensor!=NULL);

	ercd = emsk_temp_sensor->iic_open(DEV_MASTER_MODE, IIC_SPEED_HIGH);
	if ((ercd == E_OK) || (ercd == E_OPNED)) {
		ercd = emsk_temp_sensor->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slv_addr));
		temp_sensor_slvaddr = slv_addr;
		adt7420_reg_read(ADT7420_REG_CONFIG, &config);
		/** set ADT7420 resolution bit */
		if (temp_resolution == ADT7420_RESOLUTION_13BIT) {
			adt7420_reg_write(ADT7420_REG_CONFIG, (config & (~ADT7420_CONFIG_RESOLUTION)));
		} else {
			adt7420_reg_write(ADT7420_REG_CONFIG, (config | (ADT7420_CONFIG_RESOLUTION)));
		}
	}

error_exit:
	return ercd;
}

/** val is the return temperature in degrees Celsius (1/10 C) */
/**
 * \brief	read temperature sensor value in 0.1c unit
 * \param[out]	val	return temperature in degrees Celsius (1/10 C)
 * \retval	E_OK	read success
 * \retval	!E_OK	read failed
 */
int32_t temp_sensor_read(int32_t *val)
{
	int32_t ercd = E_OK;
	uint8_t data[2];
	int32_t temp;

	emsk_temp_sensor = iic_get_dev(BOARD_TEMP_SENSOR_IIC_ID);

	EMSK_TEMP_SENSOR_CHECK_EXP_NORTN(emsk_temp_sensor!=NULL);
	EMSK_TEMP_SENSOR_CHECK_EXP_NORTN(val!=NULL);

	ercd = adt7420_reg_read(ADT7420_REG_TEMP_MSB, &data[0]);
	ercd += adt7420_reg_read(ADT7420_REG_TEMP_LSB, &data[1]);

	if (ercd != 2) {
		ercd = E_OBJ;
	} else {
		ercd = E_OK;
		temp = ((int32_t)data[0] << 8) + ((int32_t)data[1]);
		/* conversion (val = val*0.0625) in Degrees */
		if (temp_resolution == ADT7420_RESOLUTION_13BIT) {
			temp >>= 3;
			if (temp & (1<<ADT7420_RESOLUTION_13BIT)) { /* negative temp */
				temp = ((temp-8192) * 10) >> 4;
			} else { /* positive temp */
				temp = (temp * 10) >> 4;
			}
		} else {
			if (temp & (1<<ADT7420_RESOLUTION_16BIT)) { /* negative temp */
				temp = ((temp-65536) * 10) >> 7;
			} else { /* positive temp */
				temp = (temp * 10) >> 7;
			}
		}
		*val = temp;
	}

error_exit:
	return ercd;
}

/** @} end of group BOARD_EMSK_DRV_PMODTEMP */