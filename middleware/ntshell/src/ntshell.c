/**
 * @file ntshell.c
 * @author Shinichiro Nakamura
 * @brief 小規模組み込みシステム向けのシェルシステムの実装。
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

#include "ntshell.h"
#include "ntlibc.h"

#define VERSION_MAJOR   0   /**< メジャー番号。 */
#define VERSION_MINOR   2   /**< マイナー番号。 */
#define VERSION_RELEASE 0   /**< リリース番号。 */

#define INITCODE    (0x4367)    /**< 初期化確認コード。 */

/**
 * @brief 未使用変数宣言。
 *
 * @param N 未使用変数。
 */
#define UNUSED_VARIABLE(N)      do { (void)(N); } while (0)

/**
 * @brief サジェストのインデックス番号。
 *
 * @param HANDLE ハンドル。
 */
#define SUGGEST_INDEX(HANDLE)   ((HANDLE)->suggest_index)

/**
 * @brief サジェストのソース文字列。
 *
 * @param HANDLE ハンドル。
 */
#define SUGGEST_SOURCE(HANDLE)  ((HANDLE)->suggest_source)

/**
 * @brief テキストエディタを取得する。
 *
 * @param HANDLE ハンドル。
 */
#define GET_EDITOR(HANDLE)      (&((HANDLE)->editor))

/**
 * @brief テキストヒストリを取得する。
 *
 * @param HANDLE ハンドル。
 */
#define GET_HISTORY(HANDLE)     (&((HANDLE)->history))

/**
 * @brief シリアルポートから読み込む。
 *
 * @param HANDLE ハンドル。
 * @param BUF 読み込みバッファ。
 * @param CNT 読み込み文字数。
 *
 * @return 実際に読み込んだ文字数。
 */
#define SERIAL_READ(HANDLE, BUF, CNT)   ((HANDLE)->func_read(BUF, CNT, (HANDLE)->extobj))

/**
 * @brief シリアルポートへ書き込む。
 *
 * @param HANDLE ハンドル。
 * @param BUF 書き込みバッファ。
 * @param CNT 書き込み文字数。
 *
 * @return 実際に書き込んだ文字数。
 */
#define SERIAL_WRITE(HANDLE, BUF, CNT)  ((HANDLE)->func_write(BUF, CNT, (HANDLE)->extobj))

/**
 * @brief シリアルポートへプロンプトを書き込む。
 *
 * @param HANDLE ハンドル。
 */
#define PROMPT_WRITE(HANDLE)            SERIAL_WRITE((HANDLE), (HANDLE)->prompt, ntlibc_strlen((HANDLE)->prompt))

/**
 * @brief シリアルポートへ改行を書き込む。
 *
 * @param HANDLE ハンドル。
 */
#define PROMPT_NEWLINE(HANDLE)          SERIAL_WRITE((HANDLE), NTSHELL_PROMPT_NEWLINE, ntlibc_strlen(NTSHELL_PROMPT_NEWLINE))

/**
 * @brief コールバックを呼び出す。
 *
 * @param HANDLE ハンドル。
 * @param TEXT コールバック関数へ渡す文字列。
 */
#define CALLBACK(HANDLE, TEXT)          ((HANDLE)->func_callback((TEXT), (HANDLE)->extobj))

#define VTSEND_ERASE_LINE(HANDLE)       (vtsend_erase_line(&((HANDLE)->vtsend)))
#define VTSEND_CURSOR_HEAD(HANDLE)      (vtsend_cursor_backward(&((HANDLE)->vtsend), 80))
#define VTSEND_CURSOR_PREV(HANDLE)      (vtsend_cursor_backward(&((HANDLE)->vtsend), 1))
#define VTSEND_CURSOR_NEXT(HANDLE)      (vtsend_cursor_forward(&((HANDLE)->vtsend), 1))

