// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace crashdump
{
    class CrashDumpWriter
    {
      public:
        static CrashDumpWriter &instance();

        virtual ~CrashDumpWriter() = default;

        virtual void openDump() = 0;
        virtual void saveDump() = 0;

        virtual void writeBytes(const std::uint8_t *buff, std::size_t size)      = 0;
        virtual void writeHalfWords(const std::uint16_t *buff, std::size_t size) = 0;
        virtual void writeWords(const std::uint32_t *buff, std::size_t size)     = 0;
    };

} // namespace crashdump
