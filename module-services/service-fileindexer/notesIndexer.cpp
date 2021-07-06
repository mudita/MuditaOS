// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>
#include "notesIndexer.hpp"
#include <cwctype>
#include <cstdio>
#include <utf8/UTF8.hpp>

namespace service::detail
{

    notesIndexer::notesIndexer(std::string_view path)
    {
        auto file = std::fopen(std::string(path).c_str(), "r");
        if (!file) {
            LOG_INFO("Unable to open requested file. Ignore...");
            return;
        }
        if (!std::feof(file)) {
            char line[4096];
            std::fgets(line, sizeof(line), file);
            mLinesCount++;
            updateLineStats(line);
        }
        std::fclose(file);
    }
    auto notesIndexer::updateLineStats(std::string_view _line) noexcept -> void
    {
        UTF8 line(std::string(_line).c_str());
        bool last_space{};
        for (std::size_t idx = 0; idx < line.length(); ++idx) {
            const auto wchar = line[idx];
            const auto space = std::iswspace(wchar);
            if (space && !last_space)
                mWordCount++;
            if (std::iswprint(wchar) && !space)
                mCharCount++;
            last_space = space;
        }
    }
} // namespace service::detail
