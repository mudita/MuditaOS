// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <rotator/Rotator.hpp>

#include <array>
#include <ctime>
#include <memory>
#include <cstdio>

namespace purefs::fs
{
    class filesystem;
}

namespace crashdump
{
    constexpr inline auto maxRotationFilesCount = 5;
    class CrashDumpWriterVFS
    {
      public:
        CrashDumpWriterVFS() : rotator{".hex"}
        {}
        void openDump();
        void saveDump();

        void writeBytes(const std::uint8_t *buff, std::size_t size);
        void writeHalfWords(const std::uint16_t *buff, std::size_t size);
        void writeWords(const std::uint32_t *buff, std::size_t size);

      private:
        utils::Rotator<maxRotationFilesCount> rotator;
        std::FILE *file{};
    };

} // namespace crashdump
