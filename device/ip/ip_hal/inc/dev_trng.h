
#ifndef _DEVICE_HAL_TRNG_H_
#define _DEVICE_HAL_TRNG_H_

#include "dev_common.h"

#define TRNG_VERSION_NIST				//switch between DW_NIST_TRNG version and DW_TRNG version: define this value to choose NIST, otherwise undefine it.

#define TRNG_CMD_SET_RAND_128_BIT_OUTPUT			DEV_SET_SYSCMD(0)

#define TRNG_CMD_SET_RAND_256_BIT_OUTPUT			DEV_SET_SYSCMD(1)

#define TRNG_CMD_SET_RAND_RESEED_AUTO				DEV_SET_SYSCMD(2)

#define TRNG_CMD_SET_RAND_RESEED_NONCE				DEV_SET_SYSCMD(3)

#define TRNG_CMD_SET_IN_CB							DEV_SET_SYSCMD(4)

#define TRNG_CMD_SET_OUT_CB							DEV_SET_SYSCMD(5)

#define TRNG_CMD_SET_ERR_CB							DEV_SET_SYSCMD(6)


typedef struct dev_trng_cbs {
	DEV_CALLBACK in_cb;	/*!< uart data transmit success required bytes callback */
	DEV_CALLBACK out_cb;	/*!< uart data receive success required bytes callback */
	DEV_CALLBACK err_cb;	/*!< uart error callback */
} DEV_TRNG_CBS, *DEV_TRNG_CBS_PTR;


typedef struct dev_trng_info {
	void *trng_ctrl;	/*!< trng control related pointer, implemented by bsp developer, and this should be set during trng object implementation */
	uint32_t opn_cnt;	/*!< trng open count, open it will increase 1, close it will decrease 1, 0 for close, >0 for open */
	uint32_t length;	/*!<  output length */
	uint32_t byte_generated;	/*!< count the bytes generated since last reseed, can be a reference and a reminder for next reseed operation */
	DEV_BUFFER in_buf;	/*!< input buffer via interrupt, this should be all zero for first open */
	DEV_BUFFER out_buf;	/*!< output buffer via interrupt, this should be all zero for first open */
	DEV_TRNG_CBS trng_cbs;
	void *extra;		/*!< a extra pointer to get hook to applications which should not used by bsp developer,
					this should be NULL for first open and you can \ref DEV_TRNG_INFO_SET_EXTRA_OBJECT "set"
					or \ref DEV_TRNG_INFO_GET_EXTRA_OBJECT "get" the extra information pointer */
} DEV_TRNG_INFO, * DEV_TRNG_INFO_PTR;

/** Set extra information pointer of trng info */
#define DEV_TRNG_INFO_SET_EXTRA_OBJECT(trng_info_ptr, extra_info)	(trng_info_ptr)->extra = (void *)(extra_info)
/** Get extra information pointer of trng info */
#define DEV_TRNG_INFO_GET_EXTRA_OBJECT(trng_info_ptr)			((trng_info_ptr)->extra)


typedef struct dev_trng {
	DEV_TRNG_INFO trng_info;					/*!< device information */

	int32_t (*trng_open) (void);				/*!< Open trng device */
	int32_t (*trng_close) (void);				/*!< Close trng device */
	int32_t (*trng_control) (uint32_t ctrl_cmd, void *param);	/*!< Control trng device */
	int32_t (*trng_read) (uint32_t *data_buf);	/*!< Read data from trng device(blocked) */
} DEV_TRNG, * DEV_TRNG_PTR;

#endif /* _DEVICE_HAL_TRNG_H_ */
