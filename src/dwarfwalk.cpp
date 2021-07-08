// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2021 sonal.santan@gmail.com
 *
 */

#include <iostream>

#include <libelf.h>
#include <dwarf.h>
#include <elfutils/libdw.h>


int walk(const char *buffer, size_t size)
{
    Elf *ehandle = elf_memory(const_cast<char *>(buffer), size);
    // Example code snippet from elfutils/debuginfod/debuginfod.cxx
    Dwarf *dw = dwarf_begin_elf(ehandle, DWARF_C_READ, nullptr);
    Dwarf_Off offset = 0;
    Dwarf_Off old_offset;
    size_t hsize;

    while (dwarf_nextcu (dw, old_offset = offset, &offset, &hsize, NULL, NULL, NULL) == 0)
    {
        Dwarf_Die cudie_mem;
        Dwarf_Die *cudie = dwarf_offdie (dw, old_offset + hsize, &cudie_mem);
        std::cout << cudie << std::endl;
        if (cudie == NULL)
            continue;
        if (dwarf_tag (cudie) != DW_TAG_compile_unit)
            continue;
        const char *cuname = dwarf_diename(cudie) ?: "unknown";
        std::cout << cuname << std::endl;
    }
    dwarf_end(dw);
    return 0;
}
