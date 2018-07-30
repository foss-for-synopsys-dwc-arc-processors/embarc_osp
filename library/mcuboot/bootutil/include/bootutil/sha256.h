/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * This module provides a thin abstraction over some of the crypto
 * primitives to make it easier to swap out the used crypto library.
 *
 * At this point, only mbedTLS is supported.
 */

#ifndef __BOOTUTIL_CRYPTO_H_
#define __BOOTUTIL_CRYPTO_H_

#include <mbedtls/sha256.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef mbedtls_sha256_context bootutil_sha256_context;

static inline void bootutil_sha256_init(bootutil_sha256_context *ctx)
{
    mbedtls_sha256_init(ctx);
    mbedtls_sha256_starts(ctx, 0);
}

static inline void bootutil_sha256_update(bootutil_sha256_context *ctx,
                                          const void *data,
                                          uint32_t data_len)
{
    mbedtls_sha256_update(ctx, data, data_len);
}

static inline void bootutil_sha256_finish(bootutil_sha256_context *ctx,
                                          uint8_t *output)
{
    mbedtls_sha256_finish(ctx, output);
}

#ifdef __cplusplus
}
#endif

#endif /* __BOOTUTIL_SIGN_KEY_H_ */
