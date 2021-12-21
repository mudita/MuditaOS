// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <functional>

using Language = std::string;

namespace utils
{
    namespace files
    {
        constexpr auto jsonExtension = ".json";
        constexpr auto breakSign     = "_";
    } // namespace files

    const std::string &translate(const std::string &text);
    const std::vector<std::string> translate_array(const std::string &text);
    const std::string &getDisplayLanguage();
    const std::string &getInputLanguage();
    const std::string &getInputLanguageFilename(const std::string &inputMode);

    std::vector<Language> getAvailableDisplayLanguages();
    std::vector<Language> getAvailableInputLanguages();

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
