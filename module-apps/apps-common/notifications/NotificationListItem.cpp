// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationListItem.hpp"

#include <TextFixedSize.hpp>
#include <parsers/RichTextParser.hpp>
#include <FontManager.hpp>
#include <Image.hpp>

#include <widgets/Style.hpp>
#include <map>
#include <Utils.hpp>

using namespace gui;

namespace
{
    auto buildImageInactive(const UTF8 &img) -> gui::Image *
    {
        auto thumbnail        = new gui::Image(img);
        thumbnail->activeItem = false;
        return thumbnail;
    }

    auto buildNotificationIcon(const UTF8 &icon) -> gui::Image *
    {
        auto thumbnail = buildImageInactive(icon);
        thumbnail->setMinimumWidth(style::notifications::iconWidth);
        thumbnail->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        thumbnail->setMargins(
            gui::Margins(style::window::default_left_margin, 0, style::window::default_right_margin, 0));
        return thumbnail;
    }

    auto buildNotificationNameLabel(uint32_t width) -> gui::TextFixedSize *
    {
        auto text =
            new gui::TextFixedSize(nullptr, 0, 0, style::notifications::textMinWidth, style::notifications::itemHeight);

        text->setMaximumSize(style::notifications::textMaxWidth, Axis::X);
        text->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        text->setPenWidth(style::window::default_border_no_focus_w);
        text->drawUnderline(false);
        text->setFont(style::window::font::medium);
        text->activeItem = false;
        text->setTextLimitType(TextLimitType::MaxLines, 1);
        return text;
    }

    void setNotificationHboxLayoutProperties(gui::HBox *hbox)
    {
        hbox->setAlignment(Alignment(gui::Alignment::Vertical::Center));
        hbox->setMargins(gui::Margins(0, 0, 0, 10));
        hbox->setPenWidth(style::window::default_border_no_focus_w);
        hbox->setPenFocusWidth(style::window::default_border_focus_w);
        hbox->setEdges(RectangleEdge::None);
        hbox->setMaximumSize(style::window::default_body_width, style::notifications::itemHeight);
    }

    constexpr auto singleNotification             = 1;
    constexpr auto maxNotificationValue           = 99;
    constexpr auto maxNotificationReplacementText = "99+";

    auto buildNotificationCountText(unsigned count) -> gui::Text *
    {
        const auto number = new gui::TextFixedSize();
        UTF8 countText;

        if (count == singleNotification) {
            countText = "";
        }
        else if (count > maxNotificationValue) {
            countText = maxNotificationReplacementText;
        }
        else {
            countText = utils::to_string(count);
        }

        number->drawUnderline(false);
        number->setFont(style::window::font::mediumbold);
        number->setMinimumWidthToFitText(countText);
        number->setMinimumHeightToFitText();
        number->setPenWidth(style::window::default_border_no_focus_w);
        number->setMargins(gui::Margins(0, 0, style::window::default_right_margin, 0));
        number->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        number->setText(countText);
        number->activeItem = false;

        return number;
    }

    std::map<notifications::NotificationType, UTF8> typeToIcon{
        {notifications::NotificationType::NotSeenSms, "messages_notification_icon_W_G"},
        {notifications::NotificationType::NotSeenCall, "calls_notification_icon_W_G"},
        {notifications::NotificationType::Tethering, "tethering_notification_icon_W_G"},
        {notifications::NotificationType::AlarmSnooze, "alarm_notification_icon_W_G"},
        {notifications::NotificationType::PhoneLock, "lock_notification_icon_W_G"}};
} // namespace

NotificationListItem::NotificationListItem(NotificationType type) : type{type}
{
    this->setMinimumHeight(style::notifications::itemHeight);
    this->setMaximumWidth(style::window::default_body_width);

    box = new gui::HBox(this, 0, 0, style::window::default_body_width, style::notifications::itemHeight);
    box->addWidget(buildNotificationIcon(typeToIcon[type]));
    text = buildNotificationNameLabel(box->area().w);
    box->addWidget(text);

    setNotificationHboxLayoutProperties(box);
}

void NotificationListItem::setName(const UTF8 &name, bool isRich, gui::text::RichTextParser::TokenMap &&tokens)
{
    if (isRich) {
        TextFormat format(FontManager::getInstance().getFont(style::window::font::medium));
        text::RichTextParser rtParser;
        auto parsedText = rtParser.parse(name, &format, std::move(tokens));
        text->setText(std::move(parsedText));
    }
    else {
        text->setText(name);
    }
}

void NotificationListItem::setDismissible(bool isDismissible) noexcept
{
    dismissible = isDismissible;
}

bool NotificationListItem::isDismissible() const noexcept
{
    return dismissible;
}

notifications::NotificationType NotificationListItem::getType() const noexcept
{
    return type;
}

NotificationWithEventCounter::NotificationWithEventCounter(notifications::NotificationType type, unsigned count)
    : NotificationListItem(type)
{
    box->addWidget(buildImageInactive("dot_12px_hard_alpha_W_G"));
    box->addWidget(buildNotificationCountText(count));
    text->setMaximumSize(text->getSize(Axis::X), Axis::X);
}

NotificationWithOnOffButton::NotificationWithOnOffButton(notifications::NotificationType type,
                                                         gui::ButtonTriState::State state)
    : NotificationListItem(type)
{
    auto button = new ButtonTriState(nullptr, state);
    button->setMargins(Margins(0, 0, 20, 0));
    box->addWidget(button);
}
