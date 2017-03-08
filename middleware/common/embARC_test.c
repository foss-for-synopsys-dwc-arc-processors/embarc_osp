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
/*
 * Modified for port to ARC processor
 * by Wayne Ren wei.ren@synopsys.com, Synopsys, Inc.
 */

/**
 * \file
 * \ingroup EMBARC_TEST
 * \brief a light unit test framework based on CTest and  Contiki App\unitest
 */

#include <setjmp.h>
#include <string.h>
#include "embARC_test.h"

#ifndef EMBARC_PRINTF
	#ifdef MID_COMMON
		#include "xprintf.h"
		#define EMBARC_PRINTF xprintf
	#else
		#include <stdio.h>
		#define EMBARC_PRINTF printf
	#endif
#endif


#define UNIT_TEST_ERR(fmt, ...)	do {						\
					EMBARC_PRINTF(fmt, ##__VA_ARGS__);	\
					longjmp(unit_test_err, 1);		\
				} while(0)

typedef int (*filter_func_t)(struct unit_test*);

static jmp_buf unit_test_err;
static const char *suite_name;


/**
 * \brief a default test suite filter
 *
 * \param t	pointer to test case
 *
 * \returns 1
 */
static int suite_all(struct unit_test* t) {
	return 1;
}


/**
 * \brief a test suite filter by name
 *
 * \param t pointer to test case
 *
 * \returns 1 in the suite, 0 not in the suite
 */
static int suite_filter(struct unit_test* t) {
	return strncmp(suite_name, t->ssname, strlen(suite_name)) == 0;
}

/**
 * \brief assert when the real string isn't the expected string
 *
 * \param exp	expected string
 * \param real	real string
 * \param caller the caller file name string
 * \param line	line number in the caller file
 */
void unit_test_assert_str(const char* exp, const char*  real, const char* caller, int line) {
	if ((exp == NULL && real != NULL) ||
		(exp != NULL && real == NULL) ||
		(exp && real && strcmp(exp, real) != 0)) {
		UNIT_TEST_ERR("%s:%d  expected '%s', got '%s'\n", caller, line, exp, real);
	}
}

/**
 * \brief assert when the real data area doesn't equal the expected data area
 *
 * \param exp		expected data area
 * \param expsize	data size of expected data area
 * \param real		real data area
 * \param realsize	data size of real data area
 * \param caller	the caller file
 * \param line		line number in the caller file
 */
void unit_test_assert_data(				\
	const unsigned char* exp, int expsize,		\
	const unsigned char* real, int realsize,	\
	const char* caller, int line) {
	int i;
	if (expsize != realsize) {
		UNIT_TEST_ERR("%s:%d  expected %d bytes, got %d\n",	\
					caller, line, expsize, realsize);
	}
	for (i=0; i<expsize; i++) {
		if (exp[i] != real[i]) {
			UNIT_TEST_ERR("%s:%d expected 0x%02x at offset %d got 0x%02x\n",\
					caller, line, exp[i], i, real[i]);
		}
	}
}

/**
 * \brief assert when the real value doesn't equal the expected value
 *
 * \param exp	expected value
 * \param real	real value
 * \param caller the caller file name string
 * \param line	line number in the caller file
 */
void unit_test_assert_equal(long exp, long real, const char* caller, int line) {
	if ((exp) != (real)) {
		UNIT_TEST_ERR("%s:%d  expected %ld, got %ld\n", caller, line, exp, real);
	}
}

/**
 * \brief assert when the real value equal the expected value
 *
 * \param exp	expected value
 * \param real	real value
 * \param caller the caller file name string
 * \param line	line number in the caller file
 */
void unit_test_assert_not_equal(long exp, long real, const char* caller, int line) {
	if ((exp) == (real)) {
		UNIT_TEST_ERR("%s:%d  should not be %ld\n", caller, line, real);
	}
}

