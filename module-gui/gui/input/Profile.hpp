// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputEvent.hpp"
#include <string>
#include <cstdint>
#include <vector>
#include <map>
namespace gui
{

    class KeyProfile
    {
      public:
        static const uint32_t none_key; /// defaults to 0
        uint32_t keyCode = none_key;
        bool cyclic      = false;
        std::vector<uint32_t> chars;
        std::vector<uint32_t> timeouts;

        virtual ~KeyProfile() = default;

        void addCharacters(const std::string &s);
        void addTimeouts(const std::string &s);
    };

    class Profile
    {
        std::string name;
        std::map<uint32_t, KeyProfile *> keys = {};

        void addCharacters(KeyProfile *pk, const std::string &s) const;
        void addTimeouts(KeyProfile *pk, const std::string &s) const;
        void addKeyProfile(KeyProfile *pk);
        void setName(const std::string &name);
        const KeyProfile *getKeyProfile(uint32_t keyCode) const;

      public:
        void clear();
        Profile() = default;
        Profile(const std::string &name);
        Profile(Profile &&p);
        virtual ~Profile();

        [[nodiscard]] std::string getName() const noexcept;
        bool load(const std::string &filename);

        uint32_t get(bsp::KeyCodes code, uint32_t times);
    };

} /* namespace gui */
