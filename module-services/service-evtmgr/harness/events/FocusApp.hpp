// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string> // for string

#include "../Events.hpp" // for Event, Events, Events::FocusApp

namespace harness
{

    struct FocusApp : public Event<harness::Events::FocusApp>
    {
        FocusApp(const std::string &name) : name(name)
        {}
        std::string name;
        // encoded json event
        virtual std::string encode() final;
    };

} // namespace harness
