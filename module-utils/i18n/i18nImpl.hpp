// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <mutex.hpp>
#include <json11.hpp>
#include <i18n/i18n.hpp>

namespace utils
{

    class LangLoaderImpl : public LangLoader
    {
      public:
        json11::Json createJson(const std::string &filename);
    };

    class i18n
    {
      private:
        json11::Json displayLanguage;
        json11::Json fallbackLanguage; // backup language if item not found
        LangLoaderImpl loader;
        Language fallbackLanguageName = getDefaultLanguage();
        Language inputLanguage        = fallbackLanguageName;
        Language inputLanguageFilename;
        Language currentDisplayLanguage;
        std::filesystem::path InputLanguageDirPath   = "assets/profiles";
        std::filesystem::path DisplayLanguageDirPath = "assets/lang";
        mutable cpp_freertos::MutexStandard mutex;

        void changeDisplayLanguage(const json11::Json &lang);
        void loadFallbackLanguage();

      protected:
        const std::string &get(const std::string &str);
        json11::Json &getDisplayLanguageJSON()
        {
            return displayLanguage;
        }
        json11::Json &getFallbackLanguageJSON()
        {
            return fallbackLanguage;
        }
        const std::string &getDisplayLanguage()
        {
            return currentDisplayLanguage;
        }
        const std::string &getInputLanguage()
        {
            return inputLanguage;
        }
        const std::string &getInputLanguageFilename(const std::string &inputMode);
        bool setInputLanguage(const Language &lang);
        bool setDisplayLanguage(const Language &lang);
        const std::filesystem::path getInputLanguagePath()
        {
            return InputLanguageDirPath;
        }
        const std::filesystem::path getDisplayLanguagePath()
        {
            return DisplayLanguageDirPath;
        }

        void resetDisplayLanguages();
        void resetAssetsPath(const std::filesystem::path &);
    };

} // namespace utils
