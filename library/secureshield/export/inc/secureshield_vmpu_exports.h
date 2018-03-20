/*------------------------------------------
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
#ifndef _SECURESHIELD_VMPU_EXPORTS_H_
#define _SECURESHIELD_VMPU_EXPORTS_H_

/* basic resource access control attribute definitions */
/* access control right in user mode */
#define SECURESHIELD_AC_UEXECUTE	0x0001UL
#define SECURESHIELD_AC_UWRITE		0x0002UL
#define SECURESHIELD_AC_UREAD		0x0004UL
#define SECURESHIELD_AC_UAC		(SECURESHIELD_AC_UREAD |\
					SECURESHIELD_AC_UWRITE |\
					SECURESHIELD_AC_UEXECUTE)

/* access control right in kernel mode */
#define SECURESHIELD_AC_KEXECUTE	0x0008UL
#define SECURESHIELD_AC_KWRITE		0x0010UL
#define SECURESHIELD_AC_KREAD		0x0020UL
#define SECURESHIELD_AC_KAC		(SECURESHIELD_AC_KREAD |\
					SECURESHIELD_AC_KWRITE |\
					SECURESHIELD_AC_KEXECUTE)

#define SECURESHIELD_AC_EXECUTE		(SECURESHIELD_AC_UEXECUTE |\
					SECURESHIELD_AC_KEXECUTE)

/* all possible access control flags */
#define SECURESHIELD_AC_ACCESS		(SECURESHIELD_AC_UAC | SECURESHIELD_AC_KAC |\
					SECURESHIELD_AC_SECURE)

/* container resource definitions */
#define SECURESHIELD_AC_MEMORY		0x0100UL
#define SECURESHIELD_AC_PERIPHERAL	0x0400UL
#define SECURESHIELD_AC_INTERFACE	0x0800UL
#define SECURESHIELD_AC_IRQ		0x1000UL
#define SECURESHIELD_AC_AUX		0x2000UL

/*  extended container resource access control attribute definitions */
#define SECURESHIELD_AC_SIZE_ROUND_UP	0x10000000UL 	/* the resource size should be rounded up */
#define SECURESHIELD_AC_SIZE_ROUND_DOWN	0x20000000UL	/* the resource size should be rounded down */
#define SECURESHIELD_AC_SHARED		0x40000000UL	/* the resource is a shared resource (no implementation now) */
#define SECURESHIELD_AC_SECURE		0x80000000UL 	/* the resource is a secure resource  */
#define SECURESHIELD_AC_NORMAL		0x0UL 		/* the resource is a normal resource  */


/* the following macros are combinations based on resource type and resource attribute */
/* ROM area requires user privilege */
#define SECURESHIELD_ACDEF_UROM		(SECURESHIELD_AC_MEMORY	|\
					SECURESHIELD_AC_UREAD	|\
					SECURESHIELD_AC_KREAD	)
/* RAM area requires user privilege */
#define SECURESHIELD_ACDEF_URAM		(SECURESHIELD_AC_MEMORY	|\
					SECURESHIELD_AC_UREAD	|\
					SECURESHIELD_AC_UWRITE	|\
					SECURESHIELD_AC_KREAD	|\
					SECURESHIELD_AC_KWRITE	)

/* instruction area (.text) requires user privilege */
#define SECURESHIELD_ACDEF_UTEXT	(SECURESHIELD_ACDEF_UROM |\
					SECURESHIELD_AC_EXECUTE)
/* memory area (.data, .bss) requires user privilege */
#define SECURESHIELD_ACDEF_UDATA	SECURESHIELD_ACDEF_URAM
#define SECURESHIELD_ACDEF_UBSS		SECURESHIELD_ACDEF_URAM


/* peripheral area requires user privilege */
#define SECURESHIELD_ACDEF_UPERIPH	(SECURESHIELD_AC_PERIPHERAL |\
					SECURESHIELD_AC_UREAD	|\
					SECURESHIELD_AC_UWRITE	|\
					SECURESHIELD_AC_KREAD	|\
					SECURESHIELD_AC_KWRITE	)
