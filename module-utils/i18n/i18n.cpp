// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "log/log.hpp"
#include "i18n.hpp"
#include "Utils.hpp"
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

    i18n localize;
    json11::Json LangLoader::createJson(const std::string &filename)
    {
        const auto path = utils::localize.DisplayLanguageDirPath / (filename + utils::files::jsonExtension);
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
        for (const auto &entry : std::filesystem::directory_iterator(utils::localize.DisplayLanguageDirPath)) {
            languageNames.push_back(std::filesystem::path(entry.path()).stem());
        }
        return languageNames;
    }

    std::vector<Language> LangLoader::getAvailableInputLanguages() const
    {
        std::vector<std::string> languageNames;
        for (const auto &entry : std::filesystem::directory_iterator(utils::localize.InputLanguageDirPath)) {
            languageNames.push_back(std::filesystem::path(entry.path()).stem());
        }
        return languageNames;
    }

    void i18n::setInputLanguage(const Language &lang)
    {
        if (lang.empty() || lang == inputLanguage) {
            return;
        }

        inputLanguage = lang;
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

    const std::string &i18n::getInputLanguage()
    {
        return inputLanguage;
    }

    const std::string &i18n::getDisplayLanguage()
    {
        return currentDisplayLanguage;
    }

    const std::string &i18n::get(const std::string &str)
    {
        // if language pack returned nothing then try default language
        if (displayLanguage[str].string_value().empty()) {
            return returnNonEmptyString(fallbackLanguage[str].string_value(), str);
        }
        return returnNonEmptyString(displayLanguage[str].string_value(), str);
    }

    bool i18n::setDisplayLanguage(const Language &lang)
    {
        if ((lang.empty() || lang == currentDisplayLanguage) && !currentDisplayLanguage.empty()) {

            return false;
        }
        else if (json11::Json pack = loader.createJson(lang); pack != json11::Json()) {

            currentDisplayLanguage = lang;
            changeDisplayLanguage(pack);

            return true;
        }
        else if (fallbackLanguage == json11::Json()) {

            loadFallbackLanguage();
            return true;
        }

        return false;
    }

    void i18n::changeDisplayLanguage(const json11::Json &lang)
    {
        cpp_freertos::LockGuard lock(mutex);
        displayLanguage = lang;
    }

    void i18n::resetDisplayLanguages()
    {
        currentDisplayLanguage.clear();
        displayLanguage  = json11::Json();
        fallbackLanguage = json11::Json();
    }

    void i18n::loadFallbackLanguage()
    {
        cpp_freertos::LockGuard lock(mutex);
        currentDisplayLanguage = fallbackLanguageName;
        fallbackLanguage       = loader.createJson(fallbackLanguageName);
    }

} // namespace utils
