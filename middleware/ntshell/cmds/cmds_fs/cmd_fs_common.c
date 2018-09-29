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

/**
* \file
* \brief filesystem operation commands common source
*/
#include "cmd_cfg.h"
#if NTSHELL_USE_CMDS_FS

#include "cmd_fs_common.h"

EMBARC_ALIGNED(4) uint8_t cmd_fs_buffer[CMD_FS_BUF_SIZE];	/* Working Buffer */

char working_directory[256] = {"/"};

FIL cmd_files[2];

static NTSHELL_IO_PREDEF;

/** put the infromation of error*/
void fs_put_err(FRESULT rc, void *extobj)
{
	int32_t ercd = E_OK;
	FRESULT i;
	const char *str =
		"Success!\0" "Disk error\0" "INT_ERR\0" "No storge device!\0" "No such file!\0" "No such directory!\0"
		"Pathname is invalid!\0" "DENIED\0" "file or folder already exits!\0" "file/directory is invalid!\0"
		"Permission denide: write protected!\0" "Invalid drive number!\0" "NOT_ENABLED\0" "No valid FAT volume!\0"
		"MKFS_ABORTED\0" "TIMEOUT\0" "LOCKED\0" "NOT_ENOUGH_CORE\0" "Open too many files!\0";

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	for (i = 0; i != rc && *str; i++) {
		while (*str++) ;
	}

	CMD_DEBUG("%s Error Code = %u!\r\n", str, (uint32_t)rc);

	return ;
error_exit:
	return ;
}
/** get filesystem working directory*/
char *fs_working_dir(void)
{
	f_getcwd(working_directory, sizeof working_directory);

	return working_directory;
}

/**check path*/
void check_path(char *pathname)
{
	int32_t len, i;
	char *ptr = NULL;

	ptr = pathname;

	/*change "\"" to "/" */
	while (*ptr) {
		if (*ptr == '\\') {
			*ptr = '/';
		}

		ptr ++;
	}

	/*delete the "/" from the end*/
	ptr = pathname;

	len = strlen(pathname);

	i = len - 1;

	/* check whether the path is "./" */
	if ((i == 1) && (ptr[0] == '.') && (ptr[1] == '/')) {
		return;
	}

	while (i) {
		if (ptr[i] == '/') {
			ptr[i] = '\0';
		} else {
			break;
		}

		i--;
	}
}

/** Get new path by concatenating old filename with new directory name */
char *get_new_filepath(char *new_dir, char *old_name)
{
	if (!(new_dir && old_name)) {
		return NULL;
	}

	int len, len_new, i;
	char *ptr;
	/* get the file name*/
	len = strlen(old_name);
	len_new = strlen(new_dir);
	i = len;

	while (i) {
		i--;

		if (old_name[i] == '/') {
			i++;
			break;
		}
	}

	ptr = malloc((len+len_new-i+3));

	if (ptr == NULL) {
		return ptr;
	}

	strcpy(ptr, new_dir);

	/* if new name is a folder path without '/', then add '/' to it */
	if (ptr[len_new-1] != '/') {
		ptr[len_new] = '/';
		ptr[len_new+1] = '\0';
	}

	strcat(ptr, &old_name[i]);
	return ptr;
}

#endif /* NTSHELL_USE_CMDS_FS */