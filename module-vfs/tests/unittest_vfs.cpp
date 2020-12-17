// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "vfs.hpp"

#include <ticks.hpp>

#include <algorithm>

#include <cstdint>

#include <thread.hpp>

#include <vector>

#include <algorithm>

class vfs vfs;

TEST_CASE("Test vfs case 1")
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
    vfs.mkdir("module-vfs");
    vfs.mkdir("module-vfs/test_dir2");

    fd = vfs.fopen("module-vfs/test1.txt", "a");
    REQUIRE(fd != nullptr);
    REQUIRE(vfs.fclose(fd) == 0);

    fd = vfs.fopen("module-vfs/test2.txt", "a");
    REQUIRE(fd != nullptr);
    REQUIRE(vfs.fclose(fd) == 0);

    auto dirList = vfs.listdir("module-vfs");
    REQUIRE(dirList.size() >= 4);
    for (auto &dir : dirList) {
        if (dir.fileName == "test_dir2") {
            REQUIRE(dir.attributes == vfs::FileAttributes::Directory);
        }
        if (dir.fileName == "test1.txt" || dir.fileName == "test2.txt") {
            REQUIRE(dir.attributes == vfs::FileAttributes::Writable);
        }
    }
}





TEST_CASE("VFS lseek check")
{
    vfs.Init();

    static constexpr auto seek_filename = "lseek_test.fil";
    static constexpr auto buf_elems_size = 1024U;
    static constexpr auto buf_elems      = buf_elems_size / sizeof(int);
    static constexpr auto buf_items     = 1024U;
    auto fd                             = vfs.fopen(seek_filename, "w+");
    REQUIRE(fd != nullptr);
    std::vector<int> buffer(buf_elems);
    for (auto record = 0U; record < buf_items; ++record) {
        std::iota(std::begin(buffer), std::end(buffer), record * buf_elems);
        REQUIRE(vfs.fwrite(buffer.data(), buffer.size() * sizeof(buffer[0]), 1, fd) == 1);
    }
    REQUIRE(vfs.fclose(fd) == 0);
    fd = vfs.fopen(seek_filename, "r");
    REQUIRE(fd != nullptr);
    std::vector<int> buf_out(buf_elems);
    static constexpr auto offs_elems1 = 256U;
    static constexpr auto offs_seek1  = offs_elems1 * sizeof(int);
    REQUIRE(vfs.fseek(fd, offs_seek1, SEEK_SET) == 0);
    REQUIRE(vfs.ftell(fd) == offs_seek1);
    REQUIRE(vfs.fread(buf_out.data(), buf_out.size() * sizeof(buf_out[0]), 1, fd) == 1);
    std::iota(std::begin(buffer), std::end(buffer), offs_elems1);
    REQUIRE(buffer == buf_out);

    REQUIRE(vfs.fseek(fd, 0UL, SEEK_END) == 0);
    REQUIRE(vfs.ftell(fd) == buf_elems_size * buf_items);
    REQUIRE(vfs.fseek(fd, buf_elems_size * buf_items + 10, SEEK_SET) < 0);
    REQUIRE(vfs.ftell(fd) == buf_elems_size * buf_items);

    REQUIRE(vfs.fseek(fd, 0UL, SEEK_SET) == 0);
    REQUIRE(vfs.fread(buf_out.data(), buf_out.size() * sizeof(buf_out[0]), 1, fd) == 1);
    std::iota(std::begin(buffer), std::end(buffer), 0);
    REQUIRE(buffer == buf_out);
    REQUIRE(vfs.fclose(fd) == 0);
}

TEST_CASE("Simple file notifier init")
{
    vfs.Init();

    vfs.registerNotificationHandler(
        [](std::string_view file, vfsn::utility::vfsNotifier::FsEvent ev, std::string_view old_file) {
            using namespace std::string_literals;
            REQUIRE(file == "/"s);
            REQUIRE(ev == vfsn::utility::vfsNotifier::FsEvent::initialized);
            REQUIRE(old_file.empty());
        });
    vfs.registerNotificationHandler(nullptr);
}

TEST_CASE("Simple file notifier write")
{
    vfs.Init();

    vfs.registerNotificationHandler(
        [](std::string_view file, vfsn::utility::vfsNotifier::FsEvent ev, std::string_view old_file) {
            using namespace std::string_literals;
            namespace fs = std::filesystem;
            fs::path fspath(file);
            REQUIRE(fspath.is_absolute());
            REQUIRE(fspath.filename() == "testFileLB.txt"s);
            REQUIRE(ev == vfsn::utility::vfsNotifier::FsEvent::modified);
            REQUIRE(old_file.empty());
        });

    const size_t testBufferSize = 1024 * 1024;

    uint8_t testBuffer[testBufferSize] = {0};

    auto fd = vfs.fopen("testFileLB.txt", "w");
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

    vfs.registerNotificationHandler(nullptr);
}
