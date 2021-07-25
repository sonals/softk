// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2021 Xilinx, Inc. All rights reserved.
 * Author: sonal.santan@xilinx.com
 *
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <tuple>
#include <map>
#include <vector>

#include <libelf.h>
#include <dwarf.h>
#include <elfutils/libdw.h>
#include <ffi.h>

#include "skutils.h"

enum enumtest {
    ENUM1,
    ENUM2,
    ENUM3
};

extern "C" {
__attribute__((visibility("default"))) int foo(char *bar, double baz)
{
    return std::strlen(bar) + (int)baz;
}

__attribute__((visibility("default"))) int zoo(char zar, short dar)
{
    return zar + dar;
}

__attribute__((visibility("default"))) int doo(enumtest ear, void *var)
{
    return ear;
}

__attribute__((visibility("default"))) int poo(long &tear, float fear)
{
    return fear;
}
}

namespace sk {
static const std::map<std::pair<Dwarf_Word, Dwarf_Word>, ffi_type*> typeTable = {
    std::make_pair(std::make_pair(DW_ATE_unsigned_char, 1), &ffi_type_uint8),
    std::make_pair(std::make_pair(DW_ATE_signed_char, 1), &ffi_type_sint8),
    std::make_pair(std::make_pair(DW_ATE_unsigned, 2), &ffi_type_uint16),
    std::make_pair(std::make_pair(DW_ATE_signed, 2), &ffi_type_sint16),
    std::make_pair(std::make_pair(DW_ATE_unsigned, 4), &ffi_type_uint32),
    std::make_pair(std::make_pair(DW_ATE_signed, 4), &ffi_type_sint32),
    std::make_pair(std::make_pair(DW_ATE_unsigned, 8), &ffi_type_uint64),
    std::make_pair(std::make_pair(DW_ATE_signed, 8), &ffi_type_sint64),
    std::make_pair(std::make_pair(DW_ATE_float, 4), &ffi_type_float),
    std::make_pair(std::make_pair(DW_ATE_float, 8), &ffi_type_double)
};

static ffi_type *qualifyBaseType(const std::pair<Dwarf_Word, Dwarf_Word> &code)
{
    auto iter = typeTable.find(code);
    return (iter == typeTable.end()) ? nullptr : iter->second;
}

static ffi_type *analyzeBaseType(Dwarf_Die *die)
{
    Dwarf_Attribute temp;
    Dwarf_Word type;

    if (!dwarf_attr(die, DW_AT_encoding, &temp))
        return nullptr;

    if (dwarf_formudata(&temp, &type))
        return nullptr;

    //std::cout << type << std::endl;

    Dwarf_Attribute temp2;
    Dwarf_Word size;
    if (!dwarf_attr(die, DW_AT_byte_size, &temp2))
        return nullptr;

    if (dwarf_formudata(&temp2, &size))
        return nullptr;

    //std::cout << size << std::endl;
    return qualifyBaseType(std::make_pair(type, size));
}


static ffi_type *processType(Dwarf_Attribute *attr)
{
    Dwarf_Die die;
    dwarf_formref_die(attr, &die);

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
            ffi_type *ftype = processType(&attrt);
            (void)ftype;
        }
//        std::cout << " *" << std::endl;
        return &ffi_type_pointer;
        break;
    }
    case DW_TAG_base_type:
    {
        return analyzeBaseType(&die);
    }
    case DW_TAG_enumeration_type:
    {
        return &ffi_type_sint32;
    }
    case DW_TAG_reference_type:
    case DW_TAG_ptr_to_member_type:
    case DW_TAG_structure_type:
    case DW_TAG_class_type:
    case DW_TAG_union_type:
    case DW_TAG_array_type:
    default:
        break;
    }
    return nullptr;
}

static int processArgs(Dwarf_Die *die, sk::argTypes &typeTable)
{
    switch (dwarf_tag (die)) {
    case DW_TAG_formal_parameter:
    {
        const char *aname = dwarf_diename(die);
        if (!aname)
            aname = "??";
//        std::cout << aname << std::endl;
        Dwarf_Attribute attrt;

        if (!dwarf_attr_integrate(die, DW_AT_type, &attrt))
            typeTable.push_back(std::make_pair(aname, nullptr));
        else
            typeTable.push_back(std::make_pair(aname, processType(&attrt)));
        break;
    }
    default:
        break;
    }
    Dwarf_Die sibling;
    if (dwarf_siblingof(die, &sibling))
        return 0;
    return processArgs(&sibling, typeTable);
}

int processFunction(Dwarf_Die *die, void *ctx)
{
    std::vector<sk::signature> *sigs = (std::vector<sk::signature> *)ctx;
    assert(dwarf_tag(die) == DW_TAG_subprogram);
    const char *fname = dwarf_diename(die);

    /* We only care for exported symbols */
    Dwarf_Attribute attrv;
    if (!dwarf_attr(die, DW_AT_external, &attrv))
        return 0;

    /* Attribute external is present, check if DW_FORM_flag_present flag is defined */
    bool val = false;
    if (dwarf_formflag(&attrv, &val))
        return 0;

    if (!val)
        return 0;

    /* Symbols should have int return type */
    Dwarf_Attribute attrt;
    if (!dwarf_attr_integrate(die, DW_AT_type, &attrt))
        return 0;

    if (processType(&attrt) != &ffi_type_sint32)
        return 0;

    /* Identify and ignore C++ symbols -- they have a linkage name */
    Dwarf_Attribute attrl;
    if (dwarf_attr(die, DW_AT_linkage_name, &attrl))
        return 0;

    /* Go ahead and record this symbol */
    sigs->emplace_back(fname);
    Dwarf_Die child;
    /* No child means no function arguments */
    if (dwarf_child(die, &child) != 0)
        return 0;
    processArgs(&child, sigs->back().args);
    return 0;
}
}

std::vector<sk::signature> walk(const char *buffer, size_t size)
{
    std::vector<sk::signature> sigs;
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
        dwarf_getfuncs(cudie, &sk::processFunction, &sigs, 0);
    }
    dwarf_end(dw);
    elf_end(ehandle);
    return sigs;
}
