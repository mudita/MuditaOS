// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "widgets/BellSideListItemWithCallbacks.hpp"

namespace gui
{

    gui::BellSideListItemWithCallbacks::BellSideListItemWithCallbacks(const std::string &description)
        : BellSideListItem(description)
    {
        focusChangedCallback = [&](Item &) {
            if (focus) {
                setFocusItem(body);
            }
            else {
                setFocusItem(focus ? body : nullptr);
                if (onExit) {
                    onExit();
                }
            }

            return true;
        };

        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        inputCallback = [&](Item &, const InputEvent &inputEvent) -> bool { return body->onInput(inputEvent); };
    }

} // namespace gui
