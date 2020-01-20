#pragma once

#include <string>

namespace harness
{
    inline const std::string Type = "t";
    inline const std::string Data = "t";
    enum class Events;

    /// basic event structure:
    /// {
    ///      Type : enum class Events
    ///      Data : Json() <- per event type expectancy
    /// }
    template <Events type> struct Event
    {
        const enum Events evt = type;
        // encoded json event
        virtual std::string encode() = 0;
    };

    // enum of all emulated events we can parse
    enum class Events
    {
        KeyPress = 0, // -> key press event
        SysStart = 1, // <- system started - EventManager is on
        FocusApp = 2, // <- focus application changed to

        Top, // <- performance monitor message, TODO

        Size /// last element for size purposes
    };

} // namespace harness
