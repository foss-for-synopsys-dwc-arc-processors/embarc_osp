/**
 * @file text_editor.c
 * @author Shinichiro Nakamura
 * @brief NT-Shell用テキストエディタモジュールの実装。
 * @details
 * 文字列の編集を論理的に扱うためのモジュール。
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

#include "text_editor.h"

/**
 * @brief テキストエディタを初期化する。
 *
 * @param p テキストエディタ構造体。
 */
void text_editor_init(text_editor_t *p)
{
    p->pos = 0;
    p->len = 0;
    p->buffer[p->len] = '\0';
}

/**
 * @brief 文字を挿入する。
 *
 * @param p テキストエディタ構造体。
 * @param c 文字。
 */
int text_editor_insert(text_editor_t *p, char c)
{
    if (p->len < (int)sizeof(p->buffer) - 1) {
        int n = p->len - p->pos + 1;
        int i;
        char *src = p->buffer + p->len + 0;
        char *des = p->buffer + p->len + 1;
        for (i = 0; i < n; i++) {
            *des = *src;
            des--;
            src--;
        }

        p->buffer[p->pos] = c;
        p->pos++;
        p->len++;
        p->buffer[p->len] = '\0';
        return 1;
    }
    return 0;
}

/**
 * @brief 文字を削除する。
 *
 * @param p テキストエディタ構造体。
 */
int text_editor_backspace(text_editor_t *p)
{
    if (0 < p->pos) {
        int n = p->len - p->pos;
        int i;
        char *src = p->buffer + p->pos - 0;
        char *des = p->buffer + p->pos - 1;
        p->pos--;
        p->len--;
        for (i = 0; i < n; i++) {
            *des = *src;
            des++;
            src++;
        }
        *(p->buffer + p->len) = '\0';
        return 1;
    }
    return 0;
}

/**
 * @brief 文字を削除する。
 *
 * @param p テキストエディタ構造体。
 */
int text_editor_delete(text_editor_t *p)
{
    if (p->pos < p->len) {
        int n = p->len - p->pos - 1;
        int i;
        char *src = p->buffer + p->pos + 1;
        char *des = p->buffer + p->pos + 0;
        p->len--;
        for (i = 0; i < n; i++) {
            *des = *src;
            des++;
            src++;
        }
        *(p->buffer + p->len) = '\0';
        return 1;
    }
    return 0;
}

/**
 * @brief カーソル位置を取得する。
 *
 * @param p テキストエディタ構造体。
 */
int text_editor_cursor_get_position(text_editor_t *p)
{
    return p->pos;
}

/**
 * @brief カーソルを先頭に移動させる。
 *
 * @param p テキストエディタ構造体。
 */
int text_editor_cursor_head(text_editor_t *p)
{
    if (0 < p->pos) {
        p->pos = 0;
        return 1;
    }
    return 0;
}

/**
 * @brief カーソルを最後尾に移動させる。
 *
 * @param p テキストエディタ構造体。
 */
int text_editor_cursor_tail(text_editor_t *p)
{
    if (p->pos < p->len) {
        p->pos = p->len;
        return 1;
    }
    return 0;
}

/**
 * @brief カーソルを左へ移動させる。
 *
 * @param p テキストエディタ構造体。
 */
int text_editor_cursor_left(text_editor_t *p)
{
    if (0 < p->pos) {
        p->pos--;
        return 1;
    }
    return 0;
}

/**
 * @brief カーソルを右へ移動させる。
 *
 * @param p テキストエディタ構造体。
 */
int text_editor_cursor_right(text_editor_t *p)
{
    if (p->pos < p->len) {
        p->pos++;
        return 1;
    }
    return 0;
}

/**
 * @brief 文字列を設定する。
 *
 * @param p テキストエディタ構造体。
 * @param buf 文字列が格納されたバッファ。
 */
int text_editor_set_text(text_editor_t *p, char *buf)
{
    char *src = buf;
    char *des = p->buffer;
    int n = 0;
    while (*src) {
        *des = *src;
        des++;
        src++;
        n++;
        if ((int)sizeof(p->buffer) <= n - 1) {
            break;
        }
    }
    *des = '\0';
    p->len = n;
    p->pos = p->len;
    return n;
}

/**
 * @brief 文字列を取得する。
 *
 * @param p テキストエディタ構造体。
 * @param buf 文字列を格納するバッファ。
 * @param siz バッファサイズ。
 */
int text_editor_get_text(text_editor_t *p, char *buf, int siz)
{
    char *src = p->buffer;
    char *des = buf;
    int n = 0;
    while (*src) {
        *des++ = *src++;
        n++;
        if (siz <= n) {
            break;
        }
    }
    *des = '\0';
    return n;
}

/**
 * @brief 文字列を消去する。
 *
 * @param p テキストエディタ構造体。
 */
void text_editor_clear(text_editor_t *p)
{
    p->pos = 0;
    p->len = 0;
    p->buffer[p->len] = '\0';
}

