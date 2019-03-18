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
//#define DBG_MORE

#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"

#include "dev_iic.h"
#include "ad7991.h"
#include "board.h"

/** AD7991_REG_CONFIG definition */
#define AD7991_CONFIG_BIT_TRAIL_DELAY	(1 << 0)
#define AD7991_CONFIG_SAMPLE_DELAY	(1 << 1)
#define AD7991_CONFIG_FLTR		(1 << 2)
#define AD7991_CONFIG_REF_SEL		(1 << 3)
/** AD7991_CONFIG_CH0 ~ CH3 defines see ad7991.h  */

#define EMSK_ADC_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)

/**
 * \brief	write ad7991 register
 * \param[in]	obj	ad7991 object
 * \param[in] 	val	config value to write
 * \retval	>=0	write success, return bytes written
 * \retval	!E_OK	write failed
 */
static int32_t _ad7991_reg_write(AD7991_DEF_PTR obj, uint8_t val)
{
	int32_t ercd = E_PAR;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->dev_id);

	//dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	EMSK_ADC_CHECK_EXP_NORTN(iic_obj!=NULL);

	/** make sure set the slave address */
	iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(obj->slvaddr));
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_write(&val, 1);

error_exit:
	return ercd;
}

/**
 * \brief	read ad7991 register
 * \param[in]	obj		ad7991 object
 * \param[in] 	val		pointer to buffer to read
 * \param[in] 	channel_num	active channel number
 * \retval	>=0		read success, return bytes read
 * \retval	<E_OK	read failed
 */
static int32_t _ad7991_reg_read(AD7991_DEF_PTR obj, uint8_t *val, uint8_t channel_num)
{
	int32_t ercd = E_PAR;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->dev_id);

	//dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	EMSK_ADC_CHECK_EXP_NORTN(iic_obj!=NULL);

	/** make sure set the slave address */
	iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(obj->slvaddr));
	/** read 2 bytes value */
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_read(val, 2*channel_num);

error_exit:
	return ercd;
}

/**
 * \brief	ad7991 init function
 * \param[in]	obj	ad7991 object
 * \retval	E_OK	init success
 * \retval	!E_OK	init failed
 */
int32_t ad7991_adc_init(AD7991_DEF_PTR obj)
{
	int32_t ercd = E_OK;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->dev_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	EMSK_ADC_CHECK_EXP_NORTN(iic_obj!=NULL);

	ercd = iic_obj->iic_open(DEV_MASTER_MODE, IIC_SPEED_HIGH);
	if ((ercd == E_OK) || (ercd == E_OPNED)) {
		ercd = iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(obj->slvaddr));
		obj->ref_source = AD7991_REF_VDD;
		obj->ref_voltage_mv = AD2_REF_VDD;
		ad7991_ref_select(obj, AD7991_REF_VDD);
		/*AD7991 default open 4 channel on power on, default config reg value is 0xF0*/
		ad7991_adc_channel(obj, 0xF0);
	}

error_exit:
	return ercd;
}

/**
 * \brief	ad7991 deinit
 * \param[in]	obj	ad7991 object
 * \retval	E_OK	init success
 * \retval	!E_OK	init failed
 */
int32_t ad7991_adc_deinit(AD7991_DEF_PTR obj)
{
	int32_t ercd = E_OK;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->dev_id);
	ercd = iic_obj->iic_close();
	return ercd;
}

/**
 * \brief	ad7991 reference voltage source selection
 * \param[in]	obj		ad7991 object
 * \param[in]	source	sensor working source, choose from enum sensor_op_mode
 * \retval		E_OK	always return E_OK
 */
int32_t ad7991_ref_select(AD7991_DEF_PTR obj, int32_t source)
{

	if(source == AD7991_REF_EXTERNAL) {
		obj->ref_source = source;
		obj->ref_voltage_mv = AD2_REF_EXTERNAL;
		obj->reg_config |= AD7991_CONFIG_REF_SEL;
		/* choose external reference uses VIN4 as REF, so there are only 3 channels available, disable CH3 here*/
		ad7991_adc_channel(obj, obj->reg_config & ~AD7991_CONFIG_CH3_EN);
	} else { /*< AD7991_REF_VDD is default setting*/
		obj->ref_source = AD7991_REF_VDD;
		obj->ref_voltage_mv = AD2_REF_VDD;
		obj->reg_config &= ~AD7991_CONFIG_REF_SEL;
	}
		/** set working source bits: first clear bits to zero, then set bits to correct source */
		dbg_printf(DBG_MORE_INFO, "[%s]%d: after: reg 0x%x , source 0x%x\r\n", __FUNCTION__, __LINE__, obj->reg_config, obj->ref_source);
		_ad7991_reg_write(obj, obj->reg_config);
	return E_OK;
}

