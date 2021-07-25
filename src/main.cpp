// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2021 Xilinx, Inc. All rights reserved.
 * Author: sonal.santan@xilinx.com
 *
 */

#include <iostream>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>
#include <fstream>

#include "skutils.h"

extern std::vector<sk::signature> walk(const char *buffer, size_t size);

int main(int argc, char *argv[])
{
    std::cout << "Using shared object file: " << argv[1] << std::endl;
    std::ifstream ins(argv[1]);
    ins.seekg (0, ins.end);
    std::streampos length = ins.tellg();
    ins.seekg (0, ins.beg);

    std::unique_ptr<char[]> buffer(new char[length]);
    ins.read(buffer.get(), length);
    ins.close();
    std::vector<sk::signature> sigs = walk(buffer.get(), length);
    for (auto sig : sigs)
        sig.dump(std::cout);
    return 0;
}
