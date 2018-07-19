/*
 * Copyright (c) 2017 Linaro Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef H_BOOTUTIL_LOG_H_
#define H_BOOTUTIL_LOG_H_

#include "ignore.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define BOOT_LOG_LEVEL_OFF      0
#define BOOT_LOG_LEVEL_ERROR    1
#define BOOT_LOG_LEVEL_WARNING  2
#define BOOT_LOG_LEVEL_INFO     3
#define BOOT_LOG_LEVEL_DEBUG    4

/*
 * The compiled log level determines the maximum level that can be
 * printed. Messages at or below this level can be printed.
 */
#ifndef BOOT_LOG_LEVEL
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#endif

int sim_log_enabled(int level);

#if BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_ERROR
#define BOOT_LOG_ERR(_fmt, ...)                  \
    printf("[ERR] " _fmt "\r\n", ##__VA_ARGS__)
#else
#define BOOT_LOG_ERR(...) IGNORE(__VA_ARGS__)
#endif

#if BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_WARNING
#define BOOT_LOG_WRN(_fmt, ...)                  \
    printf("[WRN] " _fmt "\r\n", ##__VA_ARGS__)
#else
#define BOOT_LOG_WRN(...) IGNORE(__VA_ARGS__)
#endif

#if BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_INFO
#define BOOT_LOG_INF(_fmt, ...)                  \
    printf("[INF] " _fmt "\r\n", ##__VA_ARGS__)
#else
#define BOOT_LOG_INF(...) IGNORE(__VA_ARGS__)
#endif

#if BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_DEBUG
#define BOOT_LOG_DBG(_fmt, ...)                  \
    printf("[DBG] " _fmt "\r\n", ##__VA_ARGS__)
#else
#define BOOT_LOG_DBG(...) IGNORE(__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif
