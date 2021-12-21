// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional>
#include <filesystem>
#include <json11.hpp>

namespace utils
{
    class LanguageMetadata
    {
      public:
        static std::optional<LanguageMetadata> get(const std::filesystem::path &path, const json11::Json &contents)
        {
            if (contents[metadataKey].is_null() || contents[metadataKey][metadataDisplayKey].is_null()) {
                return {};
            }

            return LanguageMetadata{.displayName = contents[metadataKey][metadataDisplayKey].string_value(),
                                    .path        = path};
        }
        const std::string displayName;
        const std::filesystem::path path;

      private:
        static constexpr auto metadataKey        = "metadata";
        static constexpr auto metadataDisplayKey = "display_name";
    };
} // namespace utils