/**
 * @brief テキストヒストリで１つ後ろを辿る。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_history_prev(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    if (text_history_read_point_prev(GET_HISTORY(ntshell))) {
        char txt[TEXTHISTORY_MAXLEN];
        int n = text_history_read(GET_HISTORY(ntshell), &txt[0], sizeof(txt));
        if (0 < n) {
            VTSEND_ERASE_LINE(ntshell);
            VTSEND_CURSOR_HEAD(ntshell);
            PROMPT_WRITE(ntshell);
            SERIAL_WRITE(ntshell, txt, n);
            text_editor_set_text(GET_EDITOR(ntshell), txt);
        }
    }
}

/**
 * @brief テキストヒストリで１つ前を辿る。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_history_next(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    if (text_history_read_point_next(GET_HISTORY(ntshell))) {
        char txt[TEXTHISTORY_MAXLEN];
        int n = text_history_read(GET_HISTORY(ntshell), &txt[0], sizeof(txt));
        if (0 < n) {
            VTSEND_ERASE_LINE(ntshell);
            VTSEND_CURSOR_HEAD(ntshell);
            PROMPT_WRITE(ntshell);
            SERIAL_WRITE(ntshell, txt, n);
            text_editor_set_text(GET_EDITOR(ntshell), txt);
        }
    }
}

/**
 * @brief カーソルを左へ移動させる。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_cursor_left(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    if (text_editor_cursor_left(GET_EDITOR(ntshell))) {
        VTSEND_CURSOR_PREV(ntshell);
    }
}

/**
 * @brief カーソルを右へ移動させる。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_cursor_right(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    if (text_editor_cursor_right(GET_EDITOR(ntshell))) {
        VTSEND_CURSOR_NEXT(ntshell);
    }
}

/**
 * @brief エンターキーの処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_enter(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    char txt[TEXTEDITOR_MAXLEN];
    text_editor_get_text(GET_EDITOR(ntshell), &txt[0], sizeof(txt));
    text_editor_clear(GET_EDITOR(ntshell));
    text_history_write(GET_HISTORY(ntshell), txt);
    PROMPT_NEWLINE(ntshell);
    CALLBACK(ntshell, txt);
    PROMPT_WRITE(ntshell);
}

/**
 * @brief キャンセルキーの処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * 一般的なOSのCTRL+C処理はシグナルを発行し、受信したプロセスが
 * 中断処理を実行する。
 * ここでのキャンセルは見た目を再現したもので、
 * 入力中の文字列を破棄してカーソルを新しい入力に備えて復帰させるものだ。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_cancel(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    text_editor_clear(GET_EDITOR(ntshell));
    SERIAL_WRITE(ntshell, "^C", 2);
    PROMPT_NEWLINE(ntshell);
    PROMPT_WRITE(ntshell);
}

/**
 * @brief 挿入処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_insert(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    /*
     * 入力があった場合、入力補完状態から抜ける。
     */
    SUGGEST_INDEX(ntshell) = -1;

    /*
     * テキストエディタを使って文字を文字列に挿入する。
     */
    if (text_editor_insert(GET_EDITOR(ntshell), ch)) {
        char txt[TEXTEDITOR_MAXLEN];
        int len = text_editor_get_text(GET_EDITOR(ntshell), &txt[0], sizeof(txt));
        int pos = text_editor_cursor_get_position(GET_EDITOR(ntshell));
        int n = len - pos;
        SERIAL_WRITE(ntshell, (char *)&ch, sizeof(ch));
        if (n > 0) {
            int i;
            SERIAL_WRITE(ntshell, txt + pos, len - pos);
            for (i = 0; i < n; i++) {
                VTSEND_CURSOR_PREV(ntshell);
            }
        }
    }
}

