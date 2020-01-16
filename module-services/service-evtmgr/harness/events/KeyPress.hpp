#pragma once

#include <Service/Message.hpp>
#include <json/json11.hpp>

namespace harness
{
    /// returns nullptr on fail, KbdMessage on success
    /// of json { "t" : KeyPress, "d" : [1,2,3,4]
    /// where 1,2,3,4 is: [ key_code, key_press (0 released), press_time, release_time ]
    /// TODO now only KeyReleased
    std::shared_ptr<sys::DataMessage> parseKeyPress(json11::Json &js);
}; // namespace harness
