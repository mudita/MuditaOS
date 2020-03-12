#pragma once

#include <string>
#include <json/json11.hpp>
#include <Service/Message.hpp>

namespace harness
{
    inline const std::string Type = "t";
    inline const std::string Data = "d";
    enum class Events;

    /// basic event structure:
    /// {
    ///      Type : enum class Events
    ///      Data : Json() <- per event type expectancy
    /// }
    template <Events type> struct Event
    {
        const enum Events evt = type;
        std::shared_ptr<sys::DataMessage> msg; /// msg to send if success

        virtual ~Event()                     = default;
        virtual auto encode() -> std::string = 0;
        virtual auto decode(const json11::Json &js) -> bool
        {
            return false;
        }
        virtual operator bool() final
        {
            return state;
        };

      protected:
        bool state;
    };

    // enum of all emulated events we can parse
    enum class Events
    {
        KeyPress = 0, // -> key press event
        SysStart = 1, // <- system started - EventManager is on
        FocusApp = 2, // <- focus application changed to
        GSMCmd = 3, // <-> command to send on cellular command channel, expects data string in ( { ..., Data : "command
                    // to execute" }
        GPIO = 4,   // <-> GPIO request data

        Top, // <- performance monitor message, TODO

        Size /// last element for size purposes
    };

} // namespace harness