/**
 * @brief バックスペース処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_backspace(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    if (text_editor_backspace(GET_EDITOR(ntshell))) {
        char txt[TEXTEDITOR_MAXLEN];
        int len = text_editor_get_text(GET_EDITOR(ntshell), &txt[0], sizeof(txt));
        int pos = text_editor_cursor_get_position(GET_EDITOR(ntshell));
        int n = len - pos;
        VTSEND_CURSOR_PREV(ntshell);
        if (n > 0) {
            int i;
            SERIAL_WRITE(ntshell, txt + pos, n);
            SERIAL_WRITE(ntshell, " ", 1);
            for (i = 0; i < n + 1; i++) {
                VTSEND_CURSOR_PREV(ntshell);
            }
        } else {
            SERIAL_WRITE(ntshell, " ", 1);
            VTSEND_CURSOR_PREV(ntshell);
        }
    }
}

/**
 * @brief デリート処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_delete(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    if (text_editor_delete(GET_EDITOR(ntshell))) {
        char txt[TEXTEDITOR_MAXLEN];
        int len = text_editor_get_text(GET_EDITOR(ntshell), &txt[0], sizeof(txt));
        int pos = text_editor_cursor_get_position(GET_EDITOR(ntshell));
        int n = len - pos;
        if (n > 0) {
            int i;
            SERIAL_WRITE(ntshell, txt + pos, n);
            SERIAL_WRITE(ntshell, " ", 1);
            for (i = 0; i < n + 1; i++) {
                VTSEND_CURSOR_PREV(ntshell);
            }
        } else {
            SERIAL_WRITE(ntshell, " ", 1);
            VTSEND_CURSOR_PREV(ntshell);
        }
    }
}

/**
 * @brief 入力補完処理を実行する。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_suggest(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    char buf[TEXTEDITOR_MAXLEN];
    if (SUGGEST_INDEX(ntshell) < 0) {
        /*
         * 入力補完モードにこれから入る場合。
         * 現在の入力文字列を元に補完候補を取得する。
         */
        if (text_editor_get_text(
                    GET_EDITOR(ntshell),
                    SUGGEST_SOURCE(ntshell),
                    sizeof(SUGGEST_SOURCE(ntshell))) > 0) {
            SUGGEST_INDEX(ntshell) = 0;
            if (text_history_find(
                        GET_HISTORY(ntshell),
                        SUGGEST_INDEX(ntshell),
                        SUGGEST_SOURCE(ntshell),
                        buf,
                        sizeof(buf)) == 0) {
                // 候補が見つかればテキストを設定して、インデックスをメモする。
                int n = ntlibc_strlen((const char *)buf);
                VTSEND_ERASE_LINE(ntshell);
                VTSEND_CURSOR_HEAD(ntshell);
                PROMPT_WRITE(ntshell);
                SERIAL_WRITE(ntshell, buf, n);
                text_editor_set_text(GET_EDITOR(ntshell), buf);
            } else {
                // 候補がなければ入力補完モードから抜ける。
                SUGGEST_INDEX(ntshell) = -1;
            }
        }
    } else {
        /*
         * 既に入力補完モードに入っている場合、
         * 次の候補を探して見つかればテキストとして設定する。
         */
        SUGGEST_INDEX(ntshell) = SUGGEST_INDEX(ntshell) + 1;
        if (text_history_find(
                    GET_HISTORY(ntshell),
                    SUGGEST_INDEX(ntshell),
                    SUGGEST_SOURCE(ntshell),
                    buf,
                    sizeof(buf)) == 0) {
            // 候補が見つかればテキストを設定する。
            int n = ntlibc_strlen((const char *)buf);
            VTSEND_ERASE_LINE(ntshell);
            VTSEND_CURSOR_HEAD(ntshell);
            PROMPT_WRITE(ntshell);
            SERIAL_WRITE(ntshell, buf, n);
            text_editor_set_text(GET_EDITOR(ntshell), buf);
        } else {
            // 候補が見つからなければ元の入力文字列に戻し、入力補完モードから抜ける。
            int n = ntlibc_strlen(SUGGEST_SOURCE(ntshell));
            VTSEND_ERASE_LINE(ntshell);
            VTSEND_CURSOR_HEAD(ntshell);
            PROMPT_WRITE(ntshell);
            SERIAL_WRITE(ntshell, SUGGEST_SOURCE(ntshell), n);
            text_editor_set_text(GET_EDITOR(ntshell), SUGGEST_SOURCE(ntshell));
            SUGGEST_INDEX(ntshell) = -1;
        }
    }
}

