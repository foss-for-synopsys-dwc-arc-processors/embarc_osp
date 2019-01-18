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

#include "embARC.h"
#include "embARC_debug.h"
#include "mpu9250.h"
#include "stdio.h"

#if defined(BOARD_IOTDK) || defined(BOARD_EMSDP)
#define MPU9250_IIC_ID DFSS_IIC_0_ID
#else
#define MPU9250_IIC_ID 0
#endif

static MPU9250_DEFINE(mpu9250_sensor, MPU9250_IIC_ID, MPU9250_IIC_ADDRESS);

int main(void)
{
	MPU9250_DATA mpu9250_data = { 0 };


	mpu9250_sensor_init(mpu9250_sensor);
	EMBARC_PRINTF("\r\n\r\n\r\n");
	while (1) {
		mpu9250_sensor_read(mpu9250_sensor, &mpu9250_data);
#ifdef MPU9250_USE_DMP
		char datap[10];
		char datar[10];
		char datay[10];
		sprintf(datap, "%06.1f", mpu9250_data.pitch);
		sprintf(datar, "%06.1f", mpu9250_data.roll);
		sprintf(datay, "%06.1f", mpu9250_data.yaw);

		EMBARC_PRINTF("dmp:  pitch=%s,  roll=%s,  yaw=%s \r\n", datap, datar, datay);
		board_delay_ms(100, 1);
		EMBARC_PRINTF("\x1b[2k\x1b\x45");
#else
		EMBARC_PRINTF("accel   x=%5d,   y=%5d,   z=%5d \r\n", mpu9250_data.accel_x, mpu9250_data.accel_y, mpu9250_data.accel_z);
		EMBARC_PRINTF("gpro    x=%5d,   y=%5d,   z=%5d \r\n", mpu9250_data.gyro_x, mpu9250_data.gyro_y, mpu9250_data.gyro_z);
		EMBARC_PRINTF("mag     x=%5d,   y=%5d,   z=%5d \r\n", mpu9250_data.mag_x, mpu9250_data.mag_y, mpu9250_data.mag_z);
		board_delay_ms(100, 1);
		EMBARC_PRINTF("\x1b[2k\x1b\x45");
		EMBARC_PRINTF("\x1b[2k\x1b\x45");
		EMBARC_PRINTF("\x1b[2k\x1b\x45");
#endif
	}

	return E_SYS;
}


