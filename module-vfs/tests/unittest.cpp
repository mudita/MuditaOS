/*
 * @file unittests_vfs.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "vfs.hpp"

#include "catch.hpp"

#include <stdint.h>
#include <algorithm>

#include <iostream>

class vfs vfs;

TEST_CASE("Test case 1")
{

    vfs.Init();

    const size_t testBufferSize = 1024 * 1024;

    uint8_t testBuffer[testBufferSize] = {0};

    auto fd = vfs.fopen("testFile.txt", "w");
    REQUIRE(fd != nullptr);

    auto bytesWritten = vfs.fwrite(testBuffer, 1, testBufferSize, fd);
    REQUIRE(bytesWritten == testBufferSize);

    auto currFilePos = vfs.ftell(fd);
    REQUIRE(currFilePos == testBufferSize);

    auto fileSize = vfs.filelength(fd);
    REQUIRE(fileSize == testBufferSize);

    currFilePos = vfs.ftell(fd);
    REQUIRE(currFilePos == testBufferSize);

    REQUIRE(vfs.fclose(fd) == 0);

    // current directory is the build dir
    // vfs adds sys/ to the path we need to got outside sys (bad!)
    // and look for some files there
    auto dirList = vfs.listdir("../module-vfs/test_dir");
    REQUIRE(dirList.size() == 3);
    for (auto &dir : dirList) {
        if (dir.fileName == "test_dir2") {
            REQUIRE(dir.attributes == vfs::FileAttributes::Directory);
        }
        if (dir.fileName == "test1.txt" || dir.fileName == "test2.txt") {
            REQUIRE(dir.attributes == vfs::FileAttributes::Writable);
        }
    }
}
