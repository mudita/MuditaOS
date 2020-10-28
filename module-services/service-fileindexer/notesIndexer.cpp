// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include "notesIndexer.hpp"
#include <vfs.hpp>
#include <cwctype>
#include <utf8/UTF8.hpp>

namespace service::detail
{

    notesIndexer::notesIndexer(std::string_view path)
    {
        auto file = vfs.fopen(std::string(path).c_str(), "r");
        if (!file) {
            LOG_INFO("Unable to open file %s. Ignore...", std::string().c_str());
            return;
        }
        if (!vfs.eof(file)) {
            auto line = vfs.getline(file, 4096);
            mLinesCount++;
            updateLineStats(line);
        }
        vfs.fclose(file);
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
