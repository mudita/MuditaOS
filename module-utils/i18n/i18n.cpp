// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "i18nImpl.hpp"
#include <log.hpp>
#include <cstdio>
#include <purefs/filesystem_paths.hpp>

namespace utils
{
    namespace
    {
        auto returnNonEmptyString(const std::string &str, const std::string &ret) -> const std::string &
        {
            return str.empty() ? ret : str;
        }
    } // namespace

    namespace
    {
        class i18nPrivateInterface : public i18n
        {
          public:
            const std::string &get(const std::string &str);
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

    } // namespace

    i18nPrivateInterface localize;

    json11::Json LangLoaderImpl::createJson(const std::string &filename)
    {
        const auto path = localize.getDisplayLanguagePath() / (filename + utils::files::jsonExtension);
        auto fd         = std::fopen(path.c_str(), "r");
        if (fd == nullptr) {
            LOG_FATAL("Error during opening file %s", path.c_str());
            return json11::Json();
        }

        uint32_t fsize = std::filesystem::file_size(path);

        auto stream = std::make_unique<char[]>(fsize + 1); // +1 for NULL terminator

        std::fread(stream.get(), 1, fsize, fd);

        std::string err;
        json11::Json js = json11::Json::parse(stream.get(), err);

        std::fclose(fd);

        // Error
        if (err.length() != 0) {
            LOG_FATAL("%s", err.c_str());
            return json11::Json();
        }
        else {
            return js;
        }
    }

    std::vector<Language> LangLoader::getAvailableDisplayLanguages() const
    {
        std::vector<std::string> languageNames;
        for (const auto &entry : std::filesystem::directory_iterator(getDisplayLanguagePath())) {
            languageNames.push_back(std::filesystem::path(entry.path()).stem());
        }
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
        else if (json11::Json pack = loader.createJson(lang); pack != json11::Json()) {

            currentDisplayLanguage = lang;
            changeDisplayLanguage(pack);

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
        fallbackLanguage       = loader.createJson(fallbackLanguageName);
    }

    const std::string &translate(const std::string &text)
    {
        return utils::localize.get(text);
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
