// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "i18nImpl.hpp"
#include <log/log.hpp>
#include <algorithm>
#include <optional>
#include <cstdio>
#include <purefs/filesystem_paths.hpp>
#include <i18n/i18n.hpp>
#include <utility>

namespace utils
{
    namespace
    {
        auto returnNonEmptyString(const std::string &str, const std::string &ret) -> const std::string &
        {
            return str.empty() ? ret : str;
        }

        std::optional<json11::Json> loadJSONData(const std::filesystem::path &path)
        {
            auto fd = std::fopen(path.c_str(), "r");
            if (fd == nullptr) {
                LOG_FATAL("Error during opening file %s", path.c_str());
                return {};
            }

            const auto fsize = std::filesystem::file_size(path);

            auto stream = std::make_unique<char[]>(fsize + 1); // +1 for NULL terminator

            std::fread(stream.get(), 1, fsize, fd);

            std::string err;
            json11::Json js = json11::Json::parse(stream.get(), err);

            std::fclose(fd);

            // Error
            if (err.length() != 0) {
                LOG_FATAL("%s", err.c_str());
                return {};
            }
            else {
                return js;
            }
        }

        struct LanguageMetadata
        {
            static std::optional<LanguageMetadata> get(const std::filesystem::path &path)
            {
                const auto contents = loadJSONData(path);
                if (not contents) {
                    return {};
                }
                const auto contentsValue = *contents;
                if (contentsValue[metadataKey].is_null() || contentsValue[metadataKey][metadataDisplayKey].is_null()) {
                    return {};
                }

                return LanguageMetadata{.displayName = contentsValue[metadataKey][metadataDisplayKey].string_value()};
            }
            const std::string displayName;

          private:
            static constexpr auto metadataKey        = "metadata";
            static constexpr auto metadataDisplayKey = "display_name";
        };
    } // namespace

    namespace
    {
        class i18nPrivateInterface : public i18n
        {
          public:
            const std::string &get(const std::string &str);
            const std::vector<std::string> get_array(const std::string &str);
            using i18n::getDisplayLanguage;
            using i18n::getDisplayLanguagePath;
            using i18n::getInputLanguage;
            using i18n::getInputLanguageFilename;
            using i18n::getInputLanguagePath;
            using i18n::resetAssetsPath;
            using i18n::resetDisplayLanguages;
            using i18n::setDisplayLanguage;
            using i18n::setInputLanguage;
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

    } // namespace

    i18nPrivateInterface localize;

    std::vector<Language> LangLoader::getAvailableDisplayLanguages() const
    {
        std::vector<std::string> languageNames;
        for (const auto &entry : std::filesystem::directory_iterator(getDisplayLanguagePath())) {
            const auto metadata = LanguageMetadata::get(entry.path());
            if (metadata) {
                languageNames.push_back(metadata->displayName);
            }
            else {
                /// If metadata is not valid use file name string as a display language
                languageNames.push_back(std::filesystem::path(entry).stem());
            }
        }

        std::sort(languageNames.begin(), languageNames.end());

        return languageNames;
    }

    std::vector<Language> LangLoader::getAvailableInputLanguages() const
    {
        std::vector<std::string> languageNames;
        for (const auto &entry : std::filesystem::directory_iterator(getInputLanguagePath())) {
            languageNames.push_back(std::filesystem::path(entry.path()).stem());
        }
        return languageNames;
    }

    void i18n::resetAssetsPath(const std::filesystem::path &assets)
    {
        DisplayLanguageDirPath = assets / "lang";
        InputLanguageDirPath   = assets / "profiles";
    }

    bool i18n::setInputLanguage(const Language &lang)
    {
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
        if (fallbackLanguage == json11::Json()) {
            loadFallbackLanguage();
        }

        if ((lang.empty() || lang == currentDisplayLanguage) && !currentDisplayLanguage.empty()) {

            return false;
        }
        else if (const auto pack =
                     loadJSONData(localize.getDisplayLanguagePath() / (lang + utils::files::jsonExtension))) {

            currentDisplayLanguage = lang;
            changeDisplayLanguage(pack.value());

            return true;
        }
        return false;
    }

    void i18n::changeDisplayLanguage(const json11::Json &lang)
    {
        cpp_freertos::LockGuard lock(mutex);
        displayLanguage = lang;
    }

    void i18n::loadFallbackLanguage()
    {
        cpp_freertos::LockGuard lock(mutex);
        currentDisplayLanguage = fallbackLanguageName;
        fallbackLanguage =
            loadJSONData(localize.getDisplayLanguagePath() / (fallbackLanguageName + utils::files::jsonExtension))
                .value();
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

    void resetDisplayLanguages()
    {
        return localize.resetDisplayLanguages();
    }

    void resetAssetsPath(const std::filesystem::path &p)
    {
        return localize.resetAssetsPath(p);
    }

} // namespace utils
