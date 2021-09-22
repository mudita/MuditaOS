// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <filesystem>

struct __dirstream;

namespace utils::benchmark
{

    class FileSystemBenchmark
    {
        static constexpr auto fileSize = 4U * 1024U * 1024U;

      public:
        enum returnCodes //! Return codes from single step
        {
            retContinue = 1, //! Single step continue
            retFinished = 0  //! Single step finish
        };

        enum class testName //! Current processed test
        {
            prepareEnv, //! Prepare test environment
            writeTest,  //! Create files with random data test
            readTest,   //! Read files tests
            rmTest,     //! Remove files test
            dirCreate,  //! Create directories test
            dirIterate, //! Directory iterate test
            cleanupEnv, //! Delete all test
            finished    //! Finished or error
        };

        //! Current test status
        struct status
        {
            int step{};
            int numSteps{1};
            int64_t time_ms{};
            testName currentTest{};
        };

        FileSystemBenchmark(FileSystemBenchmark &) = delete;
        FileSystemBenchmark &operator=(FileSystemBenchmark &) = delete;
        explicit FileSystemBenchmark(int num_files);

        // !Get info
        auto &getInfo() const noexcept
        {
            return testStatus;
        }

        /** Process single step
         * @return see @enum returnCodes
         */
        int step() noexcept;

      private:
        int prepare() noexcept;
        int fileWrite() noexcept;
        int fileRead() noexcept;
        int dirCreate() noexcept;
        int dirIterate() noexcept;
        int cleanup() noexcept;
        int remove() noexcept;

      private:
        status testStatus;
        const int numSteps;
        const std::filesystem::path testDir;
        std::string memBuf;
        __dirstream *dir{};
        int64_t time_ms{};
    };

    inline FileSystemBenchmark::testName &operator++(FileSystemBenchmark::testName &inp)
    {
        inp = static_cast<FileSystemBenchmark::testName>(static_cast<int>(inp) + 1);
        return inp;
    }
} // namespace utils::benchmark
