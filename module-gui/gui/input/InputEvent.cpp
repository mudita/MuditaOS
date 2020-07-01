#include "InputEvent.hpp"
namespace gui
{

    InputEvent::InputEvent(RawKey key, State state, KeyCode keyCode) : key(key), state(state), keyCode(keyCode)
    {}

    std::string InputEvent::str() const
    {
        std::stringstream ss;
        ss << "KeyCode: " << c_str(keyCode) << " ";
        ss << "State: " << c_str(state) << " ";
        ss << "short press: " << isShortPress();
        return ss.str();
    }

}; // namespace gui
