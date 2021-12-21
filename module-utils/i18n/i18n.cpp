// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "i18nImpl.hpp"
#include "Metadata.hpp"

#include <purefs/filesystem_paths.hpp>
#include <i18n/i18n.hpp>
#include <algorithm>
#include <optional>

namespace utils
{
    namespace
    {
        auto returnNonEmptyString(const std::string &str, const std::string &ret) -> const std::string &
        {
            return str.empty() ? ret : str;
        }

        class i18nPrivateInterface : public i18n
        {
          public:
            const std::string &get(const std::string &str);
            const std::vector<std::string> get_array(const std::string &str);
        };

        const std::string &i18nPrivateInterface::get(const std::string &str)
        {
            // if language pack returned nothing then try default language
            if (getDisplayLanguageJSON()[str].string_value().empty()) {
                return returnNonEmptyString(getFallbackLanguageJSON()[str].string_value(), str);
            }
            return returnNonEmptyString(getDisplayLanguageJSON()[str].string_value(), str);
        }

        const std::vector<std::string> i18nPrivateInterface::get_array(const std::string &str)
        {
            std::vector<std::string> out;
            auto object = getDisplayLanguageJSON()[str];
            // if language pack returned nothing then try default language
            if (not object.is_array()) {
                object = getFallbackLanguageJSON()[str];
            }
            if (object.is_array()) {
                out.reserve(object.array_items().size());
                std::for_each(object.array_items().begin(), object.array_items().end(), [&out](const auto &o) {
                    out.emplace_back(o.string_value());
                });
            }
            return out;
        }

        i18nPrivateInterface localize;

    } // namespace

    void i18n::resetAssetsPath(const std::filesystem::path &assets)
    {
        DisplayLanguageDirPath = assets / "lang";
        InputLanguageDirPath   = assets / "profiles";
    }

    bool i18n::setInputLanguage(const Language &lang)
    {
        cpp_freertos::LockGuard lock(mutex);
        if (lang.empty() || lang == inputLanguage) {
            return false;
        }

        inputLanguage = lang;
        return true;
    }

    const std::string &i18n::getInputLanguageFilename(const std::string &inputMode)
    {
        // if language pack returned nothing then try default language
        if (inputLanguage.empty()) {
            inputLanguageFilename = fallbackLanguageName + utils::files::breakSign + inputMode;
        }
        else {
            inputLanguageFilename = inputLanguage + utils::files::breakSign + inputMode;
        }
        return inputLanguageFilename;
    }

    bool i18n::setDisplayLanguage(const Language &lang)
    {
        cpp_freertos::LockGuard lock(mutex);
        if (fallbackLanguage.is_null()) {
            loadFallbackLanguage();
            loadMetadata();
        }

        if ((lang.empty() || lang == currentDisplayLanguage) && !currentDisplayLanguage.empty()) {
            return false;
        }
        else if (const auto result = getMetadata(lang)) {
            currentDisplayLanguage = lang;
            displayLanguage        = loader(result->path).value();
            return true;
        }
        return false;
    }

    void i18n::loadFallbackLanguage()
    {
        currentDisplayLanguage = fallbackLanguageName;
        fallbackLanguage =
            loader(localize.getDisplayLanguagePath() / (fallbackLanguageName + utils::files::jsonExtension)).value();
    }

    void i18n::loadMetadata()
    {
        for (const auto &entry : std::filesystem::directory_iterator(getDisplayLanguagePath())) {
            if (const auto data = fetchMetadata(entry.path())) {
                metadata.push_back(*data);
            }
        }
    }

    const std::string &translate(const std::string &text)
    {
        return utils::localize.get(text);
    }

    const std::vector<std::string> translate_array(const std::string &text)
    {
        return utils::localize.get_array(text);
    }

    const std::string &getDisplayLanguage()
    {
        return utils::localize.getDisplayLanguage();
    }

    const std::string &getInputLanguage()
    {
        return utils::localize.getInputLanguage();
    }

    const std::string &getInputLanguageFilename(const std::string &inputMode)
    {
        return localize.getInputLanguageFilename(inputMode);
    }

    bool setInputLanguage(const Language &lang)
    {
        return localize.setInputLanguage(lang);
    }

    bool setDisplayLanguage(const Language &lang)
    {
        return localize.setDisplayLanguage(lang);
    }

    const std::filesystem::path getInputLanguagePath()
    {
        return localize.getInputLanguagePath();
    }

    const std::filesystem::path getDisplayLanguagePath()
    {
        return localize.getDisplayLanguagePath();
    }

    void i18n::resetDisplayLanguages()
    {
        currentDisplayLanguage.clear();
        displayLanguage  = json11::Json();
        fallbackLanguage = json11::Json();
    }

    std::optional<LanguageMetadata> i18n::getMetadata(const Language &lang) const
    {
        const auto result =
            std::find_if(metadata.begin(), metadata.end(), [&lang](const auto &e) { return e.displayName == lang; });
        if (result != metadata.end()) {
            return *result;
        }
        else {
            return {};
        }
    }
    std::optional<LanguageMetadata> i18n::fetchMetadata(const std::filesystem::path &path) const
    {
        if (const auto jsonData = loader(path)) {
            return LanguageMetadata::get(path, jsonData.value());
        }
        return {};
    }
    std::vector<Language> i18n::getAvailableDisplayLanguages() const
    {
        std::vector<Language> languages{metadata.size()};
        std::transform(
            metadata.cbegin(), metadata.cend(), languages.begin(), [](const auto &meta) { return meta.displayName; });

        std::sort(languages.begin(), languages.end());
        return languages;
    }
    std::vector<Language> i18n::getAvailableInputLanguages() const
    {
        std::vector<Language> languageNames;
        for (const auto &entry : std::filesystem::directory_iterator(getInputLanguagePath())) {
            languageNames.push_back(std::filesystem::path(entry.path()).stem());
        }
        return languageNames;
    }

    void resetDisplayLanguages()
    {
        return localize.resetDisplayLanguages();
    }

    void resetAssetsPath(const std::filesystem::path &p)
    {
        return localize.resetAssetsPath(p);
    }
    std::vector<Language> getAvailableDisplayLanguages()
    {
        return localize.getAvailableDisplayLanguages();
    }

} // namespace utils

