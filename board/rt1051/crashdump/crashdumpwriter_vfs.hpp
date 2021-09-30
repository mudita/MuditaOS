// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "crashdumpwriter.hpp"
#include <rotator/Rotator.hpp>

#include <array>
#include <ctime>
#include <memory>

namespace purefs::fs
{
    class filesystem;
}

namespace crashdump
{
    constexpr inline auto maxRotationFilesCount = 5;
    class CrashDumpWriterVFS : public CrashDumpWriter
    {
      public:
        CrashDumpWriterVFS() : rotator{".hex"}
        {}
        void openDump() override;
        void saveDump() override;

        void writeBytes(const std::uint8_t *buff, std::size_t size) override;
        void writeHalfWords(const std::uint16_t *buff, std::size_t size) override;
        void writeWords(const std::uint32_t *buff, std::size_t size) override;

      private:
        utils::Rotator<maxRotationFilesCount> rotator;
        int dumpFd{-1};

        std::shared_ptr<purefs::fs::filesystem> vfs;
    };

} // namespace crashdump
