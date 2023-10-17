// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "InputEvent.hpp"
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <json11.hpp>
#include <filesystem>

namespace gui
{

    class Profile
    {
      private:
        std::string name;
        json11::Json inputChars;

        const json11::Json createJson(const std::string &filepath);

      public:
        static constexpr uint32_t none_key = 0;
        Profile()                          = default;
        explicit Profile(const std::filesystem::path &filepath);

        [[nodiscard]] const std::string &getName() noexcept;
        [[nodiscard]] uint32_t getCharKey(bsp::KeyCodes code, uint32_t times);
        [[nodiscard]] const json11::Json &getJson() const noexcept;
    };

} /* namespace gui */
