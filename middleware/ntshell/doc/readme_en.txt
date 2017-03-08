===============================================================
 Natural Tiny Shell - A tiny shell for a small embedded system
 Copyright(C) 2011 Shinichiro Nakamura
===============================================================

 Natural Tiny Shell (NT-Shell) is a tiny shell for a small embedded system.

1. Special thanks

 Mr.Paul Williams
 - He created a draw of DEC compatible state machine parser.

 Mr.Joshua Haberman
 - He implemented the state machine parser with C language.
   And it has great interface. It's really simple and smart!

2. The background

 Today, There are so many great MPUs.
 And the developper can design a small embedded system with it.
 It's really great things and I never imagine 20 years ago.

 There is a way to debug a embedded system using UART interface.
 It is same as UNIX terminal interfaces.

 The developper can be control a embedded system with it.

3. The basic concepts

 * Small :)
 * Simple :)
 * Compatible with VT100 :)
 * No need any C runtime library :)

4. Features

 CTRL+A : Head
 CTRL+E : Tail
 CTRL+F : Cursor right
 CTRL+B : Cursor left
 CTRL+C : Cancel
 CTRL+D : Delete
 CTRL+P : History(Prev)
 CTRL+N : History(Next)
 TAB    : Suggestion

5. The structure

 NT-Shell are consists from 6 groups.

 * Top interface module
    ntshell.c
    ntshell.h
 * Utility
    ntopt.c
    ntopt.h
 * VT100 parser
    vtrecv.c
    vtrecv.h
    vtsend.c
    vtsend.h
 * Text editor
    text_editor.c
    text_editor.h
 * Text history
    text_history.c
    text_history.h
 * Common files
    ntlibc.c
    ntlibc.h
    ntconf.h
    ntint.h

6. Porting guide

 The NT-Shell interface is really simple.
 It need two I/O interfaces for UART and a callback interface for getting a user request.

 typedef int (*NTSHELL_SERIAL_READ)(char *buf, int cnt, void *extobj);
 typedef int (*NTSHELL_SERIAL_WRITE)(const char *buf, int cnt, void *extobj);
 typedef int (*NTSHELL_USER_CALLBACK)(const char *text, void *extobj);

 void ntshell_init(ntshell_t *p,
     NTSHELL_SERIAL_READ func_read,
     NTSHELL_SERIAL_WRITE func_write,
     NTSHELL_USER_CALLBACK func_callback,
     void *extobj);

 6-1. p

  The p is a pointer to a ntshell_t structure.

 6-2. func_read

  The func_read is a interface for reading user key inputs.
  It's a blocking interface.

 6-3. func_write

  The func_write is a interface for writing shell outputs.
  It's a blocking interface.

 6-4. func_callback

  The func_callback is a callback interface for user request to your system.

 6-5. func_callback_extobj

  The func_callback_extobj is a pointer to a external object for the
  extobj argument of the callback.

7. The license

 * The license of vtrecv is public domain.
 * The license of ntshell, ntopt, ntlibc, text_editor, text_history are MIT.
   (or You can select TOPPERS license.)

 ===============================================================
 The MIT license for Natural Tiny Shell (NT-Shell)
 ===============================================================
 Copyright (c) 2010-2012 Shinichiro Nakamura

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 ===============================================================

 So you can use this software in non-commercial or commercial products.
 But there aren't any warranty.

 Please do NOT remove copyright texts, if you redistribute this package.

8. References

 * A parser for DEC's ANSI-compatible video terminal
   http://vt100.net/emu/dec_ansi_parser

9. Version

 * 2011/03/01   Version 0.0.1    Initial version released.
 * 2011/03/02   Version 0.0.2    Added a utility module.
 * 2011/03/04   Version 0.0.3    Buf fixed.
 * 2011/04/30   Version 0.0.4    Changed an implementation of the vtparse table.
                                 Added ntopt.
                                 Added ntlibc.
 * 2011/05/16   Version 0.0.5    Added input suggestion features.
                                 Changed the interface of the ntshell.
 * 2011/05/22   Version 0.0.6    Added cursor head and tail feautures.
 * 2011/09/22   Version 0.0.7    Added extobj argument to ntopt interface.
 * 2011/10/30   Version 0.0.8    Changed order of extobj in ntopt interface.
                                 Added extobj argument to ntshell interface.
 * 2012/09/23   Version 0.1.0    Updated an implementation of the vtparse table.
                                 And renamed the vtparse to vtrecv.
                                 Minor changed.
 * 2012/11/04   Version 0.2.0    Added ntshell_set_prompt interface.
                                 Added ntshell_init interface.
                                 Changed ntshell_execute interface.
                                 Bug fixed the 'Delete' key action mapping.

