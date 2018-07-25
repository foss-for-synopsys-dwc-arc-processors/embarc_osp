
#include "embARC.h"
#include "embARC_debug.h"

#include "ntshell_common.h"
#include "ihex_load.h"
#include "parson.h"

#include "bootutil/bootutil.h"
#include "bootutil/image.h"
#include "bootutil_priv.h"
#include "flash_map/flash_map.h"

#include "test.h"

#define SLOT0_STARTADDRESS	0x10000000		
#define SLOT1_STARTADDRESS	0x13000000		

typedef int (*fp_t)(void);

const char *v1_boot_file = "0:\\v1_boot.bin";
const char *v2_boot_file = "0:\\v2_boot.bin";
const char *v3_boot_file = "0:\\v3_boot.bin";

static FIL file;

int load_image(const char *boot_file, uint32_t ram_address)
{
	uint8_t res;
	uint32_t cnt;
	void *ram;

	res = f_open(&file, boot_file, FA_READ | FA_OPEN_EXISTING);
	if (res) {
		EMBARC_PRINTF("%s open error\r\n", boot_file);
	}else{
		ram = (void *)ram_address;
		res = f_read(&file, ram, file.fsize, &cnt);
		f_close(&file);
		if (res || ((uint32_t)cnt != file.fsize))
		{
			EMBARC_PRINTF("%s read error\r\n", boot_file);
		}
	}
	return res;
}

/*set the trailers of SLOT0 & SLOT1 as 0xFFFFFFFF, simulate as the initial state of flash*/
int trailer_init(void)
{
	int rc = 0;
	uint32_t off;
	const struct flash_area *fap = NULL;

	rc = flash_area_open(FLASH_AREA_IMAGE_0, &fap);
    if (rc != 0) {
        goto done;
    }

    off = boot_status_off(fap);
    memset((void *)(fap->fa_off + off), 0xFF, fap->fa_size - off);
    flash_area_close(fap);

    rc = flash_area_open(FLASH_AREA_IMAGE_1, &fap);
    if (rc != 0) {
        goto done;
    }

    memset((void *)(fap->fa_off + off), 0xFF, fap->fa_size - off);
    flash_area_close(fap);

    rc = flash_area_open(FLASH_AREA_IMAGE_SCRATCH, &fap);
    if (rc != 0) {
        goto done;
    }

    off = boot_status_off(fap);
    memset((void *)(fap->fa_off + off), 0xFF, fap->fa_size - off);

done:
	flash_area_close(fap);
	return rc;
}

void test_slot0(void)
{
	uint8_t res;
	struct boot_rsp rsp;
	fp_t fp;

	board_init();
	cpu_unlock();	/* unlock cpu to let interrupt work */

	EMBARC_PRINTF("Start test_slot0\r\n");
	EMBARC_PRINTF("intial the trailers\r\n");
	res = trailer_init();
	if (res) {
		EMBARC_PRINTF("intial the trailers error\r\n");
		return;
	}

	uint32_t load_address = SLOT0_STARTADDRESS;
	EMBARC_PRINTF("loading image %s into SLOT0 at 0x%x\r\n", v1_boot_file, load_address);
	load_image(v1_boot_file, load_address);

	EMBARC_PRINTF("start mcuboot\r\n");
	res = boot_go(&rsp);
	if (res != 0) {
		EMBARC_PRINTF("\r\nvarify %s failed \r\n", v1_boot_file);
		while (1) {
			;
		}
	} else {
		EMBARC_PRINTF("\r\nvarify %s successfully \r\n", v1_boot_file);
	}

	EMBARC_PRINTF("boot the image\r\n");

	cpu_lock();
	fp = (fp_t)(rsp.br_image_off + rsp.br_hdr->ih_hdr_size + 4);
	fp();	/* jump to program */
}

void test_slot0_good_slot1(void)
{
	uint8_t res;
	struct boot_rsp rsp;
	fp_t fp;

	board_init();
	cpu_unlock();	/* unlock cpu to let interrupt work */

	EMBARC_PRINTF("Start test_slot0_good_slot1\r\n");
	EMBARC_PRINTF("intial the trailers\r\n");
	res = trailer_init();
	if (res) {
		EMBARC_PRINTF("intial the trailers error\r\n");
		return;
	}

	uint32_t load_address = SLOT0_STARTADDRESS;
	EMBARC_PRINTF("loading image %s into SLOT0 at 0x%x\r\n", v1_boot_file, load_address);
	load_image(v1_boot_file, load_address);

	load_address = SLOT1_STARTADDRESS;
	EMBARC_PRINTF("loading image %s into SLOT1 at 0x%x\r\n", v2_boot_file, load_address);
	load_image(v2_boot_file, load_address);

	EMBARC_PRINTF("set the magic of SLOT1\r\n");
	boot_set_pending(0);

	EMBARC_PRINTF("start mcuboot\r\n");
	res = boot_go(&rsp);
	if (res != 0) {
		EMBARC_PRINTF("\r\nvarify iamge failed \r\n");
		while (1) {
			;
		}
	} else {
		EMBARC_PRINTF("\r\nvarify image successfully \r\n");
	}

	EMBARC_PRINTF("confirm the image_ok\r\n");
	boot_set_confirmed();

	EMBARC_PRINTF("start mcuboot second time\r\n");
	res = boot_go(&rsp);
	if (res != 0) {
		EMBARC_PRINTF("\r\nvarify iamge failed \r\n");
		while (1) {
			;
		}
	} else {
		EMBARC_PRINTF("\r\nvarify image successfully \r\n");
	}

	EMBARC_PRINTF("boot the image\r\n");

	cpu_lock();
	fp = (fp_t)(rsp.br_image_off + rsp.br_hdr->ih_hdr_size + 4);
	fp();	/* jump to program */
}

