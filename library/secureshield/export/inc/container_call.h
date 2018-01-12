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
#ifndef _CONTAINER_CALL_H_
#define _CONTAINER_CALL_H_

#include "arc_feature_config.h"

#if ARC_FEATURE_MPU_BUILD_S == 1 && SECURESHIELD_USE_MPU_SID == 1
/* container call */
#define container_call(dst_container, dst_fn, ...) \
	({ \
		uint32_t res = SECURESHIELD_SECURE_CALL(	\
			SECURESHIELD_SECURE_CALL_CONTAINER_IN, \
			"", \
			SECURESHIELD_MACRO_NARGS(__VA_ARGS__), &dst_container ## _cfg_ptr, dst_fn, ##__VA_ARGS__); \
		res; \
	})

#else
#define __SECURESHIELD_CONTAINER_CALL_METADATA(dst_container, dst_fn) \
	"b skip_args%=\n" \
	".long " EMBARC_TO_STRING(SECURESHIELD_CONTAINER_CALL_MAGIC) "\n" \
	".long " EMBARC_TO_STRING(dst_fn) "\n" \
	".long " EMBARC_TO_STRING(dst_container) "_cfg_ptr\n" \
	"skip_args%=:\n"


/* container call */
#define container_call(dst_container, dst_fn, ...) \
	({ \
		uint32_t res = SECURESHIELD_SECURE_CALL(	\
			SECURESHIELD_SECURE_CALL_CONTAINER_IN, \
			__SECURESHIELD_CONTAINER_CALL_METADATA(dst_container, dst_fn), \
			SECURESHIELD_MACRO_NARGS(__VA_ARGS__), ##__VA_ARGS__); \
		res; \
	})

#endif


#endif /* _CONTAINER_CALL_H_ */
