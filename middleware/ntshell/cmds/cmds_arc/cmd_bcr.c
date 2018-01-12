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
* \brief dumps the Build Configuration Registers for the ARC Family processor
*/

#include "cmds_arc_cfg.h"
#if NTSHELL_USE_CMDS_ARC_BCR

#include <stdio.h>
#include <string.h>

#include "embARC.h"

#include "ntshell_common.h"
#include "embARC_debug.h"

#ifndef USE_NTSHELL_EXTOBJ /* don't use ntshell extobj */
#define CMD_DEBUG(fmt, ...)			EMBARC_PRINTF(fmt, ##__VA_ARGS__)
#endif

static NTSHELL_IO_PREDEF;

#ifdef __HIGHC__
#include <arc/arc_reg.h>        /* from MetaWare toolchain */
#else
/* aux register offsets */
#define IDENTITY             0x0004
#define AUX_DCCM             0x0018
#define BCR_VER              0x0060
#define CRC_BUILD_BCR        0x0062
#define BTA_LINK_BUILD       0x0063
#define EA_BUILD             0x0065
#define MEMSUBSYS            0x0067
#define VECBASE_AC_BUILD     0x0068
#define P_BASE_ADDR          0x0069
#define RF_BUILD             0x006e
#define D_CACHE_BUILD        0x0072
#define DCCM_BUILD           0x0074
#define TIMER_BUILD          0x0075
#define I_CACHE_BUILD        0x0077
#define ICCM_BUILD           0x0078
#define DSPRAM_BUILD         0x0079
#define MAC_BUILD            0x007a
#define MULTIPLY_BUILD       0x007b
#define SWAP_BUILD           0x007c
#define NORM_BUILD           0x007d
#define BARREL_BUILD         0x007f
#define ISA_CONFIG           0x00c1
#define IRQ_BUILD            0x00f3
#define IFETCH_QUEUE_BUILD   0x00fe
#endif


/*
-BUILD_ARC600
-BUILD_ARC700
-BUILD_BARRELSHIFTER
-BUILD_SWAP
-BUILD_NORM
-BUILD_MULTIPLY
BUILD_MULT32D
BUILD_MULT32
-BUILD_MULT32x16
-BUILD_EA
-BUILD_XMAC_D16
-BUILD_XMAC_24
-BUILD_XMAC
-BUILD_XY
-BUILD_XYBANKS
-BUILD_XYSIZE
-BUILD_XYLSBASEX
-BUILD_XYLSBASEY
BUILD_MPU
-BUILD_MMU

-BUILD_DCACHE_PRESENT
-BUILD_DCACHESIZE
-BUILD_DCACHELINESIZE
-BUILD_DCACHEWAYS
-BUILD_ICACHE_PRESENT
-BUILD_ICACHESIZE
-BUILD_ICACHELINESIZE
-BUILD_ICACHEWAYS
*/

/*
* Determine which CPU we are building for using the _ARCVER symbol.
*/
#ifdef _ARCVER
#if (_ARCVER >= 0x21 && _ARCVER < 0x25)
#define BUILD_ARC600 1
/* TODO: arc600 core5 is 0x25 and arc600 core6 is 26 which
collide with arc601. If arc601 becomes antiquated, repair this.
*/
#elif (_ARCVER >= 0x25 && _ARCVER <= 0x26)
#define BUILD_ARC601 1
#elif (_ARCVER >= 0x31 && _ARCVER <= 0x34)
#define BUILD_ARC700 1
#elif (_ARCVER >= 0x40 && _ARCVER <= 0x4F)
#define BUILD_ARCV2EM 1
#elif (_ARCVER >= 0x50 && _ARCVER <= 0x5F)
#define BUILD_ARCV2HS 1
#else
#error "Please update this program with latest _ARCVER numbers"
#endif
#else
#error "This program depends on the _ARCVER symbol"
#endif

#if defined(BUILD_ARCV2HS) || defined(BUILD_ARCV2EM)
#define AUX_STACK_REGION_BUILD (0x0C5)
#endif


#ifdef __Xbarrel_shifter
#define BUILD_BARRELSHIFTER 1
#endif

#ifdef __Xswap
#define BUILD_SWAP 1
#endif

#ifdef __Xnorm
#define BUILD_NORM 1
#endif

#ifdef __Xmult32d
#define BUILD_MULT32D 1
#endif

#ifdef __Xmult32
#define BUILD_MULT32 1
#endif

#ifdef __Xmul32x16
#define BUILD_MULT32x16 1
#endif

#ifdef __Xmpy
#define BUILD_MULTIPLY 1        /* arc700 & arcv2em only */
#endif

#ifdef __Xea
#define BUILD_EA 1
#endif

#ifdef __Xxmac_d16
#define BUILD_XMAC_D16 1
#endif

#ifdef __Xxmac_24
#define BUILD_XMAC_24 1
#endif

#if !defined(BUILD_XMAC_D16) && !defined(BUILD_XMAC_24)
#ifdef __Xxmac
#define BUILD_XMAC 1
#endif
#endif

#ifdef __Xxy
#define BUILD_XY 1
#define BUILD_XYBANKS 1         /* TODO: not known @ compile time */
#define BUILD_XYSIZE 1
#define BUILD_XYLSBASEX 1       /* TODO: not known @ compile time */
#define BUILD_XYLSBASEY 1
#endif


#ifdef MPU_LBA_REG_LOW_16       /* from CF component */
#define BUILD_MPU 1
#endif

/*
#define BUILD_MMU 1
note: BUILD_MMU not supported yet.
*/

#define BUILD_DCACHE_PRESENT 1 /* assume it is */
/*
#define BUILD_DCACHESIZE ???
#define BUILD_DCACHELINESIZE ????
#define BUILD_DCACHEWAYS ???
*/

#define BUILD_ICACHE_PRESENT 1 /* assume it is */
/*
#define BUILD_ICACHESIZE ???
#define BUILD_ICACHELINESIZE  ???
#define BUILD_ICACHEWAYS ???
*/




#ifdef BUILD_ARC700             /* 700 always has swap, norm and barrel_shifter */
#define HAVE_SWAP          1
#define HAVE_NORM          1
#define HAVE_BARRELSHIFTER 1
#else
#ifdef BUILD_SWAP
#define HAVE_SWAP          1
#endif
#ifdef BUILD_NORM
#define HAVE_NORM          1
#endif
#ifdef BUILD_BARRELSHIFTER
#define HAVE_BARRELSHIFTER 1
#endif
#endif


#ifndef REG_XYLSBASEX
#define REG_XYLSBASEX            0x9D
#endif
#ifndef REG_XYLSBASEY
#define REG_XYLSBASEY            0x9E
#endif

/* VLC_BUILD                 0xFC */
/* SIMD_DMA_BUILD            0xFD */
/* SMART_BUILD               0xFF */

/* test result values */
#define TR_GOOD    0   /* success */
#define TR_FAIL    1   /* failure */
#define TR_WARN    2   /* warning ... may be harmless but check just in case */
#define TR_SKIP    3   /* indicates this test is skipped due to configuration */

/* dependency test codes */
#define TEST_CPUTYPE    0
#define TEST_PROCNUM    1
#define TEST_CRC        2
#define TEST_EA         3
#define TEST_INTERRUPTS 4
#define TEST_VBASE      5
#define TEST_PBASE      6
#define TEST_MMU        7
#define TEST_DCACHE     8
#define TEST_DCACHE_LINESZ 9
#define TEST_TIMER      10
#define TEST_ICACHE     11
#define TEST_ICCM       12
#define TEST_XY         13
#define TEST_XMAC       14
#define TEST_MULT       15
#define TEST_SWAP       16
#define TEST_NORM       17
#define TEST_BARREL     18
#define TEST_RFBUILD    19
#define TEST_FIRQ       20
#define TEST_NUM_PRIO   21
#define TEST_STACKCK    22
#define TEST_ISA_OPTIONS 23
#define TEST_MAX        24

/* globals */

int test_results[TEST_MAX];

int core;

#define TRESULT(test, code) test_results[(test)] = (code);

void init_test_array (void)
{
	int32_t i;
	for (i = 0; i < TEST_MAX; ++i)
	test_results[i] = TR_SKIP;  /* assumed skip initially */
}


void test_result_report (void)
{
	int32_t i;
	char str[40];
	char res[12];
	int32_t fail = 0;
	CMD_DEBUG ("\r\n");
	CMD_DEBUG ("Test Results:\r\n");
	CMD_DEBUG ("\r\n");
	for (i = 0; i < TEST_MAX; ++i){
		if (test_results[i] == TR_SKIP)
			continue;
		switch (i){
			case TEST_CPUTYPE:
				xsprintf (str, "cpu type");
				break;
			case TEST_PROCNUM:
				xsprintf (str, "processor number");
				break;
			case TEST_CRC:
				xsprintf (str, "crc");
				break;
			case TEST_EA:
				xsprintf (str, "ea");
				break;
			case TEST_INTERRUPTS:
				xsprintf (str, "interrupts");
				break;
			case TEST_VBASE:
				xsprintf (str, "vector base");
				break;
			case TEST_PBASE:
				xsprintf (str, "peripheral base");
				break;
			case TEST_MMU:
				xsprintf (str, "mmu");
				break;
			case TEST_DCACHE:
				xsprintf (str, "d-cache");
				break;
			case TEST_DCACHE_LINESZ:
				xsprintf (str, "d-cache line size");
				break;
			case TEST_TIMER:
				xsprintf (str, "timer");
				break;
			case TEST_ICACHE:
				xsprintf (str, "i-cache");
				break;
			case TEST_ICCM:
				xsprintf (str, "iccm");
				break;
			case TEST_XY:
				xsprintf (str, "xy");
				break;
			case TEST_XMAC:
				xsprintf (str, "xmac");
				break;
			case TEST_MULT:
				xsprintf (str, "multiply");
				break;
			case TEST_SWAP:
				xsprintf (str, "swap");
				break;
			case TEST_NORM:
				xsprintf (str, "norm");
				break;
			case TEST_BARREL:
				xsprintf (str, "barrel shifter");
				break;
			case TEST_RFBUILD:
				xsprintf (str, "RF build");
				break;
			case TEST_FIRQ:
				xsprintf (str, "firq_option");
				break;
			case TEST_NUM_PRIO:
				xsprintf (str, "priorities");
				break;
			case TEST_STACKCK:
				xsprintf (str, "stack checking");
				break;
			case TEST_ISA_OPTIONS:
				xsprintf (str, "isa options");
				break;
			// default:
			//     xsprintf (str, "*unknown test*");
			//     break;
		}
		switch (test_results[i]){
			case TR_GOOD:
				xsprintf (res, "GOOD");
				break;
			case TR_FAIL:
				xsprintf (res, "!!!FAIL!!!");
				fail = 1;
				break;
			case TR_WARN:
				xsprintf (res, "WARNING!");
				break;
			default:
				xsprintf (res, "**UH OH***");
				break;
		}
		CMD_DEBUG ("%-20s : %-12s\r\n", str, res);
	}
	CMD_DEBUG("***********************************************************\r\n");
	CMD_DEBUG("Overall Result: %s\r\n", (fail)?"FAILED":"PASSED");
	CMD_DEBUG("***********************************************************\r\n");
}


