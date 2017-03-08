/*
 * ihex_load.c: Read Intel HEX format.
 *
 *
 * Copyright (c) 2013-2015 Kimmo Kulovesi, http://arkku.com
 * Provided with absolutely no warranty, use at your own risk only.
 * Distribute freely, mark modified copies as such.
 */

/*
 * Modified for port to ARC processor
 * by Xinyi Zhao(xinyi@synopsys.com), Synopsys, Inc.
 */

#include "ihex_load.h"
#include "embARC.h"
#include "embARC_debug.h"

struct ihex_state ihex;

void
ihex_start(){
	ihex_read_at_address(&ihex, 0);
}

ihex_err_t
ihex_load (ihex_source type, void *ptr) {
	ihex_count_t count;
	ihex_err_t err = IHEX_NO_FILE;
	if(type == IHEX_SD_CARD){
		FIL infile;
		char buf[256];
		if(f_open(&infile, (char *)ptr, FA_READ | FA_OPEN_EXISTING))
			return IHEX_NO_FILE;

		while (f_gets(buf, sizeof(buf), &infile)) {
			count = (ihex_count_t) strlen(buf);
			err = ihex_read_bytes(&ihex, buf, count);
			if(err != IHEX_NO_ERR){
				f_close(&infile);
				return err;
			}
		}
		f_close(&infile);
	} else if(type == IHEX_UART){
		/*count = ymodem static length*/
		count = (ihex_count_t) strlen(ptr);
		err = ihex_read_bytes(&ihex, (char *)ptr, count);
		return err;
	} else if(type == IHEX_WIFI){

	}
	return err;
}

ihex_err_t
ihex_data_read (struct ihex_state *ihex, ihex_record_type_t type, uint8_t error) {
	if (error) {
		return IHEX_CHECKSUM_ERR;
	}
	if ((error = (ihex->length < ihex->line_length))) {
		return IHEX_LINE_LENGTH_ERR;
	}

	if (type == IHEX_DATA_RECORD) {
		unsigned long address = (unsigned long) IHEX_LINEAR_ADDRESS(ihex);
		memcpy((void *)address, ihex->data, ihex->length);

	} else if (type == IHEX_END_OF_FILE_RECORD) {
		//file read over here
		return IHEX_END;
	}
	return IHEX_NO_ERR;
}