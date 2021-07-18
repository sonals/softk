// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2021 Xilinx, Inc. All rights reserved.
 * Author: sonal.santan@xilinx.com
 *
 */

#include <iostream>
#include <cassert>

#include <libelf.h>
#include <dwarf.h>
#include <elfutils/libdw.h>

static int processLocation(Dwarf_Die *die)
{
    return 0;
}

static int processType(Dwarf_Attribute *attr)
{
    Dwarf_Die die;
    dwarf_formref_die(attr, &die);
    const char *tname = dwarf_diename(&die);
    if (tname)
        std::cout << tname << std::endl;

    switch (dwarf_tag(&die)) {
    case DW_TAG_typedef:
    case DW_TAG_const_type:
    case DW_TAG_volatile_type:
    case DW_TAG_restrict_type:
    {
        Dwarf_Attribute attr;
        if (dwarf_attr_integrate (&die, DW_AT_type, &attr))
            return processType(&attr);
        break;
    }
    case DW_TAG_pointer_type:
    {
        if (dwarf_hasattr(&die, DW_AT_type)) {
            Dwarf_Attribute attrt;
            dwarf_attr(&die, DW_AT_type, &attrt);
            processType(&attrt);
        }
        std::cout << " *" << std::endl;
        break;
    }
    case DW_TAG_reference_type:
    {
        if (dwarf_hasattr(&die, DW_AT_type)) {
            Dwarf_Attribute attrt;
            dwarf_attr(&die, DW_AT_type, &attrt);
            processType(&attrt);
        }
        std::cout << " &" << std::endl;
        break;
    }
    case DW_TAG_base_type:
    case DW_TAG_enumeration_type:
    case DW_TAG_ptr_to_member_type:
    case DW_TAG_structure_type:
    case DW_TAG_class_type:
    case DW_TAG_union_type:
    case DW_TAG_array_type:
    default:
//        std::cout << dwarf_tag(&die) << std::endl;
        break;
    }
    return 0;
}

static int processArgs(Dwarf_Die *die)
{
    switch (dwarf_tag (die)) {
    case DW_TAG_formal_parameter:
    {
        const char *aname = dwarf_diename(die);
        std::cout << aname << std::endl;
        Dwarf_Attribute attrt;
        if (!dwarf_attr_integrate(die, DW_AT_type, &attrt))
            return 0;
        processType(&attrt);
        break;
    }
    default:
        break;
    }
    Dwarf_Die sibling;
    if (dwarf_siblingof(die, &sibling))
        return 0;
    return processArgs(&sibling);
}

static int processFunction(Dwarf_Die *die, void *ctx)
{
    Elf *elf = (Elf *)ctx;
    assert(dwarf_tag(die) == DW_TAG_subprogram);
    const char *fname = dwarf_diename(die);
    std::cout << fname << std::endl;

    Dwarf_Attribute attrt;
    if (!dwarf_attr_integrate(die, DW_AT_type, &attrt))
        return 0;

    processType(&attrt);
    Dwarf_Attribute attrv;
    if (!dwarf_attr(die, DW_AT_external, &attrv))
        return 0;

    bool val = false;
    if (dwarf_formflag(&attrv, &val))
        return 0;

    if (!val)
        return 0;

    Dwarf_Die child;
    if (dwarf_child(die, &child) != 0)
        return 0;

    std::cout << '[' << std::endl;
    processArgs(&child);
    std::cout << ']' << std::endl;
    std::cout.flush();
    const char *name = dwarf_formstring(&attrt);
    if (name)
        std::cout << name << std::endl;
//    int ret_size = mips_arg_size(elf, die, &attr);
    return 0;
}

int walk(const char *buffer, size_t size)
{
    Elf *ehandle = elf_memory(const_cast<char *>(buffer), size);
    // Example code snippet from elfutils/debuginfod/debuginfod.cxx
    Dwarf *dw = dwarf_begin_elf(ehandle, DWARF_C_READ, nullptr);
    Dwarf_Off offset = 0;
    Dwarf_Off old_offset;
    size_t hsize;

    while (dwarf_nextcu (dw, old_offset = offset, &offset, &hsize, nullptr, nullptr, nullptr) == 0)
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
        dwarf_getfuncs(cudie, &processFunction, ehandle, 0);
    }
    dwarf_end(dw);
    return 0;
}
