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
--------------------------------------------- */

#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"

#include "dev_iic.h"
#include "adt7420.h"
#include "board.h"

/*! ADT7420 registers */
enum registers {
	ADT7420_REG_TEMP_MSB    = 0x00, /*!< ADT7420 Temperature value MSB */
	ADT7420_REG_TEMP_LSB    = 0x01, /*!< ADT7420 Temperature value LSB */
	ADT7420_REG_STATUS      = 0x02, /*!< ADT7420 Status */
	ADT7420_REG_CONFIG      = 0x03, /*!< ADT7420 Configuration */
	ADT7420_REG_T_HIGH_MSB  = 0x04, /*!< ADT7420 Temperature HIGH set point MSB */
	ADT7420_REG_T_HIGH_LSB  = 0x05, /*!< ADT7420 Temperature HIGH set point LSB */
	ADT7420_REG_T_LOW_MSB   = 0x06, /*!< ADT7420 Temperature LOW set point MSB */
	ADT7420_REG_T_LOW_LSB   = 0x07, /*!< ADT7420 Temperature LOW set point LSB */
	ADT7420_REG_T_CRIT_MSB  = 0x08, /*!< ADT7420 Temperature CRIT set point MSB */
	ADT7420_REG_T_CRIT_LSB  = 0x09, /*!< ADT7420 Temperature CRIT set point LSB */
	ADT7420_REG_HIST        = 0x0A, /*!< ADT7420 Temperature HYST set point */
	ADT7420_REG_ID          = 0x0B, /*!< ADT7420 ID */
	ADT7420_REG_RESET       = 0x2F  /*!< ADT7420 Software reset */
};

/* ADT7420_REG_STATUS definition */
#define STATUS_T_LOW    (1 << 4)
#define STATUS_T_HIGH   (1 << 5)
#define STATUS_T_CRIT   (1 << 6)
#define STATUS_RDY      (1 << 7)

/* ADT7420_REG_CONFIG definition */
#define ADT7420_CONFIG_FAULT_QUEUE(x)   (x & 0x3)
#define ADT7420_CONFIG_CT_POL           (1 << 2)
#define ADT7420_CONFIG_INT_POL          (1 << 3)
#define ADT7420_CONFIG_INT_CT_MODE      (1 << 4)
#define ADT7420_CONFIG_OP_MODE(x)       ((x & 0x3) << 5)
#define ADT7420_CONFIG_RESOLUTION       (1 << 7)

/* ADT7420_CONFIG_FAULT_QUEUE(x) options */
enum fault_queue {
	FAULT_QUEUE_1 = 0,
	FAULT_QUEUE_2 = 1,
	FAULT_QUEUE_3 = 2,
	FAULT_QUEUE_4 = 3
};

/* ADT7420 default ID */
#define ADT7420_DEFAULT_ID  0xCB

#define ADT7420_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)

/**
 * \brief	write adt7420 register
 * \param[in]	obj	temperature sensor object
 * \param[in]	regaddr temperature sensor register address
 * \param[in] 	val	pointer to buffer to write
 * \param[in] 	len	length of data in buffer
 * \retval	>=0	write success, return bytes written
 * \retval	!E_OK	write failed
 */
static int32_t _adt7420_reg_write(ADT7420_DEF_PTR obj, uint8_t regaddr, uint8_t *val, uint8_t len)
{
	int32_t ercd = E_PAR;
	uint8_t data[1];
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_LESS_INFO, "[%s]%d: obj 0x%x, regaddr 0x%x, val 0x%x\r\n", __FUNCTION__, __LINE__, obj, regaddr, *val);
	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	ADT7420_CHECK_EXP_NORTN(iic_obj!=NULL);

	data[0] = (uint8_t)(regaddr & 0xff);

	/** make sure set the temp sensor's slave address */
	iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(obj->slvaddr));

	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = iic_obj->iic_write(data, 1);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_write(val, len);

error_exit:
	return ercd;
}

/**
 * \brief	read adt7420 register
 * \param[in]	obj	emperature sensor object
 * \param[in]	regaddr temperature sensor register address
 * \param[in] 	val	pointer to buffer to read
 * \param[in] 	len	length of data to read
 * \retval	>=0	read success, return bytes read
 * \retval	!E_OK	read failed
 */
static int32_t _adt7420_reg_read(ADT7420_DEF_PTR obj, uint8_t regaddr, uint8_t *val, uint8_t len)
{
	int32_t ercd = E_PAR;
	uint8_t data[1];
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	ADT7420_CHECK_EXP_NORTN(iic_obj!=NULL);

	data[0] = (uint8_t)(regaddr & 0xff);
	/** make sure set the temp sensor's slave address */
	iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(obj->slvaddr));
	/** write register address then read register value */
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = iic_obj->iic_write(data, 1);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_read(val, len);

error_exit:
	return ercd;
}

