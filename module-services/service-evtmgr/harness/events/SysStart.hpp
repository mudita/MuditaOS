#pragma once

#include "../Events.hpp"

namespace harness
{

    struct SysStart : public Event<harness::Events::SysStart>
    {
        // encoded json event
        virtual std::string encode() final;
    };

}; // namespace harness
