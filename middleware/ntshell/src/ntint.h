/**
 * @file ntint.h
 * @author Shinichiro Nakamura
 * @brief 整数型定義。
 */

/*
 * ===============================================================
 *  Natural Tiny Shell (NT-Shell)
 * ===============================================================
 * Copyright (c) 2010-2012 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * ===============================================================
 */

#ifndef NTINT_H
#define NTINT_H

/**
 * @note
 * このファイルでは整数型定義を行なう。
 * 整数型定義は、比較的新しいツールチェインで
 * #include <stdint.h>を期待しても良いが、
 * これが使用できない場合もあるだろう。
 * 例えば小規模組み込みシステム向け開発環境として
 * libcやその他に依存させたくないケースも考えられる。
 * ここではNT-Shellが使用する型定義を宣言する事にする。
 * 既存の型定義を使用する場合、単に#include <stdint.h>
 * のように既存の定義ファイルへのインクルードを宣言すれば良い。
 */

#if 1
#   include <stdint.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned int size_t;
#endif

#endif

