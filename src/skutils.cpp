// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2021 Xilinx, Inc. All rights reserved.
 * Author: sonal.santan@xilinx.com
 *
 */

#include "skutils.h"

namespace sk {
    static const char *typeName[] = {
        /*FFI_TYPE_VOID*/       "void",
        /*FFI_TYPE_INT*/        "int",
        /*FFI_TYPE_FLOAT*/      "float",
        /*FFI_TYPE_DOUBLE*/     "double",
        /*FFI_TYPE_LONGDOUBLE*/ "??",
        /*FFI_TYPE_UINT8*/      "uint8_t",
        /*FFI_TYPE_SINT8*/      "int8_t",
        /*FFI_TYPE_UINT16*/     "uint16_t",
        /*FFI_TYPE_SINT16*/     "int16_t",
        /*FFI_TYPE_UINT32*/     "unsigned int",
        /*FFI_TYPE_SINT32*/     "int",
        /*FFI_TYPE_UINT64*/     "uint64_t",
        /*FFI_TYPE_SINT64*/     "int64_t",
        /*FFI_TYPE_STRUCT*/     "??",
        /*FFI_TYPE_POINTER*/    "*",
        /*FFI_TYPE_COMPLEX*/    "??",
    };

    std::ostream &signature::dump(std::ostream &out)
    {
        out << "----\n";
        out << name << "(";
        const char *sep = "";
        for (auto arg : args) {
            out << sep;
            out << (arg.second ? typeName[arg.second->type] : "??");
            out << " ";
            out << (arg.first ? arg.first : "??");
            sep = ", ";
        }
        sep = ")";
        out << sep << "\n----" << std::endl;
        return out;
    }
}
