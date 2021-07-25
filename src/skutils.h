// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2021 Xilinx, Inc. All rights reserved.
 * Author: sonal.santan@xilinx.com
 *
 */

#ifndef _SOFTK_UTILS_H_
#define _SOFTK_UTILS_H_

#include <string>
#include <vector>
#include <tuple>
#include <ostream>

#include <ffi.h>

namespace sk {
    typedef std::vector<std::pair<const char *, ffi_type *>> argTypes;
    struct signature {
        std::string name;
        argTypes args;
        signature(const char *_name) : name(_name) {}
        std::ostream &dump(std::ostream &out) const;
    };

    const std::vector<sk::signature> walk(const char *buffer, size_t size);
}

#endif