void dump_IDENTITY (void)
{
	int32_t a600 = 0;
	int32_t a700 = 0;
	uint32_t val;
	uint32_t t;
	char str[128];
	/****************************************/
	/** IDENTITY                           **/
	/****************************************/
	core = 0; /* assume 0 if don't know */
	TRESULT (TEST_CPUTYPE, TR_GOOD);
	val = _arc_aux_read (IDENTITY);
	CMD_DEBUG ("IDENTITY (Processor Identification Register)\r\n");
	CMD_DEBUG ("--------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", IDENTITY, val);
	t = val & 0xff;
	if ((int32_t) t >= 0 && t <= 0xf){
		xsprintf (str, "ARCtangent-A4 processor family");
	}else if (t >= 0x10 && t <= 0x1f){
		xsprintf (str, "Reserved for ARCtangent-A5 processor family");
	}
	else if (t == 0x20){
		xsprintf (str, "Reserved for ARC600 processor family");
		a600 = 1;
	}else if (t == 0x21) {
		xsprintf (str, "ARC600 processor family, base case version 1");
		a600 = 1;
	}else if (t == 0x22){
		xsprintf (str, "ARC600 processor family, base case version 2");
		a600 = 1;
	}else if (t == 0x25){
		xsprintf (str, "ARC601");
		a600 = 2;
	}else if (t >= 0x23 && t <= 0x2f){
		xsprintf (str, "ARC600 processor family");
		a600 = 1;
	}else if (t == 0x30){
		xsprintf (str, "Reserved for ARC700 processor family");
		a700 = 1;
	}else if (t == 0x31){
		xsprintf (str, "ARC700 processor family, base case version 1");
		a700 = 1;
	}else if (t == 0x32){
		xsprintf (str,
		"ARC 700 processor family,\r\n    base-case version 2, supports additional BCR region");
		a700 = 1;
	}else if (t >= 0x33 && t <= 0x3f){
		xsprintf (str, "Reserved for ARC700 processor family");
		a700 = 1;
	}else if (t == 0x40) {
		xsprintf (str, "ARCV2EM core 0");
	}else if (t >= 0x41 && t <= 0x4f) {
		xsprintf (str, "ARCV2EM core %d",t&0xf);
		core=1;
	}else if (t >= 0x50 && t <= 0x5f) {
		xsprintf (str, "ARCV2HS core %d",t&0xf);
	}else{
		xsprintf (str, "Reserved");
	}

#if defined(BUILD_ARC600) || defined(BUILD_ARC601)               /* 600 or 601-only */
	if (a600 == 0){
#ifdef BUILD_ARC600
		CMD_DEBUG("*** ERROR: built for arc600, but this CPU is not an arc600.\r\n");
#else
		CMD_DEBUG("*** ERROR: built for arc601, but this CPU is not an arc601.\r\n");
#endif
		TRESULT (TEST_CPUTYPE, TR_FAIL);
	}
#else
	if (a600 == 1){
		CMD_DEBUG("*** ERROR: CPU is a arc600, but not passing -arc600 to compiler.\r\n");
		TRESULT (TEST_CPUTYPE, TR_FAIL);
	}else if (a600 == 2){
		CMD_DEBUG("*** ERROR: CPU is a arc601, but not passing -arc601 to compiler.\r\n");
		TRESULT (TEST_CPUTYPE, TR_FAIL);
	}
#endif
#ifdef BUILD_ARC700
	if (a700 == 0){
		CMD_DEBUG("*** ERROR: built for arc700, but this CPU is not an arc700.\r\n");
		TRESULT (TEST_CPUTYPE, TR_FAIL);
	}
#else
	if (a700 == 1){
		CMD_DEBUG("*** ERROR: CPU is a arc700, but not passing -arc700 to compiler.\r\n");
		TRESULT (TEST_CPUTYPE, TR_FAIL);
	}
#endif

	TRESULT (TEST_PROCNUM, TR_GOOD);
	CMD_DEBUG (" ARCVER 0x%x = %s\r\n", t, str);
#ifdef __HIGHC__
	CMD_DEBUG ("note: _ARCVER = 0x%0x\r\n", _ARCVER);
#else
	CMD_DEBUG ("note: _ARCVER = unknown\r\n"); /* TODO: How with GNU tools? */
#endif
	t = (val >> 8) & 0xff;
	CMD_DEBUG (" ARCNUM 0x%x = Unique processor number\r\n", t);
	t = (val >> 16);
	CMD_DEBUG (" CHIPID 0x%x = Unique chip number\r\n", t);
	CMD_DEBUG ("\r\n");
}

void dump_BCR_VER (void)
{
	uint32_t val;
	/****************************************/
	/** BCR_VER                            **/
	/****************************************/
	val = _arc_aux_read (BCR_VER) & 0x00ff;
	CMD_DEBUG ("BCR_VER (Build Configuration Register Version)\r\n");
	CMD_DEBUG ("----------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x ", BCR_VER, val);
	if (val == 0){
		CMD_DEBUG ("Reserved\r\n");
	}else if (val == 1){
		CMD_DEBUG ("BCR Region at 0x60-0x7f only\r\n");
	}else if (val == 2){
		CMD_DEBUG ("BCR Region at 0x60-0x7f and 0xC0-0xFF\r\n");
	}else {
		CMD_DEBUG ("** ERROR: Unknown\r\n");
	}
	CMD_DEBUG ("\r\n");
}


void dump_DCCM_BASE_BUILD (void)
{
	uint32_t val;
	/****************************************/
	/** DCCM_BASE_BUILD                    **/
	/****************************************/
#ifdef BUILD_ARC700
	val = _arc_aux_read (DCCM_BASE_BUILD);
#else
	{
		uint32_t ver;
		val = _arc_aux_read (DCCM_BUILD);
		ver = val & 0xff;
		if (ver != 0){
			val = _arc_aux_read (AUX_DCCM); /* only read this if DCCM present */
		}
	}
#endif
	if (val != 0){
#ifdef BUILD_ARC700
		CMD_DEBUG ("DCCM_BASE_BUILD (DCCM Configuration Register)\r\n");
		CMD_DEBUG ("---------------------------------------------\r\n");
		CMD_DEBUG ("0x%x : 0x%x\r\n", DCCM_BASE_BUILD, val);
#else
		CMD_DEBUG ("AUX_DCCM (DCCM Base Address Register)\r\n");
		CMD_DEBUG ("---------------------------------------------\r\n");
		CMD_DEBUG ("0x%x : 0x%x\r\n", AUX_DCCM, val);
#endif
		CMD_DEBUG ("\r\n");
	}
}

void dump_CRC_BUILD_BCR (void)
{
	uint32_t val;
	/****************************************/
	/** CRC_BUILD_BCR                       **/
	/****************************************/
	TRESULT (TEST_CRC, TR_GOOD);
	val = _arc_aux_read (CRC_BUILD_BCR) & 0xff;
	CMD_DEBUG ("CRC_BUILD_BCR (CRC Instruction Configuration Register)\r\n");
	CMD_DEBUG ("------------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x ", CRC_BUILD_BCR, val);
	CMD_DEBUG (" Version %d\r\n", val);
#ifdef __Xcrc
	if (val != 1){
		CMD_DEBUG ("ERROR: -Xcrc is specified yet version is not 1.\r\n");
		TRESULT (TEST_CRC, TR_FAIL);
	}
#else
	if (val == 1){
		CMD_DEBUG("WARNING: -Xcrc is NOT passed to compiler, yet CRC instruction is present.\r\n");
		TRESULT (TEST_CRC, TR_WARN);
	}
#endif
	CMD_DEBUG ("\r\n");
}


void dump_BTA_LINK_BUILD (void)
{
#if !defined(BUILD_ARC600) && !defined(BUILD_ARC601)
	uint32_t val;
	/****************************************/
	/** BTA_LINK_BUILD                     **/
	/****************************************/
	val = _arc_aux_read (BTA_LINK_BUILD) & 0x01;
	CMD_DEBUG ("BTA_LINK_BUILD (BTA Configuration Register)\r\n");
	CMD_DEBUG ("-------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x ", BTA_LINK_BUILD, val);
	if (val == 0){
		CMD_DEBUG ("BTA registers are absent\r\n");
	}else if (val == 1){
		CMD_DEBUG ("BTA registers are present\r\n");
	}else{
		CMD_DEBUG ("** ERROR: Unknown\r\n");
	}
	CMD_DEBUG ("\r\n");
#endif
}


