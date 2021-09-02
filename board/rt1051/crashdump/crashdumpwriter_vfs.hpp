// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "crashdumpwriter.hpp"

#include <array>
#include <ctime>
#include <memory>

namespace purefs::fs
{
    class filesystem;
}

namespace crashdump
{
    constexpr inline auto CrashDumpFileNameFormat = "crashdump-%FT%TZ.hex";

    class CrashDumpWriterVFS : public CrashDumpWriter
    {
      public:
        void openDump() override;
        void saveDump() override;

        void writeBytes(const std::uint8_t *buff, std::size_t size) override;
        void writeHalfWords(const std::uint16_t *buff, std::size_t size) override;
        void writeWords(const std::uint32_t *buff, std::size_t size) override;

      private:
        template <std::size_t N> void formatCrashDumpFileName(std::array<char, N> &name)
        {
            std::time_t now;
            std::time(&now);
            std::strftime(name.data(), name.size(), CrashDumpFileNameFormat, std::localtime(&now));
        }

        int dumpFd{-1};

        std::shared_ptr<purefs::fs::filesystem> vfs;
    };

} // namespace crashdump
