#pragma once

#include "../Events.hpp"

namespace harness
{

    struct SysStart : public Event<harness::Events::SysStart>
    {
        // encoded json event
        auto encode() -> std::string final;
    };

}; // namespace harness