void dump_EA_BUILD (void)
{
	uint32_t val;
	/****************************************/
	/** EA_BUILD                           **/
	/****************************************/
	TRESULT (TEST_EA, TR_GOOD);
	val = _arc_aux_read (EA_BUILD) & 0xff;
	CMD_DEBUG ("EA_BUILD (Extended Arithmetic Configuration Register)\r\n");
	CMD_DEBUG ("-----------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x ", EA_BUILD, val);
	if (val == 0){
		CMD_DEBUG ("Reserved\r\n");
	}else if (val == 1){
		CMD_DEBUG ("Reserved\r\n");
	}else if (val == 2){
		CMD_DEBUG ("Current Version\r\n");
	}else{
		CMD_DEBUG ("** ERROR: Unknown\r\n");
	}
#ifdef BUILD_EA
	if (val != 2){
		CMD_DEBUG ("*** ERROR: EA is not present yet compiler thinks it is.\r\n");
		TRESULT (TEST_EA, TR_FAIL);
	}
#else
	if (val == 2){
		CMD_DEBUG("*** WARNING: -Xea not passed to compiler but EA is present.\r\n");
		TRESULT (TEST_EA, TR_WARN);
	}
#endif
	CMD_DEBUG ("\r\n");
}

void dump_MEMSUBSYS (void)
{
	uint32_t val;
	/****************************************/
	/** MEMSUBSYS                          **/
	/****************************************/
	val = _arc_aux_read (MEMSUBSYS);
	CMD_DEBUG ("MEMSUBSYS (Memory Subsystem Configuration Register)\r\n");
	CMD_DEBUG ("---------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", MEMSUBSYS, val);
	CMD_DEBUG ("  EM = %s\r\n", (val & 1) ? "external memory system enabled" : "no off chip RAM interface present");
	CMD_DEBUG ("  BE = %s\r\n", (val & 4) ? "big endian" : "little endian");
	CMD_DEBUG ("\r\n");
}

void dump_VECBASE (void)
{
	uint32_t val;
	uint32_t t;
	char str[80];
	int consider_e_bits=1;
	/****************************************/
	/** VECBASE_AC_BUILD                   **/
	/****************************************/
#if defined(BUILD_ARCV2EM)
	if (core==1)
	consider_e_bits=0; /* core1 uses IRQ_BUILD for number_of_interrupts */
#elif defined(BUILD_ARCV2HS)
	consider_e_bits=0;
#endif
	TRESULT (TEST_INTERRUPTS, TR_GOOD);
	val = _arc_aux_read (VECBASE_AC_BUILD);
	CMD_DEBUG ("VECBASE_AC_BUILD (Interrupt Vector Base Address Configuration)\r\n");
	CMD_DEBUG ("--------------------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", VECBASE_AC_BUILD, val);
	t = (val) & 3;
	if (consider_e_bits) {
		switch (t){
			case 0:
				xsprintf (str, "16 interrupts");
				break;
			case 1:
				xsprintf (str, "32 interrupts");
				break;
			case 2:
				xsprintf (str, "8 interrupts");
				break;
			case 3:
				xsprintf (str, "3 interrupts");
				break;
		}
	}else{
		xsprintf (str, "0x%x reserved", t);

		CMD_DEBUG (" E[1:0] %s\r\n", str);

		t = (val >> 2) & 0x0ff;
		switch (t) {
			case 0:
				xsprintf (str, "ARC 600 Interrupt Unit");
				break;
			case 1:
				xsprintf (str, "ARC 700 Interrupt Unit");
				break;
			case 3:
			case 4:
				xsprintf (str, "ARCompact V2 Interrupt Unit");
				break;
			default:
				xsprintf (str, "Unknown Interrupt Unit");
				break;
		}
	}
	CMD_DEBUG (" Version 0x%x = %s\r\n", t, str);
#if defined(BUILD_ARC600) || defined(BUILD_ARC601)               /* 600 or 601-only */
	if (t == 1){
		CMD_DEBUG ("*** ERROR: Interrupt unit is suppose to be for ARC600\r\n");
		TRESULT (TEST_INTERRUPTS, TR_FAIL);
	}
#elif defined(BUILD_ARCV2EM)||defined(BUILD_ARCV2HS)
	if ((t != 3) && (t != 4)){
		CMD_DEBUG("*** ERROR: ARCV2 is suppose to have version 3 or 4 interrupt unit.\r\n");
		TRESULT (TEST_INTERRUPTS, TR_FAIL);
	}
#else
	if (t != 1){
		CMD_DEBUG("*** ERROR: Interrupt unit is for ARC600 but this program doesn't think this is an ARC600.\r\n");
		TRESULT (TEST_INTERRUPTS, TR_FAIL);
	}
#endif
#ifdef BUILD_ARC700
	if (t != 1){
		CMD_DEBUG ("*** ERROR: Interrupt unit is suppose to be for ARC700\r\n");
		TRESULT (TEST_INTERRUPTS, TR_FAIL);
	}
else
	/*#if !defined(BUILD_ARCV2EM) && !defined(BUILD_ARCV2HS)*/
	if (t == 1){
		CMD_DEBUG("*** ERROR: Interrupt unit is for ARC700 but compiler's cpu is different.\r\n");
		TRESULT (TEST_INTERRUPTS, TR_FAIL);
	}
	/*#endif*/
#endif
	t = val & ~0x3ff;
	CMD_DEBUG (" ADDR[31:10] 0x%x = Interrupt Vector Base Address\r\n", t);
	TRESULT (TEST_VBASE, TR_GOOD);
	CMD_DEBUG ("\r\n");
}


void dump_PBASE (void)
{
	uint32_t val;
	/***************************************/
	/** PERIPHERAL BASE                   **/
	/***************************************/
	val = _arc_aux_read (P_BASE_ADDR);
	if (val != 0){ /* may not be present -- optional */
		TRESULT (TEST_PBASE, TR_GOOD);
		CMD_DEBUG ("P_BASE_ADDR (Peripheral Base Register)\r\n");
		CMD_DEBUG ("--------------------------------------\r\n");
		CMD_DEBUG ("0x%x : 0x%x\r\n", P_BASE_ADDR, val);
		CMD_DEBUG ("  Version %d\r\n", val & 0xf);
		val = val & 0xffffff00;   /* bits "31:8" */
		CMD_DEBUG ("  Peripheral base = 0x%x\r\n", val);
		CMD_DEBUG ("\r\n");
	}
}

void dump_RF_BUILD (void)
{
#if !defined(BUILD_ARC700)
	/* arcv2em, arcv2hs, 600 or 601-only */
	uint32_t val;
	uint32_t t;
	/****************************************/
	/** RF_BUILD                           **/
	/****************************************/
	val = _arc_aux_read (RF_BUILD);
	CMD_DEBUG ("RF_BUILD (Core Register Set Configuration Register)\r\n");
	CMD_DEBUG ("---------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", RF_BUILD, val);
	t = val & 0xff;
	CMD_DEBUG (" Version 0x%x = Version of Core Register Set\r\n", t);
	t = (val >> 8) & 1;
	CMD_DEBUG (" P 0x%x = Number of Ports\r\n", t);
	CMD_DEBUG ("        = %d port register file\r\n", t + 3);
	t = (val >> 9) & 1;
	CMD_DEBUG (" E 0x%x = Number of Entries\r\n", t);
	CMD_DEBUG ("        = %d entry register file\r\n", (t == 0) ? 32 : 16);
	/* TODO: can validate for RF16 */
	t = (val >> 10) & 1;
	CMD_DEBUG (" R 0x%x = Reset state\r\n", t);
	if (t == 0) {
		CMD_DEBUG ("        = Not cleared on reset\r\n");
	}else{
		CMD_DEBUG ("        = Cleared on reset\r\n");
	}
#if defined(BUILD_ARCV2EM) || defined(BUILD_ARCV2HS)
#if defined(BUILD_ARCV2EM)
	if (core==1)
#endif
	{
		t = (val >> 11) & 7;
		CMD_DEBUG (" B 0x%x = Number of additional banks\r\n", t);
		TRESULT (TEST_RFBUILD, TR_GOOD);
		if (t!=0) { /* skip D field if B is zero */
			t = (val >> 14) & 3;
			switch (t) {
				case 0:
				CMD_DEBUG (" D 0x%x - 4 duplicate registers\r\n", t);
				break;
				case 1:
				CMD_DEBUG (" D 0x%x - 8 duplicate registers\r\n", t);
				break;
				case 2:
				CMD_DEBUG (" D 0x%x - 16 duplicate registers\r\n", t);
				break;
				case 3:
				CMD_DEBUG (" D 0x%x - 32 duplicate registers\r\n", t);
				break;
			}
		}
	/* TODO: can validate B and D against compile time assumptions */
	}
#endif
	CMD_DEBUG ("\r\n");
#endif
}

void dump_MMU_BUILD (void)
{
/****************************************/
/** MMU_BUILD                          **/
/****************************************/
#ifdef BUILD_ARC700
	uint32_t val;
	uint32_t t;
	val = _arc_aux_read (MMU_BUILD);
	TRESULT (TEST_MMU, TR_GOOD);
	CMD_DEBUG ("MMU_BUILD (MMU Configuration Register)\r\n");
	CMD_DEBUG ("--------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", MMU_BUILD, val);
#ifdef BUILD_MMU
	t = val & 0xff;
	CMD_DEBUG ("DTLB = %d Number of uDTLB entries for the ARC 700 processor.\r\n",t);
	if (t != 8){
		CMD_DEBUG ("*** ERROR: DTLB != 8.\r\n");
	}
	t = (val >> 8) & 0xff;
	CMD_DEBUG ("ITLB = %d Number of uITLB entries for ARC 700.\r\n", t);
	if (t != 4){
		CMD_DEBUG ("*** ERROR: iTLB != 4.\r\n");
	}
	t = (val >> 16) & 0xf;
	CMD_DEBUG ("JE = %d Joint TLB contains 2JE entries, per way.\r\n", t);
	if (t != 7){
		CMD_DEBUG ("*** ERROR: JE != 7.\r\n");
	}
	t = (val >> 20) & 0xf;
	CMD_DEBUG ("JA = %d Joint TLB contains 2JA ways.\r\n", t);
	if (t != 1){
		CMD_DEBUG ("*** ERROR: JA != 1.\r\n");
	}
	t = (val >> 24) & 0xff;
	CMD_DEBUG("Version = %d First MMU release for the ARC 700 processor has version number 0x1.\r\n",t);
	if (t != 1 && t != 2){
		CMD_DEBUG ("*** ERROR: MMU Version != 1 or 2.\r\n");
	}
#else
	t = (val >> 24) & 0xff;
	if (t == 1 || t == 2){
		CMD_DEBUG("*** NOTE: MMU not supported by this software.\r\n    MMU_BUILD indicates version %d is present.\r\n",t);
		TRESULT (TEST_MMU, TR_GOOD);
	}
#endif
	CMD_DEBUG ("\r\n");
#endif
}

