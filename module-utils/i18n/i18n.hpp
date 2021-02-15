// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "json/json11.hpp"
#include <string>
#include <filesystem>

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
        json11::Json createJson(const std::string &filename);
    };

    class i18n
    {
      private:
        json11::Json displayLanguage;
        json11::Json fallbackLanguage; // backup language if item not found
        LangLoader loader;
        Language fallbackLanguageName;
        Language inputLanguage = fallbackLanguageName;
        Language inputLanguageFilename;
        Language currentDisplayLanguage = fallbackLanguageName;
        bool backupLanguageInitializer  = false;

      public:
        static constexpr auto DefaultLanguage              = "English";
        const std::filesystem::path DisplayLanguageDirPath = "assets/lang";
        const std::filesystem::path InputLanguageDirPath   = "assets/profiles";

        virtual ~i18n() = default;
        void setInputLanguage(const Language &lang);
        const std::string &getInputLanguageFilename(const std::string &inputMode);
        const std::string &getInputLanguage();
        const std::string &getDisplayLanguage();
        const std::string &get(const std::string &str);
        void setDisplayLanguage(const Language &lang);
        void setFallbackLanguage(const Language &lang);
    };

    // Global instance of i18 class
    extern i18n localize;
    inline auto translateI18(const std::string &text)
    {
        return utils::localize.get(text);
    };
} // namespace utils
