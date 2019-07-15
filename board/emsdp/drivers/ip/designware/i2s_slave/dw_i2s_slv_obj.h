#ifndef _DW_I2S_SLV_OBJ_H_
#define _DW_I2S_SLV_OBJ_H_

#include "ip/ip_hal/inc/dev_i2s.h"

//Enable
#define USE_DW_I2S_SLV_0      1

#ifdef __cplusplus
extern "C" {
#endif

extern DEV_I2S_PTR dw_i2s_slv_get_dev(int32_t i2s_id);

#ifdef __cplusplus
}
#endif

#endif

