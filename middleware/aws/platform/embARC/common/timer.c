/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file timer.c
 * @brief Linux implementation of the timer interface.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef MID_LWIP
#include <lwip/sys.h>
#else
#error "LwIP middleware is needed, please add lwip lwip-contrib to MID_SEL make option in your application makefile"
#endif

#include "timer_platform.h"

bool has_timer_expired(Timer *timer) {
	long left = (long)(timer->end_time) - (long)sys_now();
	return (left < 0);
}

void countdown_ms(Timer *timer, uint32_t timeout) {
	timer->end_time = sys_now() + timeout;
}

uint32_t left_ms(Timer *timer) {
	long left = (long)(timer->end_time) - (long)sys_now();
	return (left < 0) ? 0 : left;
}

void countdown_sec(Timer *timer, uint32_t timeout) {
	countdown_ms(timer, timeout*1000);
}

void init_timer(Timer *timer) {
	timer->end_time = 0;
}

#ifdef __cplusplus
}
#endif
