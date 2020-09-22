#include "Action.hpp"

namespace gui
{

    Action::Action(Item *parent, uint32_t x, uint32_t y) : Image(parent, x, y, 0, 0)
    {
        set(stateMap.at(state));
    }

    void Action::setState(State state)
    {
        if (state == this->state) {
            return;
        }
        this->state = state;
        set(stateMap.at(state));
    }
}; // namespace gui
