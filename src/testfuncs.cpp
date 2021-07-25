// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2021 Xilinx, Inc. All rights reserved.
 * Author: sonal.santan@xilinx.com
 *
 */

#include <cstring>

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
int qoo(long &tear, float fear)
{
    return fear;
}
