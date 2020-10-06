/*
 *  @file TimeSerivceAPI.cpp
 *  @author
 *  @date 08.05.20
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "TimeServiceAPI.hpp"
#include "Service/Bus.hpp"
#include "../ServiceTime.hpp"

bool TimeServiceAPI::messageReloadTimers(sys::Service *sender)
{
    auto msg = std::make_shared<ReloadTimersMessage>();
    return sys::Bus::SendUnicast(msg, service::name::service_time, sender);
}

bool TimeServiceAPI::messageTimersProcessingStart(sys::Service *sender)
{
    auto msg = std::make_shared<TimersProcessingStartMessage>();
    return sys::Bus::SendUnicast(msg, service::name::service_time, sender);
}

bool TimeServiceAPI::messageTimersProcessingStop(sys::Service *sender)
{
    auto msg = std::make_shared<TimersProcessingStopMessage>();
    return sys::Bus::SendUnicast(msg, service::name::service_time, sender);
}

/*namespace TimeServiceAPI
{

    uint32_t GetCurrentDT()
    {
        return 0;
    }

    int GetCurrentTimeZone()
    {
        return 0;
    }

} // namespace TimeServiceAPI
*/
