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
 * \ingroup EMBARC_TEST
 * \brief a light unit test framework based on CTest and Contiki App Unittest
 */

/**
 * \addtogroup	EMBARC_TEST
 * @{
 */

/* Copyright 2011,2012 Bas van den Berg
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
#ifndef _EMBARC_TEST_H_
#define _EMBARC_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*setup_func_t)(void*);		/*!< setup function before test case */
typedef void (*teardown_func_t)(void*);		/*!< teardown function after test case */

/**
 * \brief   Unit test structure
 */
struct unit_test {
	const char* ssname;		/*!< test suite name */
	const char* ttname;		/*!< test case name */
	void (*run)();			/*!< test case function */
	int skip;			/*!< whether to skip ? */

	void* data;			/*!< data to test function */
	setup_func_t setup;		/*!< setup function */
	teardown_func_t teardown;	/*!< teardown function */

	unsigned int magic;		/*!< magic number to identify test case */
};

#define __FNAME(sname, tname) __unit_test_##sname##_##tname##_run	/*!< generate test case function name */
#define __TNAME(sname, tname) __unit_test_##sname##_##tname		/*!< generate structure name */
#define __UNIT_TEST_MAGIC (0xdeadbeef)

/** all unit test data will be placed in a special data section .data.unittest */
#define __UNIT_TEST_SECTION __attribute__ ((unused,section (".embarc_unittest")))

#define __UNIT_TEST_STRUCT(sname, tname, _skip, __data, __setup, __teardown) \
	struct unit_test __TNAME(sname, tname) __UNIT_TEST_SECTION = {	\
		.ssname=#sname,						\
		.ttname=#tname,						\
		.run = __FNAME(sname, tname),				\
		.skip = _skip,						\
		.data = __data,						\
		.setup = (setup_func_t)__setup,				\
		.teardown = (teardown_func_t)__teardown,		\
		.magic = __UNIT_TEST_MAGIC };

/** data wrapper of test suite */
#define UNIT_TEST_DATA(sname) struct sname##_data

/** setup function of test suite */
#define UNIT_TEST_SETUP(sname)						\
	void __attribute__ ((weak)) sname##_setup(struct sname##_data* data)
/** teardown function of test suite */
#define UNIT_TEST_TEARDOWN(sname)					\
	void __attribute__ ((weak)) sname##_teardown(struct sname##_data* data)

/** naming macros of setup and tear down functions */
#define SETUP_FNAME(sname) sname##_setup
#define TEARDOWN_FNAME(sname) sname##_teardown

#define __UNIT_TEST_INTERNAL(sname, tname, _skip)			\
	void __FNAME(sname, tname)();					\
	__UNIT_TEST_STRUCT(sname, tname, _skip, NULL, NULL, NULL)	\
	void __FNAME(sname, tname)()

#define __UNIT_TEST2_INTERNAL(sname, tname, _skip)			\
	static struct sname##_data  __unit_test_##sname##_data;		\
	UNIT_TEST_SETUP(sname);						\
	UNIT_TEST_TEARDOWN(sname);					\
	void __FNAME(sname, tname)(struct sname##_data* data);		\
	__UNIT_TEST_STRUCT(sname, tname, _skip, &__unit_test_##sname##_data, SETUP_FNAME(sname), TEARDOWN_FNAME(sname))	\
	void __FNAME(sname, tname)(struct sname##_data* data)

#define UNIT_TEST(sname, tname) __UNIT_TEST_INTERNAL(sname, tname, 0)
#define UNIT_TEST_SKIP(sname, tname) __UNIT_TEST_INTERNAL(sname, tname, 1)

#define UNIT_TEST2(sname, tname) __UNIT_TEST2_INTERNAL(sname, tname, 0)
#define UNIT_TEST2_SKIP(sname, tname) __UNIT_TEST2_INTERNAL(sname, tname, 1)

/** check macros for unit test */
extern void unit_test_assert_str(const char* exp, const char* real, const char* caller, int line);
#define UNIT_TEST_ASSERT_STR(exp, real) unit_test_assert_str(exp, real, __FILE__, __LINE__)

void unit_test_assert_data(						\
	const unsigned char* exp, int expsize,				\
	const unsigned char* real, int realsize,			\
	const char* caller, int line);
#define UNIT_TEST_ASSERT_DATA(exp, expsize, real, realsize)		\
	unit_test_assert_data(exp, expsize, real, realsize, __FILE__, __LINE__)

extern void unit_test_assert_equal(long exp, long real, const char* caller, int line);
#define UNIT_TEST_ASSERT_EQUAL(exp, real) unit_test_assert_equal(exp, real, __FILE__, __LINE__)

extern void unit_test_assert_not_equal(long exp, long real, const char* caller, int line);
#define UNIT_TEST_ASSERT_NOT_EQUAL(exp, real) unit_test_assert_not_equal(exp, real, __FILE__, __LINE__)

extern void unit_test_assert_null(void* real, const char* caller, int line);
#define UNIT_TEST_ASSERT_NULL(real) unit_test_assert_null((void*)real, __FILE__, __LINE__)

extern void unit_test_assert_not_null(const void* real, const char* caller, int line);
#define UNIT_TEST_ASSERT_NOT_NULL(real) unit_test_assert_not_null(real, __FILE__, __LINE__)

extern void unit_test_assert_true(int real, const char* caller, int line);
#define UNIT_TEST_ASSERT_TRUE(real) unit_test_assert_true(real, __FILE__, __LINE__)

extern void unit_test_assert_false(int real, const char* caller, int line);
#define UNIT_TEST_ASSERT_FALSE(real) unit_test_assert_false(real, __FILE__, __LINE__)

extern void unit_test_assert_fail(const char* caller, int line);
#define UNIT_TEST_ASSERT_FAIL() unit_test_assert_fail(__FILE__, __LINE__)

extern int unit_test_run(const char * test_suite);

#ifdef __cplusplus
}
#endif

#endif	/*_EMBARC_TEST_H_ */
/** @} end of group EMBARC_TEST */
