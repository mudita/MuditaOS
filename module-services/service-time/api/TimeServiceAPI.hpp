#pragma once

#include "../messages/TimeMessage.hpp"
#include <optional>

class TimeServiceAPI
{
  public:
    static bool messageReloadTimers(sys::Service *sender);
    static bool messageTimersProcessingStart(sys::Service *sender);
    static bool messageTimersProcessingStop(sys::Service *sender);
};

/*namespace TimeServiceAPI
{

    uint32_t GetCurrentDT();
    int GetCurrentTimeZone();

    class TimeServiceAPI
    {
      public:

        static bool messageReloadTimers(sys::Service *sender);
        static bool messageTimersProcessingStart(sys::Service *sender);
        static bool messageTimersProcessingStop(sys::Service *sender);

    };
}; // namespace TimeServiceAPI*/
