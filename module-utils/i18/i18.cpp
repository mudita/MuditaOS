// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "i18.hpp"

namespace utils
{
    namespace
    {
        auto returnNonEmptyString(const std::string &str, const std::string &ret) -> const std::string &
        {
            return str.empty() ? ret : str;
        }
    } // namespace

    i18 localize;
    json11::Json LangLoader::Create(Lang lang)
    {
        const char *path = nullptr;

        switch (lang) {
        case Lang::Pl:
            path = langPL_path;
            break;
        case Lang::En:
            path = langEN_path;
            break;
        case Lang::De:
            path = langDE_path;
            break;
        case Lang::Sp:
            path = langSP_path;
            break;

        default:
            return json11::Json();
        }

        auto fd = vfs.fopen(path, "r");
        if (fd == nullptr) {
            LOG_FATAL("Error during opening file %s", path);
            return json11::Json();
        }

        uint32_t fsize = vfs.filelength(fd);

        auto stream = new char[fsize + 1]; // +1 for NULL terminator

        memset(stream, 0, fsize + 1);

        vfs.fread(stream, 1, fsize, fd);

        std::string err;
        json11::Json js = json11::Json::parse(stream, err);

        delete[] stream;
        vfs.fclose(fd);

        // Error
        if (err.length() != 0) {
            LOG_FATAL("%s", err.c_str());
            return json11::Json();
        }
        else {
            return js;
        }
    }

    void i18::setInputLanguage(Lang lang)
    {
        if (!input_init) {
            backupLang = loader.Create(lang_default);
            inputLang  = backupLang;
            input_init = true;
        }
        if (lang == inputCurrent) {
            return;
        }
        inputCurrent = lang;
        if (lang == lang_default) {
            inputLang = backupLang;
        }
        else {
            json11::Json pack = loader.Create(lang);
            inputLang         = pack;
        }
    }
    const std::string &i18::getInputLanguage(const std::string &str)
    {
        // if language pack returned nothing then try default language
        if (inputLang[str].string_value().empty()) {
            return returnNonEmptyString(backupLang[str].string_value(), str);
        }
        return returnNonEmptyString(inputLang[str].string_value(), str);
    }

    const std::string &i18::get(const std::string &str)
    {
        // if language pack returned nothing then try default language
        if (displayLang[str].string_value().empty()) {
            return returnNonEmptyString(backupLang[str].string_value(), str);
        }
        return returnNonEmptyString(displayLang[str].string_value(), str);
    }

    void i18::SetDisplayLanguage(Lang lang)
    {
        if (!display_init) {
            backupLang   = loader.Create(lang_default);
            displayLang  = backupLang;
            display_init = true;
        }
        if (lang == displayCurrent) {
            return;
        }
        displayCurrent = lang;
        if (lang == lang_default) {
            displayLang = backupLang;
        }
        else {
            json11::Json pack = loader.Create(lang);
            // Suspend whole system during switching lang packs
            vTaskSuspendAll();
            displayLang = pack;
            xTaskResumeAll();
        }
    }

} // namespace utils
