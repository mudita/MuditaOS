/*
 *  @file i18.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29 mar 2019
 *  @brief I18 implementation
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef UNTITLED_I18_HPP
#define UNTITLED_I18_HPP

#include "json/json11.hpp"
#include <string>
#include "vfs.hpp"
#include "log/log.hpp"

#include "thread.hpp"

namespace utils {

    enum class Lang{
        En,
        Pl,
		De,
        Sp
    };

    class LangLoader{

        const char* langEN_path = "sys/assets/lang/lang_en.json";
        const char* langPL_path = "sys/assets/lang/lang_pl.json";
        const char* langDE_path = "sys/assets/lang/lang_de.json";
        const char* langSP_path = "sys/assets/lang/lang_sp.json";

    public:

        LangLoader(){}
        virtual ~LangLoader(){}


        json11::Json Create(Lang lang)
        {
            std::string err;
            const char* path = nullptr;

            switch(lang)
            {
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

            auto fd = vfs.fopen(path,"r");
            if(fd == NULL){
                LOG_FATAL("Error during opening file %s",path);
                return json11::Json();
            }

            uint32_t fsize = vfs.filelength(fd);

            char* stream = static_cast<char*>(malloc(fsize+1)); // +1 for NULL terminator
            if(stream == NULL){
                LOG_FATAL("Memory allocation failure");
                vfs.fclose(fd);
                return json11::Json(); 
            }

            memset(stream,0,fsize+1);

            vfs.fread(stream,1,fsize,fd);

            json11::Json js = json11::Json::parse(stream, err);

            free(stream);
            vfs.fclose(fd);

            // Error
            if(err.length() != 0){
                LOG_FATAL(err.c_str());
                return json11::Json();
            }
            else{
                return js;
            }
        }
    };


    class i18 {

    json11::Json    langPack;
    json11::Json langBack; // backup language if item not found
    LangLoader      loader;
    static const Lang lang_default = Lang::En;
    Lang current = lang_default;
    bool debugRequests = true;

  public:
    // Default constructor, left empty on purpose
    i18()
    {
        }

        // Explicit initialization point, default constructor is omitted. This is because LangLoader uses file system which
        // is not available at program's startup.
        virtual ~i18(){}

        const std::string& get(const std::string& str)
        {
            auto retwithfallback = [this](const std::string &ret, const std::string &str) -> const std::string & {
                if (debugRequests && ret == "")
                {
                    return str;
                }
                else
                {
                    return ret;
                }
            };
            auto &ret = langPack[str].string_value();
            // if language pack returnet nothing then try default language
            if (ret == "" && current != lang_default)
            {
                auto &ret = langBack[str].string_value();
                return retwithfallback(ret, str);
            }
            return retwithfallback(ret, str);
        }

        void Switch(Lang lang){
            static bool initialized = false;
            if (!initialized)
            {
                langBack = loader.Create(lang_default);
                langPack = langBack;
                initialized = true;
            }
            if (lang == current)
                return;
            if (lang == lang_default)
            {
                langPack = langBack;
            }
            else
            {
                json11::Json pack = loader.Create(lang);
                // Suspend whole system during switching lang packs
                vTaskSuspendAll();
                langPack = pack;
                xTaskResumeAll();
            }
        }
    };

    // Global instance of i18 class
    extern i18 localize;
}



#endif //UNTITLED_I18_HPP
