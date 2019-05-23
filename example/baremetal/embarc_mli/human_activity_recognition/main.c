/* ------------------------------------------
 * Copyright (c) 2019, Synopsys, Inc. All rights reserved.

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
#include "inv_mpu.h"
#include "gpio.h"
#include "arc_timer.h"

#include "settings.h"
#include "input_buffer.h"
#include "process.h"

#include "har_smartphone_model.h"

#if defined(BOARD_IOTDK)
#define MPU9250_IIC_ID DFSS_IIC_0_ID
#else
#error Board isnt supported, check BOARD variable
#endif

static MPU9250_DEFINE(mpu9250_sensor, MPU9250_IIC_ID, MPU9250_IIC_ADDRESS);

static void set_i2c_irq_priority(MPU9250_DEF_PTR obj, uint32_t prio)
{
    DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);
    iic_obj->iic_control(IIC_CMD_SET_IRQ_PRIO, CONV2VOID(prio));
}


#define TIMER           TIMER_0
#define TIMER_IRQ       16
#define TIMER_RATE_HZ   50

static void timer_isr(void*param)
{
    static MPU9250_DATA mp_data;

    mpu9250_sensor_read(mpu9250_sensor, &mp_data);

    input_buffer_push_data(&mp_data);

    timer_int_clear(TIMER);

    return;
}

static void start_timer(void)
{
    int_handler_install(TIMER_IRQ, timer_isr);
    timer_start(TIMER, 1 /* IE = 1 */, (CORE_FREQ_MHZ * 1000000 / TIMER_RATE_HZ));
}

int main(void)
{
    pll_fout_config(CORE_FREQ_MHZ);
    set_i2c_irq_priority(mpu9250_sensor, 2);

    start_timer();
    input_buffer_init();
    preprocess_init();

    /* initializing the Sensor Hub */
    mpu9250_sensor_init(mpu9250_sensor);

    mpu_set_gyro_fsr(GYRO_DPS);
    mpu_set_accel_fsr(ACCEL_G);

    /* configuring GPIO to handle the "data ready" interrupt */
    while (1) {
        float *frame = input_buffer_get_next_buffer();

        if(frame != NULL) {
            preprocess_tensor_data(frame);
            har_smartphone_net();
            postprocess_tensor_data();
        } else {
            // Sleep to wait for the next frame
        }
    }

    return E_SYS;
}

#if 0
#ifndef _ARC
#define STATUS32      0x000a
void _init_ad(void) {
    uint32_t stat32 = _arc_aux_read(STATUS32) | 0x80000;
    __builtin_arc_flag(stat32);
}
#endif
#endif