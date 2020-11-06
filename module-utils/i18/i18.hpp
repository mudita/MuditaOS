// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef UNTITLED_I18_HPP
#define UNTITLED_I18_HPP

#include "json/json11.hpp"
#include <string>
#include "vfs.hpp"
#include "log/log.hpp"

#include "thread.hpp"

namespace utils
{

    enum class Lang
    {
        En,
        Pl,
        De,
        Sp
    };

    class LangLoader
    {

        const char *langEN_path = "assets/lang/lang_en.json";
        const char *langPL_path = "assets/lang/lang_pl.json";
        const char *langDE_path = "assets/lang/lang_de.json";
        const char *langSP_path = "assets/lang/lang_sp.json";

      public:
        LangLoader()
        {}
        virtual ~LangLoader()
        {}

        json11::Json Create(Lang lang);
    };

    class i18
    {

        json11::Json displayLang;
        json11::Json inputLang;
        json11::Json backupLang; // backup language if item not found
        LangLoader loader;
        static const Lang lang_default = Lang::En;
        Lang displayCurrent            = lang_default;
        Lang inputCurrent              = lang_default;
        bool display_init              = false;
        bool input_init                = false;

      public:
        // Default constructor, left empty on purpose
        i18()
        {}

        // Explicit initialization point, default constructor is omitted. This is because LangLoader uses file system
        // which is not available at program's startup.
        virtual ~i18()
        {}
        void setInputLanguage(Lang lang);
        const std::string &getInputLanguage(const std::string &str);
        const std::string &get(const std::string &str);
        void SetDisplayLanguage(Lang lang);
    };

    // Global instance of i18 class
    extern i18 localize;
    inline auto translateI18(const std::string &text)
    {
        return utils::localize.get(text);
    };
} // namespace utils

#endif // UNTITLED_I18_HPP