/* ROM area requires kernel privilege */
#define SECURESHIELD_ACDEF_KROM		(SECURESHIELD_AC_MEMORY	|\
					SECURESHIELD_AC_KREAD	)

/* RAM area requires kernel privilege */
#define SECURESHIELD_ACDEF_KRAM		(SECURESHIELD_AC_MEMORY	|\
					SECURESHIELD_AC_KREAD	|\
					SECURESHIELD_AC_KWRITE)

/* instruction area (.text) requires kernel privilege */
#define SECURESHIELD_ACDEF_KTEXT	(SECURESHIELD_ACDEF_KROM |\
					SECURESHIELD_AC_KEXECUTE)
/* memory area (.data, .bss) requires user privilege */
#define SECURESHIELD_ACDEF_KDATA	SECURESHIELD_ACDEF_KRAM
#define SECURESHIELD_ACDEF_KBSS		SECURESHIELD_ACDEF_KRAM

/* peripheral area requires kernel privilege */
#define SECURESHIELD_ACDEF_KPERIPH	(SECURESHIELD_AC_PERIPHERAL	|\
					SECURESHIELD_AC_KREAD	|\
					SECURESHIELD_AC_KWRITE)


#define SECURESHIELD_PAD32(x)		(32 - (sizeof(x) & ~0x1FUL))
#define SECURESHIELD_CONTAINER_MAGIC	0x42CFB66FUL		/* magic number to identify container configuration */
#define SECURESHIELD_CONTAINER_SECURE	0x1 			/* container type: secure */
#define SECURESHIELD_CONTAINER_NORMAL	0x0 			/* container type: normal */

#define SECURESHIELD_MEM_SIZE_ROUND(x)	SECURESHIELD_REGION_ROUND_UP(x)


#define SECURESHIELD_REGION_ROUND_DOWN(x)	(EMBARC_POW2_CEIL(x) >> 1)
#define SECURESHIELD_REGION_ROUND_UP(x)		EMBARC_POW2_CEIL(x)
#define SECURESHIELD_STACK_SIZE_ROUND(x)	SECURESHIELD_REGION_ROUND_UP(x)

#define CONTAINER_SECURE_STACK_SIZE 	512
#define CONTAINER_DEFAUTL_STACK_SIZE	1024

typedef uint32_t CONTAINER_AC;

typedef struct
{
	void* param1;		/* address parameter or function pointer */
	uint32_t param2;	/* size or no. */
	CONTAINER_AC ac;	/* resource type, access control types */
} EMBARC_PACKED CONTAINER_AC_ITEM;

typedef struct
{
	char * container_name;
	uint32_t magic; 	/* magic number to identify this structure */
	uint32_t type;		/* container type */
	uint32_t *stack_secure;  /* secure stack to call runtime service */
	uint32_t stack_size;	/* container stack size */
	uint32_t *stack_area;	/* container stack area */

	const CONTAINER_AC_ITEM* const ac_table; /* container resource access control table */
	uint32_t ac_count; 	/* item counts in container resource access control table */

	/* memory area */
	void *text_start;	/* the start address of container's .text section */
	void *text_end;		/* the end address of container's .text section, no .text section if text_start == text_end */
	void *rodata_start;	/* the start address of container's .rodata section */
	void *rodata_end;	/* the end address of container's .rodata section, no .rodata section if rodata_start == text_end */
	void *data_start;	/* the start address of container's .data section */
	void *data_end;		/* the end address of container's .data section, no .data section if data_start == data_end */
	void *data_load_start;  /* the start load address of container's .data section */
	void *bss_start;	/* the start address of container's .bss section, container stack is in .bss section */
	void *bss_end;		/* the end address of container's .bss section */
} EMBARC_PACKED CONTAINER_CONFIG;

#endif	/*_SECURESHIELD_VMPU_EXPORTS_H_*/
