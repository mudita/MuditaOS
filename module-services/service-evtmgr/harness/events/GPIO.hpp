// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <map>    // for map
#include <string> // for string, allocator

#include "../Events.hpp"   // for Event, Events, Events::GPIO
#include "json/json11.hpp" // for Json

namespace harness::events
{
    class GPIO : public Event<harness::Events::GPIO>
    {
      public:
        ~GPIO() override = default;
        GPIO(const json11::Json &js);
        auto encode() -> std::string final;
        auto decode(const json11::Json &js) -> bool final;

      private:
        static constexpr auto num_s     = "num";
        static constexpr auto state_s   = "state";
        static constexpr auto in_s      = "in";
        std::map<std::string, int> data = {{num_s, 0}, {state_s, 0}, {in_s, 0}};
        void create_message();
    };
} // namespace harness::events
