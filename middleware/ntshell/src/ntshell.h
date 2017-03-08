/**
 * @file ntshell.h
 * @author Shinichiro Nakamura
 * @brief 小規模組み込みシステム向けのシェルシステムの定義。
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

#ifndef NTSHELL_H
#define NTSHELL_H

#include "vtrecv.h"
#include "vtsend.h"
#include "text_editor.h"
#include "text_history.h"

#define NTSHELL_PROMPT_MAXLEN   (32)
#define NTSHELL_PROMPT_DEFAULT  ">"
#define NTSHELL_PROMPT_NEWLINE  "\r\n"

typedef int (*NTSHELL_SERIAL_READ)(char *buf, int cnt, void *extobj);
typedef int (*NTSHELL_SERIAL_WRITE)(const char *buf, int cnt, void *extobj);
typedef int (*NTSHELL_USER_CALLBACK)(const char *text, void *extobj);

/**
 * @brief NT-Shelハンドラの実装。
 * @details
 * 本来このハンドラは定義のみを公開し、実装の詳細を隠蔽すべきだが、
 * ヒープからメモリを確保するインターフェースのない環境も有り得る事を考慮して公開してある。
 * このため、スタックに置いた構造体を使って制御する事も可能である。
 */
typedef struct {
    unsigned int initcode;  /**< 初期化確認コード。 */
    vtsend_t vtsend;        /**< vtsendハンドラ。 */
    vtrecv_t vtrecv;        /**< vtrecvハンドラ。 */
    text_editor_t editor;   /**< text_editorハンドラ。 */
    text_history_t history; /**< text_historyハンドラ。 */
    int suggest_index;
    char suggest_source[TEXTEDITOR_MAXLEN];
    NTSHELL_SERIAL_READ func_read;
    NTSHELL_SERIAL_WRITE func_write;
    NTSHELL_USER_CALLBACK func_callback;
    void *extobj;
    char prompt[NTSHELL_PROMPT_MAXLEN];
} ntshell_t;

#ifdef __cplusplus
extern "C" {
#endif

void ntshell_init(ntshell_t *p,
    NTSHELL_SERIAL_READ func_read,
    NTSHELL_SERIAL_WRITE func_write,
    NTSHELL_USER_CALLBACK func_callback,
    void *extobj);
void ntshell_execute(ntshell_t *p);
void ntshell_set_prompt(ntshell_t *p, const char *prompt);
void ntshell_version(int *major, int *minor, int *release);

#ifdef __cplusplus
}
#endif

#endif

