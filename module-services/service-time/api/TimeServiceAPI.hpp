#pragma once

#include "../messages/TimeMessage.hpp"
#include <optional>

class Service;

namespace TimeServiceAPI
{

    uint32_t GetCurrentDT();
    int GetCurrentTimeZone();

}; // namespace TimeServiceAPI
