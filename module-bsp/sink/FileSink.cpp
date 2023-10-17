// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "sink/FileSink.hpp"

FileSink::FileSink(const std::string &filepath)
{
    fs.open(filepath, std::fstream::out);
}

FileSink::~FileSink()
{
    fs.close();
}

void FileSink::put(const char *data, size_t size)
{
    fs.write(data, size);
}
