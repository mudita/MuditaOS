#pragma once

#include "../Events.hpp"

namespace harness::events
{

    /// returns nullptr on fail, KbdMessage on success
    /// of json { "t" : KeyPress, "d" : [1,2,3,4]
    /// where 1,2,3,4 is: [ key_code, key_press (0 released), press_time, release_time ]
    /// TODO now only KeyReleased - as steering on application walking is done on KeyReleased either way
    struct KeyPress : public Event<harness::Events::KeyPress>
    {
        KeyPress(const json11::Json &js);
        ~KeyPress() override = default;
        auto encode() -> std::string final;
        auto decode(const json11::Json &js) -> bool final;
    };
}; // namespace harness::events
