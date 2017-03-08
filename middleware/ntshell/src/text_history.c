/**
 * @file text_history.c
 * @author Shinichiro Nakamura
 * @brief NT-Shell用テキストヒストリモジュールの実装。
 * @details
 * 文字列の入力履歴を論理的に扱うためのモジュール。
 * このモジュールはビューに関して一切感知しない。
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

#include "text_history.h"
#include "ntlibc.h"

/**
 * @brief 初期化する。
 *
 * @param p テキストヒストリ構造体。
 */
void text_history_init(text_history_t *p)
{
    int i;
    p->rp = 0;
    p->wp = 0;
    for (i = 0; i < (int)sizeof(p->history); i++) {
        p->history[i] = 0;
    }
}

/**
 * @brief テキストヒストリに対して書き込みを実行する。
 *
 * @param p テキストヒストリ構造体。
 * @param buf バッファ。
 */
int text_history_write(text_history_t *p, char *buf)
{
    char *sp = p->history + (TEXTHISTORY_MAXLEN * p->wp);
    if (buf[0] == '\0') {
        return 0;
    }
    while (*buf) {
        *sp = *buf;
        sp++;
        buf++;
    }
    *sp = '\0';
    p->wp = (p->wp + 1) % TEXTHISTORY_DEPTH;
    p->rp = p->wp;
    return 1;
}

/**
 * @brief テキストヒストリから読み出しを実行する。
 * @details
 * 得られる文字列が与えられたバッファサイズよりも大きい場合、
 * バッファに格納される文字列は途中で途切れるものとする。
 *
 * @param p テキストヒストリ構造体。
 * @param buf バッファ。
 * @param siz バッファサイズ。
 */
int text_history_read(text_history_t *p, char *buf, const int siz)
{
    char *sp = p->history + (TEXTHISTORY_MAXLEN * p->rp);
    int n = 0;
    while (*sp) {
        *buf = *sp;
        buf++;
        sp++;
        n++;
        if (siz - 1 <= n) {
            break;
        }
    }
    *buf = '\0';
    return n;
}

/**
 * @brief 読み出しポインタを次に進める。
 *
 * @param p テキストヒストリ構造体。
 */
int text_history_read_point_next(text_history_t *p)
{
    int n = (p->rp + 1) % TEXTHISTORY_DEPTH;
    if (n != p->wp) {
        p->rp = n;
        return 1;
    }
    return 0;
}

/**
 * @brief 読み出しポインタを前に戻す。
 *
 * @param p テキストヒストリ構造体。
 */
int text_history_read_point_prev(text_history_t *p)
{
    int n = (p->rp == 0) ? (TEXTHISTORY_DEPTH - 1) : (p->rp - 1);
    if (n != p->wp) {
        char *sp = p->history + (TEXTHISTORY_MAXLEN * n);
        if (*sp != '\0') {
            p->rp = n;
            return 1;
        }
    }
    return 0;
}

/**
 * @brief 与えられたテキストで始まる文字列を探す。
 * @details このインターフェースはテキスト入力補完のために作られた。
 *
 * @param p テキストヒストリオブジェクト。
 * @param index ヒストリ中で見つかる文字列のindex個目。
 * @param text 検索する文字列。
 * @param buf 格納先バッファ。
 * @param siz 格納先バッファサイズ。
 *
 * @retval 0 成功。
 * @retval 0以外 失敗。
 */
int text_history_find(text_history_t *p,
        const int index, const char *text,
        char *buf, const int siz)
{
    const int text_len = ntlibc_strlen((const char *)text);
    int found = 0;
    int i;
    for (i = 0; i < TEXTHISTORY_DEPTH; i++) {
        int target = (p->rp + i) % TEXTHISTORY_DEPTH;
        char *txtp = p->history + (TEXTHISTORY_MAXLEN * target);
        const int target_len = ntlibc_strlen((const char *)txtp);
        int comp_len = (target_len < text_len) ? target_len : text_len;
        if ((ntlibc_strncmp(
                    (const char *)txtp,
                    (const char *)text, comp_len) == 0) && (comp_len > 0)) {
            if (found == index) {
                if (siz <= ntlibc_strlen(txtp)) {
                    return -1;
                }
                ntlibc_strcpy((char *)buf, (char *)txtp);
                return 0;
            }
            found++;
        }
    }
    return -1;
}