/**
 * @brief カーソルを行頭へ移動させる。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_cursor_head(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    VTSEND_CURSOR_HEAD(ntshell);
    PROMPT_WRITE(ntshell);
    text_editor_cursor_head(GET_EDITOR(ntshell));
}

/**
 * @brief カーソルを行末へ移動させる。
 * @details
 * 論理上のテキスト編集装置内でのテキスト編集と編集結果をビューの更新を行なう。
 * text_editorは論理上のテキスト編集装置であり、ビューに関して一切感知しない。
 *
 * @param ntshell ハンドラ。
 * @param action アクション。
 * @param ch 入力文字。
 */
static void actfunc_cursor_tail(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch)
{
    char buf[TEXTEDITOR_MAXLEN];
    int len;
    UNUSED_VARIABLE(action);
    UNUSED_VARIABLE(ch);
    text_editor_get_text(GET_EDITOR(ntshell), buf, sizeof(buf));
    len = ntlibc_strlen((const char *)buf);
    VTSEND_CURSOR_HEAD(ntshell);
    PROMPT_WRITE(ntshell);
    SERIAL_WRITE(ntshell, buf, len);
    text_editor_cursor_tail(GET_EDITOR(ntshell));
}

/**
 * @brief アクションテーブルのデータ構造体。
 * @details
 * アクションは状態と入力文字によって与えられる。
 * アクションに対する関数もここで定義する。
 */
typedef struct {
    vtrecv_action_t action;
    unsigned char ch;
    void (*func)(ntshell_t *ntshell, vtrecv_action_t action, unsigned char ch);
} ntshell_action_table_t;

/**
 * @brief アクションに対する処理関数テーブル。
 * @details
 * やってくるコードは仮想端末側の処理に依存する。
 * よって様々なプラットフォームの様々な仮想端末で試すと良い。
 *
 * <table>
 *   <th>
 *     <td>Platform</td>
 *     <td>Tools</td>
 *   </th>
 *   <tr>
 *     <td>Windows</td>
 *     <td>Hyper Terminal, Poderossa, TeraTerm</td>
 *   </tr>
 *   <tr>
 *     <td>Linux</td>
 *     <td>minicom, screen, kermit</td>
 *   </tr>
 * </table>
 */
static const ntshell_action_table_t action_table[] = {
    {VTRECV_ACTION_EXECUTE, 0x01, actfunc_cursor_head},
    {VTRECV_ACTION_EXECUTE, 0x02, actfunc_cursor_left},
    {VTRECV_ACTION_EXECUTE, 0x03, actfunc_cancel},
    {VTRECV_ACTION_EXECUTE, 0x04, actfunc_delete},
    {VTRECV_ACTION_EXECUTE, 0x05, actfunc_cursor_tail},
    {VTRECV_ACTION_EXECUTE, 0x06, actfunc_cursor_right},
    {VTRECV_ACTION_EXECUTE, 0x08, actfunc_backspace},
    {VTRECV_ACTION_EXECUTE, 0x09, actfunc_suggest},
    {VTRECV_ACTION_EXECUTE, 0x0d, actfunc_enter},
    {VTRECV_ACTION_EXECUTE, 0x0e, actfunc_history_next},
    {VTRECV_ACTION_EXECUTE, 0x10, actfunc_history_prev},
    {VTRECV_ACTION_CSI_DISPATCH, 0x41, actfunc_history_prev},
    {VTRECV_ACTION_CSI_DISPATCH, 0x42, actfunc_history_next},
    {VTRECV_ACTION_CSI_DISPATCH, 0x43, actfunc_cursor_right},
    {VTRECV_ACTION_CSI_DISPATCH, 0x44, actfunc_cursor_left},
    {VTRECV_ACTION_CSI_DISPATCH, 0x7e, actfunc_delete},
    {VTRECV_ACTION_PRINT, 0x7f, actfunc_delete},
};

/**
 * @brief パーサーに対するコールバック関数。
 * @details vtrecvモジュールのコールバック関数に従った実装である。
 *
 * @param vtrecv パーサー。
 * @param action アクション。
 * @param ch キャラクタ。
 */