void dump_D_CACHE_BUILD (void)
{
	uint32_t val;
	uint32_t t;
#ifdef BUILD_DCACHE_PRESENT
	char str[80];
#endif
	/****************************************/
	/** D_CACHE_BUILD                      **/
	/****************************************/
	TRESULT (TEST_DCACHE, TR_GOOD);
	val = _arc_aux_read (D_CACHE_BUILD);
	CMD_DEBUG ("D_CACHE_BUILD (Data Cache Build Configuration Register)\r\n");
	CMD_DEBUG ("-------------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", D_CACHE_BUILD, val);
	t = val & 0xff;               /* ver_num */
	CMD_DEBUG ("  Version number = %d\r\n", t);

#ifdef BUILD_DCACHE_PRESENT
	TRESULT (TEST_DCACHE_LINESZ, TR_GOOD);        /* assume good until find otherwise */

	if (t == 0){
		; /* d-cache not present */
	}else{
		t = (val >> 8) & 0xf;     /* config_t */
		CMD_DEBUG ("  Configuration Type = %d\r\n", t);
		{
			int ways = 0;
			switch (t){
				case 0:
					xsprintf (str, "One Way Set-Associative 2 Stage Pipeline, Direct Mapped");
					ways = 1;
					break;
				case 1:
					xsprintf (str, "Two Way Set-Associative 2 Stage Pipeline");
					ways = 2;
					break;
				case 2:
					xsprintf (str, "Four Way Set-Associative 2 Stage Pipeline");
					ways = 4;
					break;
				case 3:
					xsprintf (str, "One Way Set-Associative 3 Stage Pipeline, Direct Mapped");
					ways = 1;
					break;
				case 4:
					xsprintf (str, "Two Way Set-Associative 3 Stage Pipeline");
					ways = 2;
					break;
				case 5:
					xsprintf (str, "Four Way Set-Associative 3 Stage Pipeline");
					ways = 4;
					break;
				default:
					xsprintf (str, "*Reserved");
					break;
			}
		CMD_DEBUG ("   %s\r\n", str);
#if 0
		if (ways != BUILD_DCACHEWAYS)
		CMD_DEBUG
		("*** ERROR: D-cache ways is %d but compiler thinks it is %d.\r\n",
		ways, BUILD_DCACHEWAYS);
#endif
		}
		t = (val >> 12) & 0xf;    /* ram_sz */
		{
			int rmsz = 0;
			switch (t){
				case 0:
					rmsz = 512;
					break;
				case 1:
					rmsz = 1024;
					break;
				case 2:
					rmsz = 2048;
					break;
				case 3:
					rmsz = 4 * 1024;
					break;
				case 4:
					rmsz = 8 * 1024;
					break;
				case 5:
					rmsz = 16 * 1024;
					break;
				case 6:
					rmsz = 32 * 1024;
					break;
				case 7:
					rmsz = 64 * 1024;
					break;
				default:
					break;
			}
			CMD_DEBUG ("  Cache RAM Size = %d, %d bytes\r\n", t, rmsz);
#if 0                           /* TODO: not yet */
		if (rmsz != BUILD_DCACHESIZE)
		CMD_DEBUG
		("*** ERROR: D-cache size is %d but value was assumed to be %d.\r\n",
		rmsz, BUILD_DCACHESIZE);
#endif
		}
		t = (val >> 16) & 0xf;    /* line_sz */
		CMD_DEBUG ("  Line Length = %d, ", t);
		{
			int ls = 0;
			switch (t){
				case 0:
					ls = 16;
					break;
				case 1:
					ls = 32;
					break;
				case 2:
					ls = 64;
					break;
				case 3:
					ls = 128;
					break;
				case 4:
					ls = 256;
					break;
				default:
					break;
		}
		CMD_DEBUG ("%d bytes\r\n", ls);
		}
	}
#else
	if (t != 0){
		CMD_DEBUG("WARNING: D-Cache is present but this code was compiled to assume no D-cache.\r\n");
		TRESULT (TEST_DCACHE, TR_WARN);
	}
#endif /* BUILD_DCACHE_PRESENT */
	CMD_DEBUG ("\r\n");
}


void dump_DCCM_BUILD (void)
{
	uint32_t val;
	uint32_t ver;
	uint32_t sz;
	/***********************************/
	/** DCCM_BUILD                    **/
	/***********************************/
	val = _arc_aux_read (DCCM_BUILD);
	ver = val & 0xff;
	if (ver != 0){
		CMD_DEBUG ("DCCM_BUILD (DCCM RAM Configuration Register)\r\n");
		CMD_DEBUG ("--------------------------------------------\r\n");
		CMD_DEBUG ("0x%x : 0x%x\r\n", DCCM_BUILD, val);
		CMD_DEBUG ("  Version %d\r\n", ver);
		if (ver>=2) {
			sz = (val >> 8) & 0xf; /* 4-bits with ver 2 */
			switch (sz){
				case 1:                /* 512 */
					sz = 512;
					break;
				case 2:                /* 1k */
					sz = 1024;;
					break;
				case 3:                /* 2k */
					sz = 2*1024;
					break;
				case 4:                /* 4k */
					sz = 4*1024;
					break;
				case 5:                /* 8k */
					sz = 8*1024;
					break;
				case 6:                /* 16k */
					sz = 16*1024;
					break;
				case 7:                /* 32k */
					sz = 32*1024;
					break;
				case 8:                /* 64k */
					sz = 64*1024;
					break;
				case 9:                /* 128k */
					sz = 128*1024;
					break;
				case 10:               /* 256k */
					sz = 256*1024;
					break;
				default:
					CMD_DEBUG("ERROR: unknown size for DCCM\r\n");
					return;
			}
		} else {
			sz = (val >> 8) & 7;
			switch (sz){
				case 0:                /* 2k */
					sz = 2;
					break;
				case 1:                /* 4k */
					sz = 4;
					break;
				case 2:                /* 8k */
					sz = 8;
					break;
				case 3:                /* 16k */
					sz = 16;
					break;
				case 4:                /* 32k */
					sz = 32;
					break;
				case 5:                /* 64k */
					sz = 64;
					break;
				case 6:                /* 128k */
					sz = 128;
					break;
				case 7:                /* 256k */
					sz = 256;
					break;
			}
			sz *= 1024;
		}
		CMD_DEBUG ("  Size %d bytes\r\n", sz);
		CMD_DEBUG ("\r\n");
	}
}


void dump_TIMER_BUILD (void)
{
	uint32_t val;
	uint32_t t;
	char str[80];
	/****************************************/
	/** TIMER_BUILD                        **/
	/****************************************/
	TRESULT (TEST_TIMER, TR_GOOD);
	val = _arc_aux_read (TIMER_BUILD);
	CMD_DEBUG ("TIMER_BUILD (Processor Timer Configuration Register)\r\n");
	CMD_DEBUG ("----------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", TIMER_BUILD, val);
	t = val & 0xff;
	switch (t){
		case 0:
			xsprintf (str, "*register missing*");
			break;
		case 1:
			xsprintf (str, "Version 1");
			break;
		case 2:
			xsprintf (str, "ARCtangent-A5 and ARC700 Processor Timers");
			break;
		case 3:
			xsprintf (str, "ARC600 R3 Processor Timers, with interrupt pending bits");
			break;
		case 4:
			xsprintf (str, "ARCompact V2 Processor Timers");
			break;
		default:
			xsprintf (str, "ERROR: Unknown version");
			break;
	}
#if defined(BUILD_ARC600) || defined(BUILD_ARC601)               /* 600 or 601-only */
	if (t != 3){
#ifdef BUILD_ARC600
		CMD_DEBUG ("*** ERROR: This is an arc600 w/o arc600 timers.\r\n");
#else
		CMD_DEBUG ("*** ERROR: This is an arc601 w/o arc600 timers.\r\n");
#endif
		TRESULT (TEST_TIMER, TR_FAIL);
	}
#else
	/*if (t==3) {   skip. See bug 94992
	CMD_DEBUG("*** ERROR: ARC600 timers yet this is not an ARC600.\r\n");
	}*/
#endif
#ifdef BUILD_ARC700
	if (t != 2){
		if (t == 3)
		;                       /* Tom's ISS hardware shop is more advanced than ARC IP. bug 94992 */
		else{
			CMD_DEBUG ("*** ERROR: This is an arc700 w/o arc700 timers.\r\n");
			TRESULT (TEST_TIMER, TR_FAIL);
		}
	}
#else
	if (t == 2){
		CMD_DEBUG ("*** ERROR: ARC700 timers yet this is not an ARC700.\r\n");
		TRESULT (TEST_TIMER, TR_FAIL);
	}
#endif
	CMD_DEBUG (" Version 0x%x = %s\r\n", t, str);
	t = (val >> 8) & 1;
	if (t == 0){
		CMD_DEBUG (" T0 0x0 = no timer 0\r\n");
	}else
		CMD_DEBUG (" T0 0x1 = timer 0 present\r\n");

	t = (val >> 9) & 1;
	if (t == 0){
		CMD_DEBUG (" T1 0x0 = no timer 1\r\n");
	}else{
		CMD_DEBUG (" T1 0x1 = timer 1 present\r\n");
	}
#if defined(BUILD_ARCV2EM)||defined(BUILD_ARCV2HS)
#if defined(BUILD_ARCV2EM)
	if (core==1)
#endif
	{
		t = (val >> 10) & 1;
		CMD_DEBUG (" RTC_OPTION = %d\r\n", t);
		/* TODO: v1.1 - can check against compile-time assumption */
	}
#endif
	CMD_DEBUG ("\r\n");
}

