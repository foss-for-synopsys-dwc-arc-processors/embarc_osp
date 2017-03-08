/**
 * @file ntopt.h
 * @author Shinichiro Nakamura
 * @brief NT-Shell用オプション解析モジュールの定義。
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

#ifndef NTOPT_H
#define NTOPT_H

#include "ntconf.h"

/**
 * @brief パース可能な最大文字列長。
 */
#define NTOPT_TEXT_MAXLEN   (NTCONF_EDITOR_MAXLEN)

/**
 * @brief パース可能な最大パラメータ数。
 */
#define NTOPT_TEXT_MAXARGS  (NTCONF_EDITOR_MAXLEN / 2)

/**
 * @brief コールバック関数。
 *
 * @param argc パラメータの数。
 * @param argv パラメータ。
 * @param extobj 外部オブジェクト。
 *
 * @return 返値。
 */
typedef int (*NTOPT_CALLBACK)(int argc, char **argv, void *extobj);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 与えられた文字列をデリミタで分割する。
 *
 * @param str 文字列。
 * @param func コールバック関数。
 * @param extobj 拡張オブジェクト。
 *
 * @return コールバック関数が返した値。
 */
int ntopt_parse(const char *str, NTOPT_CALLBACK func, void *extobj);

#ifdef __cplusplus
}
#endif

#endif

