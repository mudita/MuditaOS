/*
 * @file Profile.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include "log/log.hpp"
#include "utf8/UTF8.hpp"
#include "vfs.hpp"
#include "Profile.hpp"

namespace gui
{

    KeyProfile::KeyProfile()
    {}
    KeyProfile::~KeyProfile()
    {}

    Profile::Profile(const std::string &name)
    {
        LOG_INFO("Create!");
        load(name);
    }

    Profile::Profile(Profile &&p)
    {
        this->name = p.name;
        this->keys = p.keys;
        // this is important, we need to assure that moved Profile doesn't clean up our memory again
        p.clear();
    }

    Profile::~Profile()
    {
        for (auto it = keys.begin(); it != keys.end(); it++) {
            delete it->second;
        }
        keys.clear();
    }

    static inline std::string trim(const std::string &s)
    {
        auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) { return std::isspace(c); });
        return std::string(wsfront,
                           std::find_if_not(s.rbegin(), std::string::const_reverse_iterator(wsfront), [](int c) {
                               return std::isspace(c);
                           }).base());
    }

    template <typename Out> void split(const std::string &s, char delim, Out result)
    {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            *(result++) = item;
        }
    }

    static std::vector<std::string> split(const std::string &s, char delim)
    {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }

    bool Profile::load(std::string filename)
    {
        auto file = vfs.fopen(filename.c_str(), "rb");

        enum class LineType
        {
            KEY_CODE = 0,
            CYCLIC,
            CHARACTERS,
            TIMEOUTS
        };

        LineType lineType = LineType::KEY_CODE;
        bool name         = false;

        KeyProfile *pk = nullptr;
        KeyProfile tmp;
        while (vfs.eof(file) != true) {

            std::string line = trim(vfs.getline(file));
            if ((line[0] == '#') || (line.empty())) {
                continue;
            }
            else {

                // first not commented line is a name of profile
                if (name == false) {
                    LOG_INFO("profile name: %s", line.c_str());
                    setName(line);
                    name = true;
                }
                else {
                    // new structure, create profile key and read the keyboard's key code
                    if (lineType == LineType::KEY_CODE) {
                        uint32_t keyCode;
                        std::stringstream(line) >> keyCode;
                        pk          = new KeyProfile();
                        pk->keyCode = keyCode;
                        lineType    = LineType::CYCLIC;
                    }
                    else if (lineType == LineType::CYCLIC) {
                        pk->cyclic = (line.compare("true") == 0);
                        lineType   = LineType::CHARACTERS;
                    }
                    else if (lineType == LineType::CHARACTERS) {
                        addCharacters(pk, line);
                        lineType = LineType::TIMEOUTS;
                    }
                    else if (lineType == LineType::TIMEOUTS) {
                        addTimeouts(pk, line);
                        if ((pk->chars.size() == pk->timeouts.size()) && (keys.find(pk->keyCode) == keys.end())) {
                            addKeyProfile(pk);
                        }
                        else {
                            LOG_FATAL("Incorrect number of chars or key code duplicate key code: [%" PRIu32 "]",
                                      pk->keyCode);
                            delete pk;
                            pk = nullptr;
                        }

                        lineType = LineType::KEY_CODE;
                    }
                    else {
                        LOG_FATAL("invalid line: [%s]", line.c_str());
                    }
                }
            }
        }

        vfs.fclose(file);

        return true;
    }

    void Profile::addCharacters(KeyProfile *pk, const std::string &s)
    {

        uint32_t charKey;
        std::vector<std::string> vec = split(s, ',');
        for (std::string s : vec) {
            std::string ts = trim(s);
            if (ts[0] == '\'') {
                // empty character - no character
                if (s.size() == 2) {
                    pk->chars.push_back(0);
                    break;
                }
                ts       = s.substr(1, s.size() - 1);
                UTF8 utf = UTF8(ts);
                charKey  = utf[0];
                pk->chars.push_back(charKey);
            }
            else if (ts.substr(0, 2) == "0x") {
                std::stringstream ss;
                ss << std::hex << ts;
                ss >> charKey;
                pk->chars.push_back(charKey);
            }
        }
    }
    void Profile::addTimeouts(KeyProfile *pk, const std::string &s)
    {
        uint32_t timeout;
        std::vector<std::string> vec = split(s, ',');
        for (std::string s : vec) {
            std::stringstream(trim(s)) >> timeout;
            pk->timeouts.push_back(timeout);
        }
    }

    void Profile::addKeyProfile(KeyProfile *pk)
    {
        if (pk != nullptr)
            keys.insert(std::pair<uint32_t, KeyProfile *>(pk->keyCode, pk));
    }

    const KeyProfile *Profile::getKeyProfile(uint32_t keyCode)
    {
        auto key = keys.find(keyCode);
        if (key != keys.end())
            return key->second;
        return nullptr;
    }

    uint32_t Profile::get(bsp::KeyCodes code, uint32_t times)
    {
        const KeyProfile *p = getKeyProfile(static_cast<uint32_t>(code));
        if (p == nullptr) {
            LOG_DEBUG("KeyProfile for key: %" PRIu32 " not found", static_cast<uint32_t>(code));
            return 0;
        }
        return p->chars[times % p->chars.size()];
    }

} /* namespace gui */
