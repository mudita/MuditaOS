// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <filesystem>
#include <vector>

using Language = std::string;

namespace utils
{
    namespace files
    {
        constexpr auto jsonExtension = ".json";
        constexpr auto breakSign     = "_";
    } // namespace files

    class LangLoader
    {
      public:
        virtual ~LangLoader() = default;
        std::vector<Language> getAvailableDisplayLanguages() const;
        std::vector<Language> getAvailableInputLanguages() const;
    };

    const std::string &translate(const std::string &text);
    const std::string &getDisplayLanguage();
    const std::string &getInputLanguage();
    const std::string &getInputLanguageFilename(const std::string &inputMode);

    bool setInputLanguage(const Language &lang);
    bool setDisplayLanguage(const Language &lang);
    void resetDisplayLanguages();

    const std::filesystem::path getInputLanguagePath();
    const std::filesystem::path getDisplayLanguagePath();

    constexpr auto getDefaultLanguage()
    {
        return "English";
    }
    void resetAssetsPath(const std::filesystem::path &);
} // namespace utils
