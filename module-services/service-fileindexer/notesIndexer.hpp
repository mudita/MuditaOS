// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <cstddef>

namespace service::detail
{
    //! Index text file class
    class notesIndexer
    {
      public:
        explicit notesIndexer(std::string_view path);
        [[nodiscard]] auto getWords() const noexcept
        {
            return mWordCount;
        }
        [[nodiscard]] auto getLines() const noexcept
        {
            return mLinesCount;
        }
        [[nodiscard]] auto getChars() const noexcept
        {
            return mCharCount;
        }
        [[nodiscard]] auto getFileSize() const noexcept
        {
            return mCharCount;
        }

      private:
        auto updateLineStats(std::string_view line) noexcept -> void;

      private:
        std::size_t mWordCount{};
        std::size_t mLinesCount{};
        std::size_t mCharCount{};
        std::size_t mFileSize{};
    };
} // namespace service::detail