void dump_I_CACHE_BUILD (void)
{
	/****************************************/
	/** I_CACHE_BUILD                      **/
	/****************************************/
	uint32_t val;
	uint32_t version;
	uint32_t t;
#ifdef BUILD_ICACHE_PRESENT
	char str[80];
#endif
	TRESULT (TEST_ICACHE, TR_GOOD);
	val = _arc_aux_read (I_CACHE_BUILD);
	CMD_DEBUG ("I_CACHE_BUILD (Instruction Cache Build Configuration Register)\r\n");
	CMD_DEBUG ("--------------------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", I_CACHE_BUILD, val);
	version = val & 0xff;               /* ver_num */
	CMD_DEBUG ("  Version number = %d\r\n", version);
	#ifdef BUILD_ICACHE_PRESENT
	if (version == 0){
		; /* no i-cache */
	}else{
		switch (version){
			case 1:
				xsprintf(str,"reserved");
				break;
			case 2:
				xsprintf(str,"ARCompact V1, fixed 32-byte line size");
				break;
			case 3:
				xsprintf(str,"ARCompact V1, variable line size");
				break;
			case 4:
				xsprintf(str,"ARCv2");
				break;
			default:
				xsprintf(str,"(reserved)");
				break;
		}
		CMD_DEBUG ("   %s\r\n", str);
		t = (val >> 8) & 0xf;     /* config_t */
		CMD_DEBUG ("  Configuration Type = %d\r\n", t);
		{
			int ways = 0;
			switch (t){
				case 0:
					xsprintf (str, "One Way Set Associative, Direct Mapped");
					ways = 1;
					break;
				case 1:
					if (version==4) {
					xsprintf (str, "Two Way Set Associative");
					ways = 2;
					} else {
					xsprintf (str, "One Way Set Associative, with Code RAM");
					ways = 1;
					}
					break;
				case 2:
					if (version==4) {
					xsprintf (str, "Four Way Set Associative");
					ways = 4;
					} else {
					xsprintf (str, "reserved");
					}
					break;
				case 3:
					if (version==4) {
					xsprintf (str, "Eight Way Set Associative");
					ways = 8;
					} else {
					xsprintf (str, "Two Way Set Associative");
					ways = 2;
					}
					break;
				case 4:
					if (version==4) {
					xsprintf (str, "reserved");
					} else {
					xsprintf (str, "Four Way Set Associative");
					ways = 4;
					}
					break;
				default:
					xsprintf (str, "*Reserved");
				break;
			}
		CMD_DEBUG ("   %s\r\n", str);
#if 0                           /* TODO: not yet */
		if (ways != BUILD_ICACHEWAYS)
		CMD_DEBUG
		("*** ERROR: I-cache ways is %d but compiler thinks it is %d.\r\n",
		ways, BUILD_ICACHEWAYS);
#endif
		}
		t = (val >> 12) & 0xf;    /* ram_sz */
		{
			int rmsz = 0;
			switch (t){
				case 0:
					rmsz = 512;
					break;
				case 1:
					rmsz = 1024;
					break;
				case 2:
					rmsz = 2048;
					break;
				case 3:
					rmsz = 4 * 1024;
					break;
				case 4:
					rmsz = 8 * 1024;
					break;
				case 5:
					rmsz = 16 * 1024;
					break;
				case 6:
					rmsz = 32 * 1024;
					break;
				case 7:
					rmsz = 64 * 1024;
					break;
				default:
					break;
			}
			CMD_DEBUG ("  Cache RAM Size = %d, %d bytes\r\n", t, rmsz);
#if 0                           /* TODO: not yet */
			if (rmsz != BUILD_ICACHESIZE)
			CMD_DEBUG
			("*** ERROR: I-cache size is %d but compiler thinks it is %d.\r\n",
			rmsz, BUILD_ICACHESIZE);
#endif
		}
		t = (val >> 16) & 0xf;    /* line_sz */
		CMD_DEBUG ("  Line Length = %d, ", t);
		{
			int ls = 0;
			switch (t){
				case 0:
					ls = 8;
					break;
				case 1:
					ls = 16;
					break;
				case 2:
					ls = 32;
					break;
				case 3:
					ls = 64;
					break;
				case 4:
					ls = 128;
					break;
				case 5:
					ls = 256;
					break;
				default:
					break;
			}
			CMD_DEBUG ("%d bytes\r\n", ls);
#if 0                           /* get the icache lines size @ runtime */
			if (ls != BUILD_ICACHELINESIZE)
			CMD_DEBUG
			("*** ERROR: I-cache line size is %d but compiler thinks it is %d.\r\n",
			ls, BUILD_ICACHELINESIZE);
#endif
		}
		t = (val >> 20) & 0x3;    /* FL */
		CMD_DEBUG ("  Feature Level = %d, ", t);
		{
			switch (t){
				case 0:
					xsprintf(str,"Basic cache, no lock, no debug");
					break;
				case 1:
					xsprintf(str,"Lock and flush features");
					break;
				case 2:
					xsprintf(str,"Lock, flush and debug features");
					break;
				case 3:
				default:
					xsprintf(str,"(reserved)");
					break;
			}
			CMD_DEBUG ("%s\r\n", str);
		}
	}
#else /* icache not assumed by this code */
	if (t != 0){
		CMD_DEBUG("WARNING: This code assumes no i-cache, and yet there is an i-cache.\r\n");
		TRESULT (TEST_ICACHE, TR_WARN);
	}
#endif /* BUILD_ICACHE_PRESENT */
	CMD_DEBUG ("\r\n");
}

void dump_ICCM_BUILD (void)
{
	uint32_t val;
	uint32_t ver;
	uint32_t sz;
	/****************************************/
	/** ICCM_BUILD                         **/
	/****************************************/
	TRESULT (TEST_ICCM, TR_GOOD);
	val = _arc_aux_read (ICCM_BUILD);
	ver = val & 0xff;
	sz = (val >> 8) & 0xf; /* 4-bits now */
	CMD_DEBUG ("ICCM_BUILD (ICCM Configuration Register)\r\n");
	CMD_DEBUG ("----------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", ICCM_BUILD, val);
	CMD_DEBUG ("  Version %d\r\n", ver);
#if defined(BUILD_ARCV2EM)
	if (core==1 && sz==0){
		sz = (val >> 12) & 0xf;
	}
	/* if size0 is missing, use size 1. */
#endif
#if defined(BUILD_ARCV2HS)
	if (sz==0){
		sz = (val >> 12) & 0xf;
	}
	/* if size0 is missing, use size 1. */
#endif

	if (sz != 0){
	/* if  defined(BUILD_ARC601), should be version 2 */
#if defined(BUILD_ARC700)
		switch (sz){
			case 1:                /* 8kb */
				sz = 8*1024;
				break;
			case 2:                /* 16k */
				sz = 16*1024;
				break;
			case 3:                /* 32k */
				sz = 32*1024;
				break;
			case 4:                /* 64k */
				sz = 64*1024;
				break;
			case 5:                /* 128k */
				sz = 128*1024;
				break;
			case 6:                /* 256k */
				sz = 256*1024;
				break;
			case 7:                /* 512k */
				sz = 512*1024;
				break;
			default:
				CMD_DEBUG("ERROR: unknown size bit pattern\r\n");
				CMD_DEBUG ("\r\n");
				/* error */
				return;
		}
#else
		if (ver>=2) {
			switch (sz){
				case 1:                /* 512 */
					sz = 512;
					break;
				case 2:                /* 1k */
					sz = 1024;
					break;
				case 3:                /* 2k */
					sz = 2*1024;
					break;
				case 4:                /* 4k */
					sz = 4*1024;
					break;
				case 5:                /* 8k */
					sz = 8*1024;
					break;
				case 6:                /* 16k */
					sz = 16*1024;
					break;
				case 7:                /* 32k */
					sz = 32*1024;
					break;
				case 8:                /* 64k */
					sz = 64*1024;
					break;
				case 9:
					sz = 128*1024;       /* 128k */
					break;
				case 10:
					sz = 256*1024;       /* 256k */
					break;
				case 11:
					sz = 512*1024;       /* 512k */
					break;
				case 12:
					sz = 1024*1024;       /* 1 MB */
					break;
				default:
					CMD_DEBUG("ERROR: unknown size bit pattern\r\n");
					CMD_DEBUG ("\r\n");
					/* error */
					return;
			}
		} else { /* version < 2, assume version 1 */
			switch (sz){
				case 1:                /* 1k */
					sz = 1024;
					break;
				case 2:                /* 2k */
					sz = 2*1024;
					break;
				case 3:                /* 4k */
					sz = 4*1024;
					break;
				case 4:                /* 8k */
					sz = 8*1024;
					break;
				case 5:                /* 16k */
					sz = 16*1024;
					break;
				case 6:                /* 32k */
					sz = 32*1024;
					break;
				case 7:                /* 64k */
					sz = 64*1024;
					break;
				case 8:
					sz = 128*1024;       /* 128k */
					break;
				case 9:
					sz = 256*1024;       /* 256k */
					break;
				case 10:
					sz = 512*1024;       /* 512k */
					break;
				default:
					CMD_DEBUG("ERROR: unknown size bit pattern\r\n");
					CMD_DEBUG ("\r\n");
					/* error */
					return;
			}
		}
	#endif
		CMD_DEBUG ("  Size %d bytes\r\n", sz);
	}
	CMD_DEBUG ("\r\n");
}


