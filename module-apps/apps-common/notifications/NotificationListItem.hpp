// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListItem.hpp>
#include <BoxLayout.hpp>
#include <Text.hpp>
#include <widgets/ButtonTriState.hpp>

#include "NotificationData.hpp"

namespace gui
{

    class TextFixedSize;

    class NotificationListItem : public ListItem
    {
        using NotificationType      = notifications::NotificationType;
        const NotificationType type = NotificationType::NotSeenSms;
        bool dismissible            = false;

      protected:
        gui::HBox *box           = nullptr;
        gui::TextFixedSize *text = nullptr;

      public:
        explicit NotificationListItem(NotificationType type);

        [[nodiscard]] bool isDismissible() const noexcept;
        [[nodiscard]] NotificationType getType() const noexcept;
        void setName(const UTF8 &name, bool isRich = false, gui::text::RichTextParser::TokenMap &&tokens = {});
        void setDismissible(bool isDismissible) noexcept;

        std::function<void()> dismissCallback = nullptr;
    };

    class NotificationWithEventCounter : public NotificationListItem
    {
      public:
        NotificationWithEventCounter(notifications::NotificationType type, const UTF8 &indicator);
    };

    class NotificationWithOnOffButton : public NotificationListItem
    {

      public:
        NotificationWithOnOffButton(notifications::NotificationType type, gui::ButtonTriState::State state);
    };

} // namespace gui
