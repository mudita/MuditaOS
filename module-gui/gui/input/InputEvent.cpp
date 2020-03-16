#include "InputEvent.hpp"
namespace gui
{

    InputEvent::InputEvent(RawKey key, State state, KeyCode keyCode) : key(key), state(state), keyCode(keyCode)
    {}

}; // namespace gui
