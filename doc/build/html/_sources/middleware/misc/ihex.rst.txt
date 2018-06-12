.. _middleware_ihex:

iHex
####

Intel HEX(IHEX) is a file format that conveys binary information in ASCII text
form. It is commonly used for programming microcontrollers, EPROMs, and other
types of programmable logic devices. In a typical application, a compiler or
assembler converts a program's source code (such as in C or assembly language)
to machine code and outputs it into a HEX file. The HEX file is then imported
by a programmer to "burn" the machine code into a ROM, or is transferred to
the target system for loading and execution.

ihex is a free Intel HEX format parser implementation.
