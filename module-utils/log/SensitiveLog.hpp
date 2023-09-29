// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <mutex.hpp>
#include <filesystem>

namespace Log
{
    class SensitiveLog
    {
      public:
        void put(const std::string &item);
        int dumpToFile(const std::filesystem::path &logDirectoryPath);

      private:
        cpp_freertos::MutexStandard mutex;
        bool permissionToLog{false};
    };

} // namespace Log
