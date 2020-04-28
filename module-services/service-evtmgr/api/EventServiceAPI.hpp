#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

namespace EventServiceAPI
{
    /*
     * @brief Gets platform version.
     * @return true on T4 platform, false on T3 platform
     */
    bool GetHwPlatform(sys::Service *serv);
} // namespace EventServiceAPI
