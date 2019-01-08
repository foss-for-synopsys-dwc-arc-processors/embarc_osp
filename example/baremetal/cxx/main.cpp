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

#include "embARC.h"
#include "embARC_debug.h"

template <typename R>
class test_item {
public:
	R get_value() const;
	void set_value(R x);

	bool same_value(const test_item &target) const
	{
		return value == target.value;
	}
	test_item(): value(1), id(2) {}
	test_item(R init_value): value(init_value), id(3){}
	test_item(R init_value, R init_id): value(init_value), id(init_id){}
private:
	R value;
	R id;
};

template<int i>
class B
{
public:
	int num()
	{
		return i * B<i-1>().num();
	}
};

template<>
class B <1>
{
public:
	int num()
	{
		return 1;
	}
};


template<typename T, int i=1>
class someComputing {
public:
	typedef volatile T* retType;
	enum { retValume = i + someComputing<T, i-1>::retValume }; // recursion
	static void f() { EMBARC_PRINTF("someComputing: i=%d\n", i); }
};

template<typename T> // template specialization, end condition
class someComputing<T, 0> {
public:
	enum { retValume = 0 };
};

template<typename T>
class codeComputing {
public:
	static void f() { T::f(); }
};


template <typename R> R test_item<R>:: get_value() const
{
	EMBARC_PRINTF("id is %d \t", id);
	return value;
}

template <typename R> void test_item<R>:: set_value(R x)
{
	value = x;
}

test_item<int> y;

int main(void)
{
	test_item<int> x;
	test_item<int> z(4);
	test_item<int> w(5,0);

	int i = B<5>().num();
	someComputing<int>::retType a=0;

	EMBARC_PRINTF("a is %d\n",sizeof(a));
	EMBARC_PRINTF("i:%d\n", i);
	i = someComputing<int, 256>::retValume;
	EMBARC_PRINTF("i is:%d\n", i);
	codeComputing< someComputing<int, 99> >::f();
	EMBARC_PRINTF("x:%d\n", x.get_value());
	EMBARC_PRINTF("y:%d\n", y.get_value());
	x.set_value(3);
	y.set_value(3);
	EMBARC_PRINTF("z:%d\n", z.get_value());
	EMBARC_PRINTF("w:%d\n", w.get_value());

	if (x.same_value(y)) {
		EMBARC_PRINTF("x and y are the same\r\n");
	}
}
