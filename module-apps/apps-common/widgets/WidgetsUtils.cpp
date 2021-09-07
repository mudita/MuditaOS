// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WidgetsUtils.hpp"

#include <module-gui/gui/core/BoundingBox.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/Item.hpp>

namespace gui
{
    void passDefaultCallbacksFromListItem(Item *parent, Item *child)
    {
        parent->focusChangedCallback = [=](Item &) {
            parent->setFocusItem(parent->focus ? child : nullptr);
            return true;
        };

        parent->inputCallback = [=](Item &, const InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }
            return child->onInput(event);
        };

        parent->dimensionChangedCallback = [=](gui::Item &, const BoundingBox &newDim) -> bool {
            child->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
} // namespace gui