/**
 * \brief assert when the real value is not null
 *
 * \param real	real value
 * \param caller the caller file name string
 * \param line	line number in the caller file
 */
void unit_test_assert_null(void* real, const char* caller, int line) {
	if ((real) != NULL) {
		UNIT_TEST_ERR("%s:%d  should be NULL\n", caller, line);
	}
}

/**
 * \brief assert when the real value is null
 *
 * \param real	real value
 * \param caller the caller file name string
 * \param line	line number in the caller file
 */
void unit_test_assert_not_null(const void* real, const char* caller, int line) {
	if ((real) == NULL) {
		UNIT_TEST_ERR("%s:%d  should not be NULL\n", caller, line);
	}
}

/**
 * \brief assert when the real value is not true
 *
 * \param real	real value
 * \param caller the caller file name string
 * \param line	line number in the caller file
 */
void unit_test_assert_true(int real, const char* caller, int line) {
	if ((real) == 0) {
		UNIT_TEST_ERR("%s:%d  should be true\n", caller, line);
	}
}

/**
 * \brief assert when the real value is not false
 *
 * \param real	real value
 * \param caller the caller file name string
 * \param line	line number in the caller file
 */
void unit_test_assert_false(int real, const char* caller, int line) {
	if ((real) != 0) {
		UNIT_TEST_ERR("%s:%d  should be false\n", caller, line);
	}
}

/**
 * \brief assert when fail unconditionally
 *
 * \param caller the caller file name string
 * \param line	line number in the caller file
 */
void unit_test_assert_fail(const char* caller, int line) {
	UNIT_TEST_ERR("%s:%d  shouldn't come here\n", caller, line);
}


extern struct unit_test _f_embarc_unittest;
extern struct unit_test _e_embarc_unittest;
/**
 * \brief run unit test
 *
 * \param test_suite the test suite need to run, NULL for all
 *
 * \returns the number of failed test cases
 */
int unit_test_run(const char * test_suite)
{
	int total = 0;
	int num_ok = 0;
	int num_fail = 0;
	int num_skip = 0;
	int index = 1;
	filter_func_t filter = suite_all;
	struct unit_test* unit_test_begin = (struct unit_test*)(&_f_embarc_unittest);
	struct unit_test* unit_test_end = (struct unit_test*)(&_e_embarc_unittest);
	struct unit_test* test;

	if (test_suite != NULL) {
		filter = suite_filter;
		suite_name = test_suite;
	}

	for (test = unit_test_begin; test < unit_test_end; test++) {
		if (test->magic != __UNIT_TEST_MAGIC) {
			EMBARC_PRINTF("embarc_unittest section corrupted:start:%x, end:%x, test:%x\n",\
					unit_test_begin, unit_test_end, test);
			return -1;
		}
		if (filter(test)) total++;
	}
	EMBARC_PRINTF("----embARC unit tests----\n");
	EMBARC_PRINTF("start:0x%x, end:0x%x, total:%d\n",	\
			unit_test_begin, unit_test_end, total);
	for (test = unit_test_begin; test < unit_test_end; test++) {
		if (filter(test)) {
			EMBARC_PRINTF("TEST %d/%d %s:%s\n ",	\
					index, total, test->ssname, test->ttname);
			if (test->skip) {
				EMBARC_PRINTF("[SKIPPED]\n");
				num_skip++;
			} else {
				int result = setjmp(unit_test_err);
				if (result == 0) {
					if (test->setup) test->setup(test->data);
					if (test->data)
						test->run(test->data);
					else
					test->run();
					if (test->teardown) test->teardown(test->data);
					EMBARC_PRINTF("[OK]\n");
					num_ok++;
				} else {
					EMBARC_PRINTF("[FAIL]\n");
					num_fail++;
				}
			}
			index++;
		}
	}
	EMBARC_PRINTF("RESULTS: %d tests (%d ok, %d failed, %d skipped)\n",	\
			total, num_ok, num_fail, num_skip);

	return num_fail;
}
