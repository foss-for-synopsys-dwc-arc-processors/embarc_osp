/**
 * @file ntopt.c
 * @author Shinichiro Nakamura
 * @brief NT-Shell用オプション解析モジュールの実装。
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

#include "ntopt.h"

/**
 * @brief 与えられた文字がデリミタであるかどうかを判定する。
 * @param c 文字。
 * @retval true デリミタである。
 * @retval false デリミタでない。
 */
#define IS_DELIM(c) \
    (((c) == '\r') || ((c) == '\n') || ((c) == '\t') || ((c) == '\0') || ((c) == ' '))

static int ntopt_get_count(const char *str);
static char *ntopt_get_text(
        const char *str, const int n, char *buf, int siz, int *len);

/**
 * @brief 与えられた文字列のセンテンス数を取得する。
 * @param str 文字列。
 * @return センテンス数。
 */
static int ntopt_get_count(const char *str)
{
    int cnt = 0;
    int wc = 0;
    char *p = (char *)str;
    while (*p) {
        if (!IS_DELIM(*p)) {
            wc++;
            if (wc == 1) {
                cnt++;
            }
        } else {
            wc = 0;
        }
        p++;
    }
    return cnt;
}

/**
 * @brief 与えられた文字列のセンテンスを取得する。
 * @param str 文字列。
 * @param n センテンス。(0からntopt-get_count(str) - 1までの値。)
 * @param buf 格納バッファ。
 * @param siz 格納バッファのサイズ。
 * @param len 格納した文字列のサイズ。
 * @retval !NULL 取得成功。格納バッファへのポインタ。
 * @retval NULL 取得失敗。
 */
static char *ntopt_get_text(
        const char *str, const int n, char *buf, int siz, int *len)
{
    int cnt = 0;
    int wc = 0;
    char *p = (char *)str;
    *len = 0;
    while (*p) {
        if (!IS_DELIM(*p)) {
            wc++;
            if (wc == 1) {
                if (cnt == n) {
                    char *des = buf;
                    int cc = 0;
                    while (!IS_DELIM(*p)) {
                        cc++;
                        if (siz <= cc) {
                            break;
                        }
                        *des = *p;
                        des++;
                        p++;
                    }
                    *des = '\0';
                    *len = cc;
                    return buf;
                }
                cnt++;
            }
        } else {
            wc = 0;
        }
        p++;
    }
    return 0;
}

/**
 * @brief 与えられた文字列をデリミタで分割する。
 * @param str 文字列。
 * @param func コールバック関数。
 * @param extobj 拡張オブジェクト。
 * @return コールバック関数が返した値。
 */
int ntopt_parse(const char *str, NTOPT_CALLBACK func, void *extobj)
{
    int argc;
    char argv[NTOPT_TEXT_MAXLEN];
    char *argvp[NTOPT_TEXT_MAXARGS];
    int i;
    int total;
    char *p;

    argc = ntopt_get_count(str);
    if (NTOPT_TEXT_MAXARGS <= argc) {
        argc = NTOPT_TEXT_MAXARGS;
    }

    total = 0;
    p = &argv[0];
    for (i = 0; i < argc; i++) {
        int len;
        argvp[i] = ntopt_get_text(
                str, i, p, NTOPT_TEXT_MAXLEN - total, &len);
        if (total + len + 1 < NTOPT_TEXT_MAXLEN) {
            p += len + 1;
            total += len + 1;
        } else {
            break;
        }
    }

    return func(argc, &argvp[0], extobj);
}

