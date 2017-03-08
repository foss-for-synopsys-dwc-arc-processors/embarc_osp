/*
 * kk_ihex_read.h: A simple library for reading Intel HEX data. See
 * the accompanying kk_ihex_write.h for IHEX write support.
 *
 *
 *	  READING INTEL HEX DATA
 *	  ----------------------
 *
 * To read data in the Intel HEX format, you must perform the actual reading
 * of bytes using other means (e.g., stdio). The bytes read must then be
 * passed to `ihex_read_byte` and/or `ihex_read_bytes`. The reading functions
 * will then call `ihex_data_read`, at which stage the `struct ihex_state`
 * structure will contain the data along with its address. See below for
 * details and example implementation of `ihex_data_read`.
 *
 * The sequence to read data in IHEX format is:
 *	  struct ihex_state ihex;
 *	  ihex_begin_read(&ihex);
 *	  ihex_read_bytes(&ihex, my_input_bytes, length_of_my_input_bytes);
 *	  ihex_end_read(&ihex);
 *
 *
 *	  CONSERVING MEMORY
 *	  -----------------
 *
 * For memory-critical use, you can save additional memory by defining
 * `IHEX_LINE_MAX_LENGTH` as something less than 255. Note, however, that
 * this limit affects both reading and writing, so the resulting library
 * will be unable to read lines with more than this number of data bytes.
 * That said, I haven't encountered any IHEX files with more than 32
 * data bytes per line.
 *
 *
 * Copyright (c) 2013-2015 Kimmo Kulovesi, http://arkku.com/
 * Provided with absolutely no warranty, use at your own risk only.
 * Use and distribute freely, mark modified copies as such.
 */

/*
 * Modified for port to ARC processor
 * by Xinyi Zhao(xinyi@synopsys.com), Synopsys, Inc.
 */

#ifndef IHEX_LOAD_H
#define IHEX_LOAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kk_ihex_read.h"

typedef enum
{
	IHEX_SD_CARD = 0,
	IHEX_UART,
	IHEX_WIFI
} ihex_source;

extern void ihex_start();
extern ihex_err_t ihex_load (ihex_source type, void *ptr);

#ifdef __cplusplus
}
#endif

#endif // !IHEX_LOAD_H