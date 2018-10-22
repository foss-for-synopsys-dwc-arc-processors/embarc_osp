.. _lib_c_lib:

C Library Support
#################

Provides support for file-related APIs using FatFs, **printf()** support using a
UART, and time-related APIs. For details, please refer to the source code in
``library/clib``. The C library support implemented in embARC OSP is
compatible with both the GNU toolchain for ARC Processors and the MetaWare
Development Toolkit C library implementations (each included in their
respective toolchain package). More information on each C library is included
below.

ARC GNU Newlib
==============

Newlib is a C standard library implementation intended for use on embedded
systems. It is a conglomeration of several library parts, all under free
software licenses that make them easily usable on embedded products.

It was created by Cygnus Support as part of building the first GNU cross-
development toolchains. It is now maintained by Red Hat developers Jeff
Johnston and Corinna Vinschen, and is used in most commercial and non-
commercial GCC ports for non-Linux embedded systems.

MetaWare C Run-Time Library
===========================

The MetaWare C Run-Time Library is a collection of functions, macros, and type
declarations organized into header files according to ANSI Standard C Library
areas, such as mathematics and string handling. The MetaWare C Run-Time
Library is an ANSI Standard C library with extensions. The macros, function
names and parameters, and types are those dictated by the ANSI C Standard. The
library conforms to the ANSI C Standard (ANSI document X3J11/90-014); the
single-precision math functions conform to the ISO/ANSI C99 Standard. The
library also provides additional functions not described in the ANSI C
Standard.
