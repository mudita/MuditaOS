// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <vfs.hpp>
#include <time/time_conversion.hpp>
#include <random>
#include <ticks.hpp>
#include <source/version.hpp>

#include <purefs/filesystem_paths.hpp>


std::string vfs::loadFileAsString(const fs::path &fileToLoad)
{
    auto lamb = [](vfs::FILE *stream) { ::vfs.fclose(stream); };
    std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
    std::unique_ptr<vfs::FILE, decltype(lamb)> fp(fopen(fileToLoad.c_str(), "r"), lamb);
    std::string contents;
    size_t readSize;

    if (fp.get() != nullptr) {
        while (!eof(fp.get())) {
            readSize = fread(readBuf.get(), 1, purefs::buffer::tar_buf, fp.get());
            contents.append(static_cast<const char *>(readBuf.get()), readSize);
        }
    }

    return contents;
}

bool vfs::replaceWithString(const fs::path &fileToModify, const std::string &stringToWrite)
{
    auto lamb = [](vfs::FILE *stream) { ::vfs.fclose(stream); };
    std::unique_ptr<vfs::FILE, decltype(lamb)> fp(::vfs.fopen(fileToModify.c_str(), "w"), lamb);

    if (fp.get() != nullptr) {
        size_t dataWritten = fprintf(fp.get(), stringToWrite.c_str());
        return dataWritten == stringToWrite.length();
    }
    else {
        return false;
    }
}