void vtrecv_callback(vtrecv_t *vtrecv, vtrecv_action_t action, unsigned char ch)
{
    ntshell_action_table_t *p;
    int i;
    const int ACTTBLSIZ = sizeof(action_table) / sizeof(action_table[0]);

    /*
     * 制御コードに対する処理はテーブルから探す。
     */
    p = (ntshell_action_table_t *)action_table;
    for (i = 0; i < ACTTBLSIZ; i++) {
        if ((p->action == action) && (p->ch == ch)) {
            p->func(vtrecv->user_data, action, ch);
            return;
        }
        p++;
    }

    /*
     * 通常の文字列は入力として扱う。
     */
    if (VTRECV_ACTION_PRINT == action) {
        actfunc_insert(vtrecv->user_data, action, ch);
        return;
    }

    /*
     * ここに到達する場合、
     * テーブルに含まれない制御コードか、
     * 通常の文字でない事が考えられる。
     *
     * 必要なキー入力に対する動作を加えたい場合、
     * vtrecvによって得られるコードを調べるために
     * ここにテストコードを加える事ができる。
     */
}

/**
 * @brief Natural Tiny Shellを初期化する。
 *
 * @param p NT-Shellハンドラ。
 * @param func_read シリアルリード関数。
 * @param func_write シリアルライト関数。
 * @param func_callback コールバック関数。
 * @param extobj コールバック関数呼び出し時に渡す拡張オブジェクト。
 */
void ntshell_init(ntshell_t *p,
    NTSHELL_SERIAL_READ func_read,
    NTSHELL_SERIAL_WRITE func_write,
    NTSHELL_USER_CALLBACK func_callback,
    void *extobj)
{
    /*
     * vtrecvはユーザデータへのポインタを設定できるようになっている。
     * Natural Tiny Shellはこれを利用してテキストエディタやヒストリ、
     * リード関数やライト関数、コールバック関数を処理の中で使用できる
     * ようにしてある。
     */
    p->func_read = func_read;
    p->func_write = func_write;
    p->func_callback = func_callback;
    p->extobj = extobj;
    ntlibc_strcpy(p->prompt, NTSHELL_PROMPT_DEFAULT);

    p->vtrecv.user_data = p;

    /*
     * 各モジュールを初期化する。
     */
    vtsend_init(&(p->vtsend), func_write, p->extobj);
    vtrecv_init(&(p->vtrecv), vtrecv_callback);
    text_editor_init(GET_EDITOR(p));
    text_history_init(GET_HISTORY(p));
    SUGGEST_INDEX(p) = -1;

    /*
     * 初期化確認コードを設定する。
     */
    p->initcode = INITCODE;
}

/**
 * @brief Natural Tiny Shellを実行する。
 * @details この関数は実行を返さない。
 *
 * @param p NT-Shellハンドラ。
 */
void ntshell_execute(ntshell_t *p)
{
    /*
     * 初期化確認コードを検証する。
     */
    if (p->initcode != INITCODE) {
      return;
    }

    /*
     * ユーザ入力ループ。
     */
    PROMPT_WRITE(p);
    while (1) {
        unsigned char ch;
        SERIAL_READ(p, (char *)&ch, sizeof(ch));
        vtrecv_execute(&(p->vtrecv), &ch, sizeof(ch));
    }
}

/**
 * @brief Natural Tiny Shellのプロンプトを設定する。
 */
void ntshell_set_prompt(ntshell_t *p, const char *prompt)
{
    /*
     * 初期化確認コードを検証する。
     */
    if (p->initcode != INITCODE) {
      return;
    }

    ntlibc_strcpy(p->prompt, prompt);
}

/**
 * @brief Natural Tiny Shellのバージョンを返す。
 * @details 返すバージョンはリリースバージョンである。
 *
 * @param major メージャーバージョン。
 * @param minor マイナーバージョン。
 * @param release リリースバージョン。
 */
void ntshell_version(int *major, int *minor, int *release)
{
    *major = VERSION_MAJOR;
    *minor = VERSION_MINOR;
    *release = VERSION_RELEASE;
}

