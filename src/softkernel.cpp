// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2020 sonal.santan@gmail.com
 *
 */

#include <iostream>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>
#include <fstream>

extern int walk(const char *buffer, size_t size);

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
    walk(buffer.get(), length);
    return 0;
}
