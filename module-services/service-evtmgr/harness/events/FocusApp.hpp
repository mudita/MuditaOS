#pragma once

#include "../Events.hpp"

namespace harness
{

    struct FocusApp : public Event<harness::Events::FocusApp>
    {
        FocusApp(const std::string &name) : name(name)
        {
        }
        std::string name;
        // encoded json event
        virtual std::string encode() final;
    };

} // namespace harness
