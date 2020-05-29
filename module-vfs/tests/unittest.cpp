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
#include <ticks.hpp>

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
    auto dirList = vfs.listdir("module-vfs/test_dir");
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

#define RANDDOM_TESTS 128
TEST_CASE("Random strings")
{
    vfs.Init();

    std::vector<std::string> randomIds8, randomIds16, randomIds32;

    for (int i = 0; i < RANDDOM_TESTS; i++)
        randomIds8.push_back(vfs.generateRandomId(8));

    for (int i = 0; i < RANDDOM_TESTS; i++)
        randomIds16.push_back(vfs.generateRandomId(16));

    for (int i = 0; i < RANDDOM_TESTS; i++)
        randomIds32.push_back(vfs.generateRandomId(32));

    for (int k = 0; k < RANDDOM_TESTS; k++) {
        for (int i = 0; i < RANDDOM_TESTS; i++) {
            if (k == i)
                continue;

            REQUIRE(randomIds8[k] != randomIds8[i]);
            REQUIRE(randomIds16[k] != randomIds16[i]);
            REQUIRE(randomIds32[k] != randomIds32[i]);
        }
    }
}

TEST_CASE("CRC32 tests")
{
    vfs.Init();
    unsigned long crc32 = 0;
    char crcBuf[purefs::buffer::crc_char_size];
    std::string randomData = vfs.generateRandomId(128);
    auto fd                = vfs.fopen("testFile.txt", "w");
    REQUIRE(fd != nullptr);

    auto bytesWritten = vfs.fwrite(randomData.c_str(), 1, randomData.size(), fd);
    REQUIRE(bytesWritten == randomData.size());
    REQUIRE(vfs.fclose(fd) == 0);

    fd = vfs.fopen("testFile.txt", "r");
    vfs.computeCRC32(fd, &crc32);
    sprintf(crcBuf, "%lX", crc32);
    auto fdCRC   = vfs.fopen("testFile.txt.crc32", "w");
    bytesWritten = vfs.fwrite(&crcBuf, 1, purefs::buffer::crc_char_size, fdCRC);
    REQUIRE(bytesWritten == purefs::buffer::crc_char_size);
    REQUIRE(vfs.fclose(fdCRC) == 0);

    REQUIRE(vfs.verifyCRC("testFile.txt") == true);
    REQUIRE(vfs.remove("testFile.txt") == 0);
    REQUIRE(vfs.remove("testFile.txt.crc32") == 0);
}
