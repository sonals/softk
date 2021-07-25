
..
   comment:: SPDX-License-Identifier: Apache-2.0
   comment:: Copyright (C) 2019-2021 Xilinx, Inc. All rights reserved.

=================================================================
Dynamic library C API signature extraction for dynamic invocation
=================================================================

This library extracts function signatures of exported *C functions* in an ELF
file and creates a table of function to an ``ffi_type`` array describing the
function parameters. For signature extraction, this library requires the input
ELF to be compiled with DWARF information generated with ``gcc -g`` switch. Note
-g should be used even when the input ELF is compliled with optimization like
-O2 or -O3.

Dependencies
************
1. libdw
2. libelf
3. libffi

Build
*****

Release
=======

.. code-block:: bash

   mkdir -p build/Release
   cd build/Release
   cmake -DCMAKE_BUILD_TYPE=Release ../../src/
   make
   make test

Debug
=====

.. code-block:: bash

   mkdir -p build/Debug
   cd build/Debug
   cmake -DCMAKE_BUILD_TYPE=Debug ../../src/
   make
   make test


Usage
*****

::

   ./sk <elf_file_with_debug_symbols>

Example Session
===============

::

   xsjsonalsmbp:~/<1>softk/build/Debug>./sk ./sk
   Using shared object file: ./sk
   ----
   main(int argc, * argv)
   ----
   ----
   poo(?? tear, float fear)
   ----
   ----
   doo(int ear, * var)
   ----
   ----
   zoo(int8_t zar, int16_t dar)
   ----
   ----
   foo(* bar, double baz)
   ----
   xsjsonalsmbp:~/<1>softk/build/Debug>

TODO
****

Add infrastructure to the library to ``dlopen`` an input ELF, enumerate symbols
using generated signal table, find a given function's entry point using
``dlsym`` and then invoke the function after dynamically assembling a call frame
using user provided function args.
