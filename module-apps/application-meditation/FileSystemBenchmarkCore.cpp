// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FileSystemBenchmarkCore.hpp"
#include <purefs/filesystem_paths.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>
#include <log.hpp>
#include <chrono>

namespace utils::benchmark
{
    FileSystemBenchmark::FileSystemBenchmark(int num_files)
        : numSteps(num_files), testDir(purefs::dir::getUserDiskPath() / "perfTest"), memBuf(65535U, 'x')
    {}

    //! Test main loop
    int FileSystemBenchmark::step() noexcept
    {
        int ret{retFinished};
        const auto t1 = std::chrono::high_resolution_clock::now();
        switch (testStatus.currentTest) {
        case testName::prepareEnv:
            ret = prepare();
            break;
        case testName::writeTest:
            ret = fileWrite();
            break;
        case testName::readTest:
            ret = fileRead();
            break;
        case testName::rmTest:
            ret = remove();
            break;
        case testName::dirCreate:
            ret = dirCreate();
            break;
        case testName::dirIterate:
            ret = dirIterate();
            break;
        case testName::cleanupEnv:
            ret = cleanup();
            break;
        case testName::finished:
            break;
        }
        const auto t2 = std::chrono::high_resolution_clock::now();
        time_ms += std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        if (testStatus.currentTest != testName::finished) {
            if (ret == retFinished) {

                testStatus.step     = 0;
                testStatus.time_ms  = time_ms;
                time_ms             = 0;
                ret                 = retContinue;
                testStatus.numSteps = numSteps;
                ++testStatus.currentTest;
            }
            else if (ret == retContinue) {
                ++testStatus.step;
            }
            else {
                testStatus.currentTest = testName::finished;
            }
        }
        return ret;
    }

    //! Prepare directory structure
    int FileSystemBenchmark::prepare() noexcept
    {
        const auto ret = ::mkdir(testDir.c_str(), 0770);
        return (ret) ? (-errno) : (retFinished);
    }

    //! Write files
    int FileSystemBenchmark::fileWrite() noexcept
    {
        using namespace std::string_literals;
        const auto file = testDir / ("filetest"s + std::to_string(testStatus.step));
        if (testStatus.step < testStatus.numSteps) {
            auto fil = std::fopen(file.c_str(), "w");
            if (!fil) {
                return -errno;
            }
            for (size_t n = 0; n < fileSize / memBuf.size(); ++n) {
                if (std::fwrite(memBuf.data(), memBuf.size(), 1, fil) != 1) {
                    std::fclose(fil);
                    return -errno;
                }
            }
            std::fclose(fil);
            return retContinue;
        }
        else {
            return retFinished;
        }
    }

    //! Read files
    int FileSystemBenchmark::fileRead() noexcept
    {
        using namespace std::string_literals;
        const auto file = testDir / ("filetest"s + std::to_string(testStatus.step));
        if (testStatus.step < testStatus.numSteps) {
            auto fil = std::fopen(file.c_str(), "r");
            if (!fil) {
                return -errno;
            }
            for (size_t n = 0; n < fileSize / memBuf.size(); ++n) {
                if (std::fread(memBuf.data(), memBuf.size(), 1, fil) != 1) {
                    std::fclose(fil);
                    return -errno;
                }
            }
            std::fclose(fil);
            return retContinue;
        }
        else {
            return retFinished;
        }
    }

    //! Remove regular files
    int FileSystemBenchmark::remove() noexcept
    {
        using namespace std::string_literals;
        const auto file = testDir / ("filetest"s + std::to_string(testStatus.step));
        if (testStatus.step < testStatus.numSteps) {
            const auto ret = ::unlink(file.c_str());
            return (ret) ? (-errno) : (retContinue);
        }
        else {
            return retFinished;
        }
    }

    //! Create directories
    int FileSystemBenchmark::dirCreate() noexcept
    {
        using namespace std::string_literals;
        const auto dir = testDir / ("dirtest"s + std::to_string(testStatus.step));
        if (testStatus.step < testStatus.numSteps) {
            const auto ret = ::mkdir(dir.c_str(), 0770);
            return (ret) ? (-errno) : (retContinue);
        }
        else {
            return retFinished;
        }
    }
    //! Iterate directories
    int FileSystemBenchmark::dirIterate() noexcept
    {
        if (testStatus.step < testStatus.numSteps) {
            if (!dir) {
                dir = ::opendir(testDir.c_str());
                if (!dir) {
                    return -errno;
                }
            }
            dirent *dentry;
            if ((dentry = readdir(dir))) {
                return retContinue;
            }
            else {
                ::closedir(dir);
                dir = nullptr;
                return -errno;
            }
        }
        else {
            ::closedir(dir);
            dir = nullptr;
            return retFinished;
        }
    }
    //! Cleanup directories
    int FileSystemBenchmark::cleanup() noexcept
    {
        using namespace std::string_literals;
        const auto dir = testDir / ("dirtest"s + std::to_string(testStatus.step));
        if (testStatus.step < testStatus.numSteps) {
            const auto ret = ::unlink(dir.c_str());
            return (ret) ? (-errno) : (retContinue);
        }
        else {
            const auto ret = ::unlink(testDir.c_str());
            return (ret) ? (-errno) : (retFinished);
        }
    }
} // namespace utils::benchmark
