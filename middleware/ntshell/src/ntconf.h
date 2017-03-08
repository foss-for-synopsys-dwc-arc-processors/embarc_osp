/**
 * @file ntconf.h
 * @author Shinichiro Nakamura
 * @brief NT-Shellのコンフィギュレーション。
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

#ifndef NTCONF_H
#define NTCONF_H

/**
 * @note
 * ここでは、NT-Shellの内部実装で使用する共通の定義を行なう。
 * NT-Shell内部では、複数のモジュールが互いに緩い依存関係を持つ。
 * これはNT-Shellの最上位モジュールであるntshellが各モジュールを
 * 使って機能を実現するためである。
 */

/**
 * @brief 1行あたりの最大文字数。
 */
#ifndef NTCONF_EDITOR_MAXLEN
#define NTCONF_EDITOR_MAXLEN    (64)
#endif

/**
 * @brief テキストヒストリ数。
 */
#ifndef NTCONF_HISTORY_DEPTH
#define NTCONF_HISTORY_DEPTH    (8)
#endif

#endif

