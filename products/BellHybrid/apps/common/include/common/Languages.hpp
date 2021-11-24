// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <i18n/i18n.hpp>

#include <algorithm>
#include <vector>
#include <array>
#include <string_view>

namespace common
{
    class Languages
    {
      public:
        std::vector<Language> getSupportedLanguages() const
        {
            auto languages = loader.getAvailableDisplayLanguages();
            languages.erase(
                std::remove_if(languages.begin(), languages.end(), [this](const auto &lang) { return matcher(lang); }),
                languages.end());
            std::sort(languages.begin(), languages.end());
            return languages;
        }

      private:
        bool matcher(const std::string_view language) const
        {
            const auto result = std::find(excludedLanguages.begin(), excludedLanguages.end(), language);
            return result != excludedLanguages.end();
        }

        static constexpr std::array<std::string_view, 1> excludedLanguages = {{"Svenska"}};
        utils::LangLoader loader;
    };

} // namespace common
