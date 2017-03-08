/*
 *  Copyright (c) 2016, Nest Labs, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for logging.
 *
 * Modified by Qiang Gu(Qiang.Gu@synopsys.com) for embARC
 * \version 2016.12
 * \date 2016-11-7
 *
 */

#include <stdio.h>
#include <stdarg.h>

#include <platform/logging.h>
#include "platform-emsk.h"

void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
	va_list args;

	switch (aLogLevel)
	{
	case kLogLevelNone:
		printf("NONE ");
		break;

	case kLogLevelCrit:
		printf("CRIT ");
		break;

	case kLogLevelWarn:
		printf("WARN ");
		break;

	case kLogLevelInfo:
		printf("INFO ");
		break;

	case kLogLevelDebg:
		printf("DEBG ");
		break;
	}

	switch (aLogRegion)
	{
	case kLogRegionApi:
		printf("API  ");
		break;

	case kLogRegionMle:
		printf("MLE  ");
		break;

	case kLogRegionArp:
		printf("ARP  ");
		break;

	case kLogRegionNetData:
		printf("NETD ");
		break;

	case kLogRegionIp6:
		printf("IPV6 ");
		break;

	case kLogRegionIcmp:
		printf("ICMP ");
		break;

	case kLogRegionMac:
		printf("MAC  ");
		break;

	case kLogRegionMem:
		printf("MEM  ");
		break;

	case kLogRegionNcp:
		printf("NCP  ");
		break;

	case kLogRegionMeshCoP:
		printf("MeshCop  ");
		break;

	case kLogRegionNetDiag:
		printf("NetDiag  ");
		break;

	case kLogRegionPlatform:
		printf("Platform  ");
		break;
	}

	va_start(args, aFormat);
	vprintf(aFormat, args);
	printf("\r\n");
	va_end(args);
}

