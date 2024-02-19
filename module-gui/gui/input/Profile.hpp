// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
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
      public:
        Profile()                          = default;
        explicit Profile(const std::filesystem::path &filepath);

        [[nodiscard]] const std::string &getName() noexcept;
        [[nodiscard]] std::uint32_t getCharKey(bsp::KeyCodes code, std::uint32_t times);
        [[nodiscard]] const json11::Json &getJson() const noexcept;

        static constexpr std::uint32_t none_key = 0;

      private:
        const json11::Json createJson(const std::string &filepath);

        static constexpr auto nameKey = "name";
        json11::Json profileJson;
    };
} // namespace gui
