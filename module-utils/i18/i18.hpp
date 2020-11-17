// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef UNTITLED_I18_HPP
#define UNTITLED_I18_HPP

#include "json/json11.hpp"
#include <string>
#include "vfs.hpp"
#include "log/log.hpp"

#include "thread.hpp"

using Language = std::string;

namespace utils
{
    class LangLoader
    {
      public:
        LangLoader()
        {}
        virtual ~LangLoader()
        {}
        std::vector<Language> getAvailableDisplayLanguages() const;
        json11::Json createJson(const std::string &filename);
    };

    class i18
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
        // Default constructor, left empty on purpose
        i18()
        {}

        // Explicit initialization point, default constructor is omitted. This is because LangLoader uses file system
        // which is not available at program's startup.
        virtual ~i18()
        {}
        void setInputLanguage(const Language &lang);
        const std::string &getInputLanguage(const std::string &str);
        const std::string &get(const std::string &str);
        void setDisplayLanguage(const Language &lang);
        void setFallbackLanguage(const Language &lang);
    };

    // Global instance of i18 class
    extern i18 localize;
    inline auto translateI18(const std::string &text)
    {
        return utils::localize.get(text);
    };
} // namespace utils

#endif // UNTITLED_I18_HPP
