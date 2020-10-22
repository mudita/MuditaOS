// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BoxLayout.hpp"
#include "Icon.hpp"

namespace gui
{
    class NotificationsBox : public VBox
    {
      public:
        NotificationsBox(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

        auto addNotification(UTF8 icon,
                             UTF8 name,
                             UTF8 indicator,
                             std::function<bool()> showCallback,
                             std::function<bool()> clearCallback,
                             std::function<void(bool)> onFocus) -> bool;

        bool onInput(const InputEvent &inputEvent) override;
        bool clearAll(const InputEvent &event);

        void navigateToBottom();
    };
} // namespace gui