void dump_DSPRAM_BUILD (void)
{
	/****************************************/
	/** DSPRAM_BUILD                       **/
	/****************************************/
	uint32_t val;
	uint32_t t;
#ifdef BUILD_XY
	char str[80];
#endif
	TRESULT (TEST_XY, TR_GOOD);
	val = _arc_aux_read (DSPRAM_BUILD);
	CMD_DEBUG ("DSPRAM_BUILD (XY Memory Build Configuration Register)\r\n");
	CMD_DEBUG ("-----------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", DSPRAM_BUILD, val);
	t = val & 0xff;
#ifdef BUILD_XY
	if (t == 0){
		CMD_DEBUG("ERROR: Compiled with the -Xxy switch, and yet there is no XY memory.\r\n");
		TRESULT (TEST_XY, TR_FAIL);
	}else{
		switch (t){
		case 1:
		case 3:
			xsprintf (str, "* Reserved *");
			break;
		case 2:
			xsprintf (str, "version 2");
			break;
		case 4:
			xsprintf (str, "Dual port RAM configuration only supported");
			break;
		case 5:
			xsprintf (str, "Dual and single port RAM configuration supported");
			break;
		default:
			xsprintf (str, "* unknown *");
			break;
			/*TODO: AS211 shows version 3 */
		}
		CMD_DEBUG ("  Version = %d, %s\r\n", t, str);
		t = (val >> 8) & 0xf;
		{
			int sz = 0;
			switch (t){
				case 0:
					xsprintf (str, "* Reserved *");
					break;
				case 1:
					xsprintf (str, "1k");
					sz = 1024;
					break;
				case 2:
					xsprintf (str, "2k");
					sz = 2 * 1024;
					break;
				case 3:
					xsprintf (str, "4k");
					sz = 4 * 1024;
					break;
				case 4:
					xsprintf (str, "8k");
					sz = 8 * 1024;
					break;
				case 5:
					xsprintf (str, "16k");
					sz = 16 * 1024;
					break;
				case 6:
					xsprintf (str, "32k");
					sz = 32 * 1024;
					break;
				default:
					xsprintf (str, "* Reserved *");
					break;
			}
#if 0                           /* TODO: not yet */
			if (sz != BUILD_XYSIZE)
			CMD_DEBUG ("*** ERROR: XYSIZE = %d, yet register shows %d\r\n",
			BUILD_XYSIZE, sz);
#endif
		}
		CMD_DEBUG ("  Bank RAM Size (both X & Y) = %d, %s\r\n", t, str);
		t = (val >> 12) & 0xf;
		if (t == 1){
			xsprintf (str, "1 Bank");
		}else if (t == 2){
			xsprintf (str, "2 Bank");
		}else{
			xsprintf (str, "* Reserved *");
		}
		CMD_DEBUG ("  Number of Banks = %d, %s\r\n", t, str);
#if 0                           /* TODO: not yet */
		if (t != BUILD_XYBANKS)
		CMD_DEBUG ("*** ERROR: XYBANKS = %d, yet register shows %d\r\n",
		BUILD_XYBANKS, t);
#endif
		t = (val >> 16) & 0x3;
		switch (t){
			case 0:
				xsprintf (str, "Dual Port RAM architecture");
				break;
			case 1:
				xsprintf (str, "Two Port RAM architecture");
				break;
			case 2:
				xsprintf (str, "Single port RAM architecture");
				break;
			default:
				xsprintf (str, "* Reserved *");
				break;
		}
		CMD_DEBUG ("  Ram organization type = %d, %s\r\n", t, str);

	/* verify x/y base address registers */
#if 0                           /* TODO : not yet */
		val = _arc_aux_read (REG_XYLSBASEX);
		if (val != BUILD_XYLSBASEX)
		CMD_DEBUG
		("*** WARNING: compiler XYLSBASEX is 0x%x but register reads 0x%x\r\n",
		BUILD_XYLSBASEX, val);
		val = _arc_aux_read (REG_XYLSBASEY);
		if (val != BUILD_XYLSBASEY)
		CMD_DEBUG
		("*** WARNING: compiler XYLSBASEY is 0x%x but register reads 0x%x\r\n",
		BUILD_XYLSBASEY, val);
#endif
	}
#else
	/* TODO: I'm not certain that zero means the XY is missing */
	if (t != 0){
		CMD_DEBUG("WARNING: Compiled without the -Xxy switch, and yet there is an XY memory.\r\n");
		TRESULT (TEST_XY, TR_WARN);
	}
#endif /* BUILD_XY */
	CMD_DEBUG ("\r\n");
}


void dump_MAC_BUILD (void)
{
	uint32_t val;
	uint32_t t;
	char str[80];
	/****************************************/
	/** MAC_BUILD                          **/
	/****************************************/
	val = _arc_aux_read (MAC_BUILD);
	if ((val & 0xff) != 0)
	{
		TRESULT (TEST_XMAC, TR_GOOD);
		CMD_DEBUG ("MAC_BUILD (MAC Configuration Register)\r\n");
		CMD_DEBUG ("--------------------------------------\r\n");
		CMD_DEBUG ("0x%x : 0x%x\r\n", MAC_BUILD, val);
		t = val & 0xff;
		switch (t){
			case 1:
				xsprintf (str, "MUL/MAC");
				break;
			case 2:
				xsprintf (str, "MAC");
				break;
			case 3:
				xsprintf (str, "MAC V3");
				break;
			case 4:
				xsprintf (str, "MAC V3.1");
				break;
			case 5:
				xsprintf (str, "MAC for DSP v3.1 Pack A");
				break;
			/* TODO: what are the new mac types? */
			/* AS221 has version 6, type 0 */
			case 6:
				xsprintf (str, "MAC V6");
				break;
			default:
				xsprintf (str, " ");
				break;
		}
		CMD_DEBUG ("Version = %d, %s\r\n", t, str);
		t = (val >> 8) & 0xff;
		if (t == 0){
			xsprintf (str, "16x16 only");
		}else{
			str[0] = 0;
		}
		if (t & 1){
			strcat (str, "[24x24]");
		}
		if (t & 2){
			strcat (str, "[Dual 16x16]");
		}
		if (t & 4){
			strcat (str, "[32x16]");
		}
		if (t & 8){
			strcat (str, "[Dual 32x16]");
		}

		CMD_DEBUG ("MAC Architecture 0x%x %s\r\n", t, str);
		/* TODO: missing the new ver 6, architecture 0 info */
		{
			int m = 0;
#ifdef BUILD_XMAC_24
			m |= 1;
#endif
#ifdef BUILD_XMAC_D16
			m |= 2;
#endif
#ifdef BUILD_MULT32x16
			m |= 4;
#endif
#ifdef BUILD_XMAC
			m |= 6;                 /* ARC700 with xmac always gets "Dual 16x16 and 32x16" I think. */
#endif
#ifdef __Xdmulpf
			m |= 8;
#endif
			if (t != m){
				int j;
				str[0] = 0;
				if (t & 1){
					strcat (str, "[24x24]");
				}
				if (t & 2){
					strcat (str, "[Dual 16x16]");
				}
				if (t & 4){
					strcat (str, "[32x16]");
				}
				if (t & 8){
					strcat (str, "[Dual 32x16]");
				}

				for (j=1;j<=8;j=j<<1) {
					uint32_t mbit = m & j;
					uint32_t tbit = t & j;
					if (tbit && !mbit) {
						TRESULT (TEST_XMAC, TR_WARN);
						switch (tbit) {
							case 1:
								CMD_DEBUG("    WARNING: You can compile with -Xxmac_24 to exploit 24x24 XMAC hardware.\r\n");
								break;
							case 2:
								CMD_DEBUG("    WARNING: You can compile with -Xxmac_d16 to exploit Dual 16x16 XMAC hardware.\r\n");
								break;
							case 4:
								CMD_DEBUG("    WARNING: You can compile with -Xmul32x16 to exploit 32x16 multiply.\r\n");
								break;
							case 8:
								CMD_DEBUG("    WARNING: You can compile with -Xdmulpf to exploit dual 32x16 multiply.\r\n");
								break;
							default:
								CMD_DEBUG("    ERROR: Unknown bits for xmac. t=0x%x,m=0x%x\r\n", t,m);
							break;
						}

					}
#if defined(BUILD_XMAC_24) || defined(BUILD_XMAC_D16) || defined(BUILD_MULT32x16) || defined(BUILD_XMAC) || defined(__Xdmulpf)
					else if (mbit && !tbit) {
						TRESULT (TEST_XMAC, TR_FAIL);
						switch (tbit) {
							case 1:
								CMD_DEBUG("    ERROR: Compiling with -Xxmac_24, yet 24x24 XMAC hardware is not present.\r\n");
								break;
							case 2:
								CMD_DEBUG("    ERROR: Compiling with -Xxmac_d16, yet Dual 16x16 XMAC hardware is not present.\r\n");
								break;
							case 4:
								CMD_DEBUG("    ERROR: Compiling with -Xmul32x16, yet 32x16 multiply is not present.\r\n");
								break;
							case 8:
								CMD_DEBUG("    ERROR: Compiling with -Xdmulpf, yet dual 32x16 multiply is not present.\r\n");
								break;
							default:
								CMD_DEBUG("    ERROR: Unknown bits for xmac. t=0x%x,m=0x%x\r\n", t,m);
								break;
						}
					}
#endif
				}
			}
		}
		CMD_DEBUG ("\r\n");
	}
}

