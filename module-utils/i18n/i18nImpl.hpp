// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "JSONLoader.hpp"
#include "Metadata.hpp"

#include <optional>
#include <mutex.hpp>
#include <json11.hpp>
#include <i18n/i18n.hpp>

namespace utils
{
    class i18n
    {
      private:
        using Loader  = std::function<std::optional<json11::Json>(const std::filesystem::path &path)>;
        Loader loader = JSONLoader;

        json11::Json displayLanguage;
        json11::Json fallbackLanguage; // backup language if item not found
        Language fallbackLanguageName = getDefaultLanguage();
        Language inputLanguage        = fallbackLanguageName;
        Language inputLanguageFilename;
        Language currentDisplayLanguage;
        std::filesystem::path InputLanguageDirPath   = "assets/profiles";
        std::filesystem::path DisplayLanguageDirPath = "assets/lang";
        cpp_freertos::MutexStandard mutex;
        std::vector<LanguageMetadata> metadata;

        void loadFallbackLanguage();
        void loadMetadata();
        std::optional<LanguageMetadata> getMetadata(const Language &lang) const;
        std::optional<LanguageMetadata> fetchMetadata(const std::filesystem::path &path) const;

      public:
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
        const std::filesystem::path getInputLanguagePath() const
        {
            return InputLanguageDirPath;
        }
        const std::filesystem::path getDisplayLanguagePath() const
        {
            return DisplayLanguageDirPath;
        }

        std::vector<Language> getAvailableDisplayLanguages() const;
        std::vector<Language> getAvailableInputLanguages() const;

        void resetDisplayLanguages();
        void resetAssetsPath(const std::filesystem::path &);
    };

} // namespace utils
