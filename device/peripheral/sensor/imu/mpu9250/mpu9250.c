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
#include "mpu9250.h"
#include "board.h"

//****************************************
#define	SMPLRT_DIV	0x19
#define	CONFIG		0x1A
#define	GYRO_CONFIG	0x1B
#define	ACCEL_CONFIG	0x1C
#define	ACCEL_CONFIG_2	0x1D
#define INT_PIN_CFG	0x37
#define INT_ENABLE	0x38
#define USER_CTRL	0x6A
#define	PWR_MGMT_1	0x6B
#define PWR_MGMT_2 	0x6C
#define MAG_CTRL	0x0A

#define MPU_WIM		0x75
#define MPU_ID		0x71

#define MAG_WIM		0x00
#define MAG_ID		0x48

#define	ACCEL_XOUT_H	0x3B
#define	GYRO_XOUT_H	0x43
#define MAG_XOUT_L	0x03


#define MPU9250_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)

static int32_t _mpu_reg_write(MPU9250_DEF_PTR obj, uint32_t slaveaddr, uint8_t regaddr, uint8_t *val, uint8_t len)
{
	int32_t ercd = E_PAR;
	uint8_t data[1];
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_LESS_INFO, "[%s]%d: obj 0x%x, regaddr 0x%x, val 0x%x\r\n", __FUNCTION__, __LINE__, obj, regaddr, *val);
	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	MPU9250_CHECK_EXP_NORTN(iic_obj!=NULL);

	data[0] = (uint8_t)(regaddr & 0xff);

	iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slaveaddr));

	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = iic_obj->iic_write(data, 1);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_write(val, len);

error_exit:
	return ercd;
}

static int32_t _mpu_reg_read(MPU9250_DEF_PTR obj, uint32_t slaveaddr, uint8_t regaddr, uint8_t *val, uint8_t len)
{
	int32_t ercd = E_PAR;
	uint8_t data[1];
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	MPU9250_CHECK_EXP_NORTN(iic_obj!=NULL);

	data[0] = (uint8_t)(regaddr & 0xff);


	iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slaveaddr));


	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = iic_obj->iic_write(data, 1);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_read(val, len);

error_exit:
	return ercd;
}

int32_t mpu9250_sensor_init(MPU9250_DEF_PTR obj)
{
	int32_t ercd = E_OK;
	uint8_t config;
	uint8_t data[0];
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	MPU9250_CHECK_EXP_NORTN(iic_obj != NULL);

	ercd = iic_obj->iic_open(DEV_MASTER_MODE, IIC_SPEED_FAST);
	if ((ercd == E_OK) || (ercd == E_OPNED)) {
		config = 0x80;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, PWR_MGMT_1, &config, 1);//0x6B
		board_delay_ms(100, OSP_DELAY_OS_COMPAT_DISABLE);

		/*
		 * get stable time source; Auto select clock source to be PLL gyroscope reference if ready
		 * else use the internal oscillator
		 */
		config = 0x01;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, PWR_MGMT_1, &config, 1);
		config = 0x00;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, PWR_MGMT_2, &config, 1);
		/* no i2c master */
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, USER_CTRL, &config, 1);
		board_delay_ms(200, OSP_DELAY_OS_COMPAT_DISABLE);

		ercd = _mpu_reg_read(obj, obj->mpu_slvaddr, MPU_WIM, data, 1);

		if (data[0] != MPU_ID) {
			dbg_printf(DBG_MORE_INFO,"mpu init failed\r\n");
			return E_SYS;
		}

		config = 0x07;//SAMPLE_RATE=Internal_Sample_Rate(1khz) / (1 + SMPLRT_DIV)
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, SMPLRT_DIV, &config, 1);//Sample Rate Divider

		config = 0x06;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, CONFIG, &config, 1);//DLPF config: 5Hz

		config = 0x18;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, GYRO_CONFIG, &config, 1);//+2000dps

		config = 0x00;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, ACCEL_CONFIG, &config, 1);// +-2g

		config = 0x08;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, ACCEL_CONFIG_2, &config, 1);//1.13kHz

		config = 0x2; //set passby
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, INT_PIN_CFG, &config, 1);
		board_delay_ms(100, OSP_DELAY_OS_COMPAT_DISABLE);

		ercd = _mpu_reg_read(obj, obj->mag_slvaddr, MAG_WIM, data, 1);//read mag who i am;

		if (data[0] != MAG_ID) {
			dbg_printf(DBG_MORE_INFO,"mpu init failed\r\n");
			return E_SYS;
		}

		config = 0x01;
		ercd = _mpu_reg_write(obj, obj->mag_slvaddr, MAG_CTRL, &config, 1);//mag single measurement mode
	}

error_exit:
	return ercd;
}

int32_t mpu9250_sensor_deinit(MPU9250_DEF_PTR obj)
{
	int32_t ercd = E_OK;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);
	ercd = iic_obj->iic_close();
	MPU9250_CHECK_EXP_NORTN(ercd == E_OK);

error_exit:
	return ercd;
}

int32_t mpu9250_sensor_read(MPU9250_DEF_PTR obj, MPU9250_DATA_PTR mp_data)
{
	int32_t ercd = E_OK;
	uint8_t data[6];
	uint8_t config;

	MPU9250_CHECK_EXP_NORTN(mp_data!=NULL);

	ercd = _mpu_reg_read(obj, obj->mpu_slvaddr, GYRO_XOUT_H, data, 6);
	if (ercd != 6) {
		ercd = E_OBJ;
		goto error_exit;
	} else {
		ercd = E_OK;

		mp_data->gyro_x = ((int16_t)data[0] << 8) + ((int16_t)data[1]);
		mp_data->gyro_y = ((int16_t)data[2] << 8) + ((int16_t)data[3]);
		mp_data->gyro_z = ((int16_t)data[4] << 8) + ((int16_t)data[5]);
	}

	ercd = _mpu_reg_read(obj, obj->mpu_slvaddr, ACCEL_XOUT_H, data, 6);
	if (ercd != 6) {
		ercd = E_OBJ;
		goto error_exit;
	} else {
		ercd = E_OK;

		mp_data->accel_x = ((int16_t)data[0] << 8) + ((int16_t)data[1]);
		mp_data->accel_y = ((int16_t)data[2] << 8) + ((int16_t)data[3]);
		mp_data->accel_z = ((int16_t)data[4] << 8) + ((int16_t)data[5]);
	}

	ercd = _mpu_reg_read(obj, obj->mag_slvaddr, MAG_XOUT_L, data, 6);
	if (ercd != 6) {
		ercd = E_OBJ;
	} else {
		ercd = E_OK;

		mp_data->mag_x = ((int16_t)data[1] << 8) + ((int16_t)data[0]);
		mp_data->mag_y = ((int16_t)data[3] << 8) + ((int16_t)data[2]);
		mp_data->mag_z = ((int16_t)data[5] << 8) + ((int16_t)data[4]);
	}
	config = 0x01;
	ercd = _mpu_reg_write(obj, obj->mag_slvaddr, MAG_CTRL, &config, 1);//mag single measurement mode

error_exit:
	return ercd;
}