void dump_MULTIPLY_BUILD (void)
{
	#if !defined(BUILD_ARC600) && !defined(BUILD_ARC601)
	/* check multiply only on arc700, arcv2em or arcv2hs */
	uint32_t val;
	uint32_t t;
	int dsp_field=0;
	char str[80];
	/****************************************/
	/** MULTIPLY_BUILD                     **/
	/****************************************/
	val = _arc_aux_read (MULTIPLY_BUILD);
	CMD_DEBUG ("MULTIPLY_BUILD (Multiply Configuration Register)\r\n");
	CMD_DEBUG ("------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", MULTIPLY_BUILD, val);
	t = val & 0xff;
	switch (t){
		case 0:
			xsprintf (str, "*register missing*");
		break;
		case 1:
			xsprintf (str, "Multiply 32x32 with special result register");
		break;
		case 2:
			xsprintf (str, "Multiply 32x32 with any result register");
		break;
		case 6:
			xsprintf (str, "ARCompact V2 32x32 Muliply");
		break;
		default:
			xsprintf (str, "ERROR: Unknown version");
		break;
	}
	CMD_DEBUG (" Version 0x%x = %s\r\n", t, str);
#ifdef BUILD_MULTIPLY
	if (t != 1 && t != 2 && t != 6){
		CMD_DEBUG("*** ERROR: -Xmpy is given to compiler but type = %d.\r\n", t);
		TRESULT (TEST_MULT, TR_FAIL);
		/* TODO: -hs : needs to be 6 for av2 */
	}else{
		TRESULT (TEST_MULT, TR_GOOD);
	}
#else
	if (t == 1 || t == 2 || t == 6){
		CMD_DEBUG("*** WARNING: -Xmpy is not passed to compiler but multiply is present.\r\n");
		TRESULT (TEST_MULT, TR_WARN);
	}else{
		TRESULT (TEST_MULT, TR_GOOD);
	}
#endif
#if defined(BUILD_ARCV2EM)||defined(BUILD_ARCV2HS)
	t = (val >> 8) & 3;
	if (t==0){
		CMD_DEBUG(" TYPE[1:0]=0, Serial multiplier, computing 4-bits per cycle\r\n");
	}else if (t==1){
		CMD_DEBUG(" TYPE[1:0]=1, Parallel multiplier, with latency in CYC field\r\n");
	}else if (t==2) {
		CMD_DEBUG(" TYPE[1:0]=2, Parallel multiplier, with DSP capability defined by the DSP field\r\n");
		dsp_field=1;
	}else{
		CMD_DEBUG(" TYPE[1:0]=%d, *unknown* multiplier\r\n", t);
	}
	t = (val >> 10) & 3;
	CMD_DEBUG(" CYC[2:0] = %d, %d cycle(s)\r\n", t, t+1);
	if (dsp_field) {
		t = (val >> 12) & 0xf;
		switch (t) {
		case 0: xsprintf(str,"No DSP capability included"); break;
		case 1: xsprintf(str,"DSP MPY_OPTION 7"); break;
		case 2: xsprintf(str,"DSP MPY_OPTION 8"); break;
		case 3: xsprintf(str,"DSP MPY_OPTION 9"); break;
		default: xsprintf(str,"reserved"); break;
		}
		CMD_DEBUG(" DSP[3:0] = %d %s\r\n", t,str);
	}
	t = (val >> 16) & 0xff;
	if (t==0){
		CMD_DEBUG(" Version16x16 = 0x0, ARCompact V2 16x16 Multiply NOT present\r\n");
	}else if (t==2){
		CMD_DEBUG(" Version16x16 = 0x2, ARCompact V2 16x16 Multiply present\r\n");
	}else{
		CMD_DEBUG(" Version16x16 = %d, *uknown type*\r\n", t);
	}
#endif
	CMD_DEBUG ("\r\n");
#endif /* BUILD_ARC700 or BUILD_ARV2EM or BUILD_ARCV2HS */
}

void dump_SWAP_BUILD (void)
{
	uint32_t val;
	uint32_t t;
	char str[80];
	/****************************************/
	/** SWAP_BUILD                         **/
	/****************************************/
	val = _arc_aux_read (SWAP_BUILD);
	CMD_DEBUG ("SWAP_BUILD (Swap Configuration Register)\r\n");
	CMD_DEBUG ("----------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", SWAP_BUILD, val);
	t = val & 0xff;
	switch (t){
		case 0:
			xsprintf (str, "*register missing*");
			break;
		case 1:
		case 2:
		case 3: /* arcv2em has this version */
			xsprintf (str, "Current version");
			break;
		default:
			xsprintf (str, "ERROR: Unknown version");
			TRESULT (TEST_SWAP, TR_FAIL);
			break;
	}
	CMD_DEBUG (" Version 0x%x = %s\r\n", t, str);
#ifdef HAVE_SWAP
	if (t == 0){
		CMD_DEBUG("*** ERROR: SWAP assumed by compiler but SWAP is not present.\r\n");
		TRESULT (TEST_SWAP, TR_FAIL);
	}else{
		TRESULT (TEST_SWAP, TR_GOOD);
	}
#else
	if (t >= 1){
		CMD_DEBUG("*** WARNING: -Xswap not passed to compiler but SWAP is present.\r\n");
		TRESULT (TEST_SWAP, TR_WARN);
	}else{
		TRESULT (TEST_SWAP, TR_GOOD);
	}
#endif
	CMD_DEBUG ("\r\n");
}

void dump_NORM_BUILD (void)
{
	uint32_t val;
	uint32_t t;
	char str[80];
	/****************************************/
	/** NORM_BUILD                         **/
	/****************************************/
	val = _arc_aux_read (NORM_BUILD);
	CMD_DEBUG ("NORM_BUILD (Normalize Configuration Register)\r\n");
	CMD_DEBUG ("---------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", NORM_BUILD, val);
	t = val & 0xff;
	switch (t){
		case 0:
			xsprintf (str, "*register missing*");
			break;
		case 1:
			xsprintf (str, "Reserved");
			break;
		case 2:
		case 3:
			xsprintf (str, "Current version");
			break;
		default:
			xsprintf (str, "ERROR: Unknown version");
			break;
	}
	CMD_DEBUG (" Version 0x%x = %s\r\n", t, str);
#ifdef HAVE_NORM
	if (t < 2){
		CMD_DEBUG("*** ERROR: NORM assumed by compiler but NORM is not present.\r\n");
		TRESULT (TEST_NORM, TR_FAIL);
	}else{
		TRESULT (TEST_NORM, TR_GOOD);
	}
#else
	if (t >= 2){
		CMD_DEBUG("*** WARNING: -Xnorm is not passed to compiler but NORM is present.\r\n");
		TRESULT (TEST_NORM, TR_WARN);
	}else{
		TRESULT (TEST_NORM, TR_GOOD);
	}
#endif
	CMD_DEBUG ("\r\n");
}

#if defined(BUILD_ARCV2EM)||defined(BUILD_ARCV2HS)
void dump_IRQ_BUILD (void)
{
	uint32_t val;
	uint32_t t;
	uint32_t i;
	uint32_t e;
	/****************************************/
	/** IRQ_BUILD                          **/
	/****************************************/
	val = _arc_aux_read (IRQ_BUILD);
	CMD_DEBUG ("IRQ_BUILD (Interrupt Build Configuration Register)\r\n");
	CMD_DEBUG ("--------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", IRQ_BUILD, val);
	t = val & 0xff; /* version */
	CMD_DEBUG (" Version=0x%x\r\n", t);
	t = (val>>8) & 0xff; /* IRQS */
	i = t;
	CMD_DEBUG (" IRQS=%d\r\n", t);
	t = (val>>16) & 0xff; /* EXTS */
	e = t;
	CMD_DEBUG (" EXTS=%d\r\n", t);

	TRESULT (TEST_INTERRUPTS, TR_GOOD);
	t = (val>>24) & 0xf; /* L */
	++t;
	CMD_DEBUG (" Priorities=%d\r\n", t);
	TRESULT (TEST_NUM_PRIO, TR_GOOD);
	t = (val>>28) & 0x1; /* F */
	CMD_DEBUG (" FIRQ_OPTION=%d\r\n", t);
	TRESULT (TEST_FIRQ, TR_GOOD);
	t = (val>>29) & 0x1; /* V */
	CMD_DEBUG (" <reserved for future use>=%d\r\n", t);
	CMD_DEBUG ("\r\n");

}
#endif

void dump_BARREL_BUILD (void)
{
	uint32_t val;
	uint32_t t;
	char str[80];
	/****************************************/
	/** BARREL_BUILD                       **/
	/****************************************/
	val = _arc_aux_read (BARREL_BUILD);
	CMD_DEBUG ("BARREL_BUILD (Barrel Shifter Configuration Register)\r\n");
	CMD_DEBUG ("----------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", BARREL_BUILD, val);
	t = val & 0xff;
	switch (t){
		case 0:
			xsprintf (str, "*register missing*");
			break;
		case 1:
			xsprintf (str, "Reserved");
			break;
		case 2:
			xsprintf (str, "ARCompact V1 version");
			break;
		case 3:
			xsprintf (str, "ARCv2 version");
			break;
		default:
			xsprintf (str, "ERROR: Unknown version");
			break;
	}
	CMD_DEBUG (" Version 0x%x = %s\r\n", t, str);
	if (t==3) { /* if AV2 version */
		t = (val>>8) & 0x3;
		switch (t){
			case 0:
				xsprintf (str, "Single-bit shifts only");
				break;
			case 1:
				xsprintf (str, "Single-bit and shift-assist instructions");
				break;
			case 2:
				xsprintf (str, "Single-bit and barrel-shift instructions");
				break;
			case 3:
				xsprintf (str, "All shift instructions");
				break;
		}
		CMD_DEBUG (" SHIFT_OPTION 0x%x = %s\r\n", t, str);
	}
#ifdef HAVE_BARRELSHIFTER
	if (t < 2){
		CMD_DEBUG("*** ERROR: BARRELSHIFTER assumed by compiler but BARRELSHIFTER is not present.\r\n");
		TRESULT (TEST_BARREL, TR_FAIL);
	}else{
		TRESULT (TEST_BARREL, TR_GOOD);
	}
#else
	if (t >= 2){
		CMD_DEBUG("*** WARNING: -Xbarrel_shifter is not passed to compiler but BARRELSHIFTER is present.\r\n");
		TRESULT (TEST_BARREL, TR_WARN);
	}else{
		TRESULT (TEST_BARREL, TR_GOOD);
	}
#endif
	CMD_DEBUG ("\r\n");
}