void test_slot0_bad_slot1(void)
{
	uint8_t res;
	struct boot_rsp rsp;
	fp_t fp;

	board_init();
	cpu_unlock();	/* unlock cpu to let interrupt work */

	EMBARC_PRINTF("Start test_slot0_bad_slot1\r\n");
	EMBARC_PRINTF("intial the trailers\r\n");
	res = trailer_init();
	if (res) {
		EMBARC_PRINTF("intial the trailers error\r\n");
		return;
	}

	uint32_t load_address = SLOT0_STARTADDRESS;
	EMBARC_PRINTF("loading image %s into SLOT0 at 0x%x\r\n", v1_boot_file, load_address);
	load_image(v1_boot_file, load_address);

	load_address = SLOT1_STARTADDRESS;
	EMBARC_PRINTF("loading image %s into SLOT1 at 0x%x\r\n", v3_boot_file, load_address);
	load_image(v3_boot_file, load_address);

	EMBARC_PRINTF("set the magic of SLOT1\r\n");
	boot_set_pending(0);

	EMBARC_PRINTF("start mcuboot\r\n");
	res = boot_go(&rsp);
	if (res != 0) {
		EMBARC_PRINTF("\r\nvarify iamge failed \r\n");
		while (1) {
			;
		}
	} else {
		EMBARC_PRINTF("\r\nvarify image successfully \r\n");
	}

	EMBARC_PRINTF("confirm the image_ok\r\n");
	boot_set_confirmed();

	EMBARC_PRINTF("start mcuboot second time\r\n");
	res = boot_go(&rsp);
	if (res != 0) {
		EMBARC_PRINTF("\r\nvarify iamge failed \r\n");
		while (1) {
			;
		}
	} else {
		EMBARC_PRINTF("\r\nvarify image successfully \r\n");
	}

	EMBARC_PRINTF("boot the image\r\n");

	cpu_lock();
	fp = (fp_t)(rsp.br_image_off + rsp.br_hdr->ih_hdr_size + 4);
	fp();	/* jump to program */
}

void test_aborted_swap(void)
{
	/**
	//in loader.c
	//using a global varibale in loader.c to abort the swap
	static int swap_one_sector = 0;

	//replace the part in boot_copy_image()
	printf("need swap %d sectors\n", last_sector_idx);
    while (last_sector_idx >= 0) {
        sz = boot_copy_sz(last_sector_idx, &first_sector_idx);
        if (swap_idx >= bs->idx) {
            printf("swap the sector no.%d\n", bs->idx);
            boot_swap_sectors(first_sector_idx, sz, bs);
        }

        if (swap_one_sector == 0) {
            return 0;
        }

        last_sector_idx = first_sector_idx - 1;
        swap_idx++;
    }

    //add this below boot_swap_if_needed() in boot_go() 
    if (swap_one_sector == 0) {
            swap_one_sector = 1;
            goto out;
        }

	//in target.h
    //set sractch size
    #define FLASH_AREA_IMAGE_SCRATCH_SIZE	0x00001000
	*/
	uint8_t res;
	struct boot_rsp rsp;
	fp_t fp;

	board_init();
	cpu_unlock();	/* unlock cpu to let interrupt work */

	EMBARC_PRINTF("Start test_aborted_swap\r\n");
	EMBARC_PRINTF("intial the trailers\r\n");
	res = trailer_init();
	if (res) {
		EMBARC_PRINTF("intial the trailers error\r\n");
		return;
	}

	uint32_t load_address = SLOT0_STARTADDRESS;
	EMBARC_PRINTF("loading image %s into SLOT0 at 0x%x\r\n", v1_boot_file, load_address);
	load_image(v1_boot_file, load_address);

	load_address = SLOT1_STARTADDRESS;
	EMBARC_PRINTF("loading image %s into SLOT1 at 0x%x\r\n", v2_boot_file, load_address);
	load_image(v2_boot_file, load_address);

	EMBARC_PRINTF("set the magic of SLOT1\r\n");
	boot_set_pending(0);

	EMBARC_PRINTF("start mcuboot\r\n");
	res = boot_go(&rsp);

	EMBARC_PRINTF("mcuboot was aborted\r\n");
	//boot_set_confirmed();

	EMBARC_PRINTF("start mcuboot second time\r\n");
	res = boot_go(&rsp);
	if (res != 0) {
		EMBARC_PRINTF("\r\nvarify iamge failed \r\n");
		while (1) {
			;
		}
	} else {
		EMBARC_PRINTF("\r\nvarify image successfully \r\n");
	}

	EMBARC_PRINTF("boot the image\r\n");

	cpu_lock();
	fp = (fp_t)(rsp.br_image_off + rsp.br_hdr->ih_hdr_size + 4);
	fp();	/* jump to program */
}