/**
 * \brief	adt7420 init
 * \param[in]	obj	temperature sensor object
 * \retval	E_OK	init success
 * \retval	!E_OK	init failed
 */
int32_t adt7420_sensor_init(ADT7420_DEF_PTR obj)
{
	int32_t ercd = E_OK;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	ADT7420_CHECK_EXP_NORTN(iic_obj!=NULL);

	ercd = iic_obj->iic_open(DEV_MASTER_MODE, IIC_SPEED_FAST);
	if ((ercd == E_OK) || (ercd == E_OPNED)) {
		ercd = iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(obj->slvaddr));
		/** Set to Default resolution and op_mode*/
		obj->resolution = ADT7420_RESOLUTION_13BIT;
		adt7420_sensor_resolution(obj, obj->resolution);
		obj->op_mode = ADT7420_OP_MODE_CONT_CONV;
		adt7420_sensor_mode(obj, obj->op_mode);
	}

error_exit:
	return ercd;
}

/**
 * \brief	temperature sensor deinit
 * \param[in]	obj	temperature sensor object
 * \retval	E_OK	init success
 * \retval	!E_OK	init failed
 */
int32_t adt7420_sensor_deinit(ADT7420_DEF_PTR obj)
{
	int32_t ercd = E_OK;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);
	ercd = iic_obj->iic_close();
	ADT7420_CHECK_EXP_NORTN(ercd == E_OK);
	/*Set back to default value*/
	obj->resolution = ADT7420_RESOLUTION_13BIT;
	obj->op_mode = ADT7420_OP_MODE_CONT_CONV;

error_exit:
	return ercd;
}

/**
 * \brief	temperature sensor deinit
 * \param[in]	obj		temperature sensor object
 * \param[in]	mode	sensor working mode, choose from enum sensor_op_mode
 * \retval	E_OK	always return E_OK
 */
int32_t adt7420_sensor_mode(ADT7420_DEF_PTR obj, int32_t mode){
	uint8_t config;
	obj->op_mode = mode;

	_adt7420_reg_read(obj, ADT7420_REG_CONFIG, &config, 1);
	/** set working mode bits: first clear bits to zero, then set bits to correct mode */
	config &= ~ADT7420_CONFIG_OP_MODE(0x03);
	config |= ADT7420_CONFIG_OP_MODE(obj->op_mode);
	_adt7420_reg_write(obj, ADT7420_REG_CONFIG, &config, 1);
	return E_OK;
}

/**
 * \brief	temperature sensor resolution setting
 * \param[in]	obj	        temperature sensor object
 * \param[in]	resolution	resolution of temperature sensor
 * \retval	E_OK	always return E_OK
 */
int32_t adt7420_sensor_resolution(ADT7420_DEF_PTR obj, int32_t resolution){
	uint8_t config;

	obj->resolution = resolution;
	_adt7420_reg_read(obj, ADT7420_REG_CONFIG, &config, 1);
	/** set ADT7420 resolution bit */
	if (obj->resolution == ADT7420_RESOLUTION_13BIT) {
		config &= ~ADT7420_CONFIG_RESOLUTION;
	} else {
		config |= ADT7420_CONFIG_RESOLUTION;
	}
	_adt7420_reg_write(obj, ADT7420_REG_CONFIG, &config, 1);
	return E_OK;
}

/**
 * \brief	read temperature sensor value in 0.1c unit
 * \param[in]	obj	temperature sensor object
 * \param[out]	val	return temperature in degrees Celsius (°C)
 * \retval	E_OK	read success
 * \retval	!E_OK	read failed
 */
int32_t adt7420_sensor_read(ADT7420_DEF_PTR obj, float *val)
{
	int32_t ercd = E_OK;
	uint8_t data[2];
	int16_t temp;

	ADT7420_CHECK_EXP_NORTN(val!=NULL);

	ercd = _adt7420_reg_read(obj, ADT7420_REG_TEMP_MSB, data, 2);

	if (ercd != 2) {
		ercd = E_OBJ;
	} else {
		ercd = E_OK;
		temp = ((int16_t)data[0] << 8) + ((int16_t)data[1]);
		dbg_printf(DBG_MORE_INFO, "[%s]%d: data[0] 0x%x; data[1] 0x%x\r\n", __FUNCTION__, __LINE__, data[0], data[1]);
		dbg_printf(DBG_LESS_INFO, "[%s]%d: temp %x\r\n", __FUNCTION__, __LINE__, temp);
		/* conversion (val = temp*0.0625) for 13bits[15:3] resolution and (val = temp*0.0078) for 16bits resolution in Degrees Celsius */
		if (obj->resolution == ADT7420_RESOLUTION_13BIT) {
			temp >>= 3;
			*val = temp*0.0625;
		} else {
			*val = temp*0.0078;
		}
	}

error_exit:
	return ercd;
}