void dump_PC_WIDTHS (void)
{
#if defined(BUILD_ARCV2EM)||defined(BUILD_ARCV2HS)
	uint32_t val;
	uint32_t pc_size;
	uint32_t lpc_size;
	uint32_t adr_size;
	uint32_t pc_width=0;
	char str[80];
	/*******************************/
	/** ISA_CONFIG                **/
	/*******************************/
	val = _arc_aux_read (ISA_CONFIG);
	CMD_DEBUG ("ISA_CONFIG\r\n");
	CMD_DEBUG ("----------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", ISA_CONFIG, val);
	CMD_DEBUG (" Version %d\r\n", val & 0xff);
	pc_size = (val >> 8) & 0xf;
	switch (pc_size){
		case 0:
			xsprintf (str, "16 bit");
			pc_width=16;
			break;
		case 1:
			xsprintf (str, "20 bit");
			pc_width=20;
			break;
		case 2:
			xsprintf (str, "24 bit");
			pc_width=24;
			break;
		case 3:
			xsprintf (str, "28 bit");
			pc_width=28;
			break;
		case 4:
			xsprintf (str, "32 bit");
			pc_width=32;
			break;
		default:
			xsprintf (str, "*unknown*");
			break;
	}
#if 0 /* not working!!! */
	if (pc_width == _LPC_WIDTH) {
	TRESULT (TEST_ISA_OPTIONS, TR_GOOD);
	} else {
	TRESULT (TEST_ISA_OPTIONS, TR_FAIL);
	CMD_DEBUG
	("*** ERROR: _LPC_WIDTH=%d, but hardware is different.\r\n",
	_LPC_WIDTH);
	}
#endif

	CMD_DEBUG (" pc_size = %s\r\n", str);
	lpc_size = (val >> 12) & 0xf;
	switch (lpc_size){
		case 0:
			xsprintf (str, "zero overhead loop not supported");
			break;
		case 1:
			xsprintf (str, "8 bit");
			break;
		case 2:
			xsprintf (str, "12 bit");
			break;
		case 3:
			xsprintf (str, "16 bit");
			break;
		case 4:
			xsprintf (str, "20 bit");
			break;
		case 5:
			xsprintf (str, "24 bit");
			break;
		case 6:
			xsprintf (str, "28 bit");
			break;
		case 7:
			xsprintf (str, "32 bit");
			break;
		default:
			xsprintf (str, "*unknown*");
			break;
	}
	CMD_DEBUG (" lpc_size = %s\r\n", str);
	adr_size = (val >> 16) & 0xf;
	switch (adr_size){
		case 0:
			xsprintf (str, "16 bit");
			break;
		case 1:
			xsprintf (str, "20 bit");
			break;
		case 2:
			xsprintf (str, "24 bit");
			break;
		case 3:
			xsprintf (str, "28 bit");
			break;
		case 4:
			xsprintf (str, "32 bit");
			break;
		default:
			xsprintf (str, "*unknown*");
			break;
	}
	CMD_DEBUG (" adr_size = %s\r\n", str);
	CMD_DEBUG (" byte_order = %s\r\n",((val>>20)&1)?"big endian":"little endian");
	/* TODO: v1.1 - check endian */
	CMD_DEBUG (" atomic_option = %s\r\n",((val>>21)&1)?"LLOC and SCOND present":"false");
#if defined(BUILD_ARCV2HS)
	CMD_DEBUG (" non_aligned = %s\r\n",((val>>22)&1)?"non-aligned are allowed":"non-aligned throw exception");
#endif
	CMD_DEBUG (" code_density = %d\r\n", ((val>>24)&0xf));
	/* TODO: could check -Xcd here */
	CMD_DEBUG (" div_rem_option = %d\r\n", ((val>>28)&0xf));
	/* TODO: v1.1 - check to see if compiler is given __Xdiv_rem */
	CMD_DEBUG ("\r\n");
#endif
}


#if defined(BUILD_ARC700)||defined(BUILD_ARCV2EM)||defined(BUILD_ARCV2HS)
#ifdef AUX_STACK_REGION_BUILD
void dump_STACK_CHECKING(void)
{
	uint32_t val;

	/****************************************/
	/** AUX_STACK_REGION_BUILD             **/
	/****************************************/
	val = _arc_aux_read (AUX_STACK_REGION_BUILD);
	CMD_DEBUG ("AUX_STACK_REGION_BUILD\r\n");
	CMD_DEBUG ("----------------------------------------------------\r\n");
	CMD_DEBUG ("0x%x : 0x%x\r\n", AUX_STACK_REGION_BUILD, val);
	val &= 0xff;
	if (val==0){
		CMD_DEBUG ("  stack checking feature not present\r\n");
	}else if (val==1){
		CMD_DEBUG ("  ARC700 style stack checking\r\n");
	}else if (val==2){
		CMD_DEBUG ("  ARCV2.1 style stack checking\r\n");
	}else{
		CMD_DEBUG ("  version %d style stack checking\r\n", val);
	}
#if HW_STACK_CHECKING
	TRESULT (TEST_STACKCK, TR_GOOD);
#if defined(BUILD_ARCV2EM)||defined(BUILD_ARCV2HS)
	if (val==0) {
		TRESULT (TEST_STACKCK, TR_FAIL);
		CMD_DEBUG("*** ERROR: HW_STACK_CHECKING is true, yet hardware doesn't support it.\r\n");
	} else if (val != 2) {
		TRESULT (TEST_STACKCK, TR_WARN);
		CMD_DEBUG("*** WARNING: stack checking version is incorrect.\r\n");
	}
#endif
#if defined(BUILD_ARC700)
	if (val==0) {
		TRESULT (TEST_STACKCK, TR_FAIL);
		CMD_DEBUG("*** ERROR: HW_STACK_CHECKING is true, yet hardware doesn't support it.\r\n");
	} else if (val != 1) {
		TRESULT (TEST_STACKCK, TR_WARN);
		CMD_DEBUG("*** WARNING: stack checking version is incorrect.\r\n");
	}
#endif
#endif
	CMD_DEBUG ("\r\n");
}
#endif
#endif


void dump_IFQ_BUILD (void)
{
	uint32_t val;

	/****************************************/
	/** IFETCH_QUEUE_BUILD                 **/
	/****************************************/
	val = _arc_aux_read (IFETCH_QUEUE_BUILD);
	if (val != 0) {
		CMD_DEBUG ("IFETCH_QUEUE_BUILD (Instruction Fetch Queue Build Register)\r\n");
		CMD_DEBUG ("----------------------------------------------------\r\n");
		CMD_DEBUG ("0x%x : 0x%x\r\n", IFETCH_QUEUE_BUILD, val);
		CMD_DEBUG (" Version %d\r\n", val & 0xff);
		/* Assuming Version==2 */
		CMD_DEBUG (" BD[1:0] %d entries\r\n", (1<<((val>>8)&3)));
		CMD_DEBUG ("\r\n");
	}
}

/* show help of command bcr */
static void cmd_bcr_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}
	CMD_DEBUG("Usage: %s [OPTION]... \r\n"
		"Show processor build configuration\r\n"
		"  -h/H/?    Show the help information\r\n"
		"Examples: \r\n"
		"  bcr       Show processor build configuration\r\n"
		"  bcr -h    Show the help information\r\n", cmd_name);

error_exit:
	return;
}

/*-------------------------------------------------------------
*
* Task Name : cmd_bcr
* Comments  :
*END*-----------------------------------------------------------*/
int cmd_bcr (int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OK;
	static int32_t run_status = 0;

	if (run_status != 0) {
		ercd = E_SYS;
		goto error_exit;
	}

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	/* show the help information, if the option is "-h" */
	if(argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_bcr_help(argv[0], extobj);
		goto error_exit;
	}
	/* the parameter checking */
	if (argc > 1) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}
	run_status = 1;

	init_test_array ();

	dump_IDENTITY ();

	dump_BCR_VER ();

	dump_DCCM_BASE_BUILD ();

	dump_CRC_BUILD_BCR ();

	dump_BTA_LINK_BUILD ();

	dump_EA_BUILD ();

	dump_MEMSUBSYS ();

	dump_VECBASE ();

	dump_PBASE ();

	dump_RF_BUILD ();

	dump_MMU_BUILD ();

	dump_D_CACHE_BUILD ();

	dump_DCCM_BUILD ();

	dump_TIMER_BUILD ();

	dump_I_CACHE_BUILD ();

	dump_ICCM_BUILD ();

	dump_DSPRAM_BUILD ();

	dump_MAC_BUILD ();

	dump_MULTIPLY_BUILD ();

	dump_SWAP_BUILD ();

	dump_NORM_BUILD ();

	dump_BARREL_BUILD ();

	dump_PC_WIDTHS ();

#if defined(BUILD_ARC700)||defined(BUILD_ARCV2EM)||defined(BUILD_ARCV2HS)
#ifdef AUX_STACK_REGION_BUILD
#if defined(BUILD_ARCV2EM)
	if (core==1)
#endif
	dump_STACK_CHECKING();
#endif
#endif

#if defined(BUILD_ARCV2EM)
	if (core==1)
	dump_IRQ_BUILD();
#elif defined(BUILD_ARCV2HS)
	dump_IRQ_BUILD();
#endif

	dump_IFQ_BUILD ();

	test_result_report ();

	run_status = 0;
	return E_OK;

error_exit:
	return ercd;
}

static CMD_TABLE_T bcr_cmd = {"bcr", "Dump processor build configuration", cmd_bcr, NULL};
/**
* register bcr command
*/
CMD_TABLE_T * register_ntshell_cmd_bcr(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&bcr_cmd, prev);
}

/* eof */
#endif /* NTSHELL_USE_CMDS_ARC_BCR */