/**
 * \brief	ad7991 adc channel setting
 * \param[in]	obj	        ad7991 object
 * \param[in]	channel		channel selection
 * \retval	E_OK		always return E_OK
 */
int32_t ad7991_adc_channel(AD7991_DEF_PTR obj, int8_t channel)
{
	uint8_t channel_num = 0;
	if (channel & AD7991_CONFIG_CH0_EN) {
		channel_num ++;
	}
	if (channel & AD7991_CONFIG_CH1_EN) {
		channel_num ++;
	}
	if (channel & AD7991_CONFIG_CH2_EN) {
		channel_num ++;
	}
	if ((obj->ref_source == AD7991_REF_VDD) && (channel & AD7991_CONFIG_CH3_EN)){
		channel_num ++;
	}
	/**clear channel bits in config reg and then set bits*/
	obj->reg_config = (obj->reg_config & 0x0F) | channel;
	obj->channel_num = channel_num;
	_ad7991_reg_write(obj, obj->reg_config);
	return E_OK;
}

/**
 * \brief	read 4 channel ADC converted value in 1.0 mV unit
 * \param[in]	obj	ad7991 object
 * \param[out]	val	need at least float [4] array to run, return latest avaiable convert data, array index corresponding to channel
 * \retval	E_OK	read success
 * \retval	!E_OK	read failed
 */
int32_t ad7991_adc_read(AD7991_DEF_PTR obj, float *val)
{
	int32_t ercd = E_OK;
	uint8_t data[8] = {0};
	uint8_t index;
	int16_t voltage;
	int16_t actual_ref;

	EMSK_ADC_CHECK_EXP_NORTN(val!=NULL);

	ercd = _ad7991_reg_read(obj, data, obj->channel_num);

	if (ercd != obj->channel_num * 2) {
		ercd = E_OBJ;
	} else {
		dbg_printf(DBG_MORE_INFO, "[%s]%d: ercd 0x%x, channel_num%d\r\n", __FUNCTION__, __LINE__, ercd, obj->channel_num);
		ercd = E_OK;
		dbg_printf(DBG_MORE_INFO, "[%s]%d: data[0] 0x%x; data[1] 0x%x\r\n", __FUNCTION__, __LINE__, data[0], data[1]);
		dbg_printf(DBG_MORE_INFO, "[%s]%d: data[0] 0x%x; data[1] 0x%x\r\n", __FUNCTION__, __LINE__, data[2], data[3]);
		dbg_printf(DBG_MORE_INFO, "[%s]%d: data[0] 0x%x; data[1] 0x%x\r\n", __FUNCTION__, __LINE__, data[4], data[5]);
		dbg_printf(DBG_MORE_INFO, "[%s]%d: data[0] 0x%x; data[1] 0x%x\r\n", __FUNCTION__, __LINE__, data[6], data[7]);
		if(obj->ref_source == AD7991_REF_VDD) {
			actual_ref = AD2_REF_VDD;
		} else {
			actual_ref = AD2_REF_EXTERNAL;
		}
		/* use index to put value, so if there are more values than channel_num, new value will overwrite old value */
		voltage = ((int16_t)(data[0] &0x0F) << 8) + ((int16_t)data[1]);
		index = data[0]>>4;
		dbg_printf(DBG_LESS_INFO, "[%s]%d: voltage[0] %x\r\n", __FUNCTION__, __LINE__, voltage);
		val[index] = voltage * actual_ref /4096.0;
		voltage = ((int16_t)(data[2] &0x0F) << 8) + ((int16_t)data[3]);
		index = data[2]>>4;
		dbg_printf(DBG_LESS_INFO, "[%s]%d: voltage[1] %x\r\n", __FUNCTION__, __LINE__, voltage);
		val[index] = voltage * actual_ref /4096.0;
		voltage = ((int16_t)(data[4] &0x0F) << 8) + ((int16_t)data[5]);
		index = data[4]>>4;
		dbg_printf(DBG_LESS_INFO, "[%s]%d: voltage[2] %x\r\n", __FUNCTION__, __LINE__, voltage);
		val[index] = voltage * actual_ref /4096.0;
		voltage = ((int16_t)(data[6] &0x0F) << 8) + ((int16_t)data[7]);
		index = data[6]>>4;
		dbg_printf(DBG_LESS_INFO, "[%s]%d: voltage[3] %x\r\n\r\n", __FUNCTION__, __LINE__, voltage);
		val[index] = voltage * actual_ref /4096.0;
	}

error_exit:
	return ercd;
}

