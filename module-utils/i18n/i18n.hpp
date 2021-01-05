// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "json/json11.hpp"
#include <string>

using Language = std::string;

namespace utils
{
    class LangLoader
    {
      public:
        virtual ~LangLoader() = default;
        std::vector<Language> getAvailableDisplayLanguages() const;
        json11::Json createJson(const std::string &filename);
    };

    class i18n
    {
      private:
        json11::Json displayLanguage;
        json11::Json inputLanguage;
        json11::Json fallbackLanguage; // backup language if item not found
        LangLoader loader;
        Language fallbackLanguageName;
        Language currentDisplayLanguage = fallbackLanguageName;
        Language currentInputLanguage   = fallbackLanguageName;
        bool backupLanguageInitializer  = false;

      public:
        static constexpr auto DefaultLanguage = "English";

        virtual ~i18n() = default;
        void setInputLanguage(const Language &lang);
        const std::string &getInputLanguage(const std::string &str);
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
