/**
 * @file text_history.h
 * @author Shinichiro Nakamura
 * @brief NT-Shell用テキストヒストリモジュールの定義。
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

#ifndef TEXT_HISTORY_H
#define TEXT_HISTORY_H

#include "ntconf.h"

/**
 * @brief 1履歴あたりの文字列最大長。
 */
#define TEXTHISTORY_MAXLEN  (NTCONF_EDITOR_MAXLEN)

/**
 * @brief 履歴管理数。
 */
#define TEXTHISTORY_DEPTH   (NTCONF_HISTORY_DEPTH)

/**
 * @brief テキストヒストリ構造体。
 */
typedef struct {
    /**
     * @brief 履歴文字列バッファ。
     */
    char history[TEXTHISTORY_MAXLEN * TEXTHISTORY_DEPTH];

    /**
     * @brief リード位置。
     */
    int rp;

    /**
     * @brief ライト位置。
     */
    int wp;
} text_history_t;

#ifdef __cplusplus
extern "C" {
#endif

void text_history_init(text_history_t *p);
int text_history_write(text_history_t *p, char *buf);
int text_history_read(text_history_t *p, char *buf, const int siz);
int text_history_read_point_next(text_history_t *p);
int text_history_read_point_prev(text_history_t *p);
int text_history_find(text_history_t *p, const int index, const char *text, char *buf, const int siz);

#ifdef __cplusplus
}
#endif

#endif

