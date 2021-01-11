// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsBox.hpp"

#include "application-desktop/data/Style.hpp"

#include "TextFixedSize.hpp"
#include "RichTextParser.hpp"
#include "FontManager.hpp"

using namespace gui;
using namespace style::desktop;

NotificationsBox::NotificationsBox(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
    : VBox(parent, x, y, w, h)
{
    this->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
    this->setPenWidth(style::window::default_border_no_focus_w);
    this->setPenFocusWidth(style::window::default_border_no_focus_w);

    auto getNextNotification = [this]() -> Item * {
        auto focusedItem = getFocusItem();
        if (focusedItem == nullptr) {
            return nullptr;
        }
        auto nextItem = focusedItem->getNavigationItem(NavigationDirection::UP);

        if (nextItem == nullptr) {
            nextItem = focusedItem->getNavigationItem(NavigationDirection::DOWN);
        }
        return nextItem;
    };

    auto setNextFocusedItemAfterErase = [this](Item *item) -> bool {
        if (item == nullptr) {
            setFocus(false);
        }
        else {
            item->clearNavigationItem(NavigationDirection::DOWN);
            item->clearNavigationItem(NavigationDirection::UP);
            setNavigation();
            setFocusItem(item);
        }
        return true;
    };

    inputCallback = [this, setNextFocusedItemAfterErase, getNextNotification](Item &, const InputEvent &event) -> bool {
        if (event.isShortPress() && event.is(KeyCode::KEY_RF)) {
            LOG_DEBUG("Removing single notification");
            auto ptr = getFocusItem();
            if (ptr == nullptr || focusItem == this) {
                return false;
            }
            auto nextItem = getNextNotification();
            ptr->inputCallback(*this, event);
            erase(ptr);
            return setNextFocusedItemAfterErase(nextItem);
        }
        return false;
    };
}

namespace
{
    auto buildImageInactive(const UTF8 &img) -> gui::Image *
    {
        auto thumbnail        = new gui::Image(img);
        thumbnail->activeItem = false;
        return thumbnail;
    }

    auto buildNotificationIcon(UTF8 icon) -> gui::Image *
    {
        auto thumbnail = buildImageInactive(icon);
        thumbnail->setMinimumWidth(notifications::IconWidth);
        thumbnail->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        thumbnail->setMargins(
            gui::Margins(style::window::default_left_margin, 0, style::window::default_right_margin, 0));
        return thumbnail;
    }

    auto buildNotificationNameLabel(const UTF8 &name, uint32_t width) -> gui::TextFixedSize *
    {
        auto text = new gui::TextFixedSize(
            nullptr, 0, 0, style::desktop::notifications::TextMaxWidth, style::window::label::default_h);
        text->setMaximumSize(width, Axis::X);

        TextFormat format(FontManager::getInstance().getFont(style::window::font::medium));
        text::RichTextParser rtParser;
        auto parsedText = rtParser.parse(name, &format);

        text->setText(std::move(parsedText));
        text->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        text->setPenWidth(style::window::default_border_no_focus_w);
        text->setUnderline(false);
        text->activeItem = false;
        return text;
    }

    constexpr auto maxNotificationValue = "99+";

    auto buildNotificationCountText(const UTF8 &indicator) -> gui::Text *
    {
        auto number = new gui::Text();
        if (indicator.length() > 2) {
            number->setText(maxNotificationValue);
        }
        else {
            number->setText(indicator);
        }

        number->setMinimumWidth(number->getText().length() * notifications::DigitSize);
        number->setFont(style::window::font::mediumbold);
        number->setPenWidth(style::window::default_border_no_focus_w);
        number->setMargins(gui::Margins(0, 0, style::window::default_right_margin, 0));
        number->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        number->activeItem = false;
        return number;
    }

} // namespace

auto NotificationsBox::addNotification(UTF8 icon,
                                       UTF8 name,
                                       UTF8 indicator,
                                       std::function<bool()> showCallback,
                                       std::function<bool()> clearCallback,
                                       std::function<void(bool)> onFocus) -> bool
{
    // 1. create hbox for all elements
    auto el = new gui::HBox(nullptr, 0, 0, style::window::default_body_width, style::window::label::default_h);

    // 2. Add all elements to hbox layout
    el->addWidget(buildNotificationIcon(icon));

    el->addWidget(buildNotificationNameLabel(name, el->area().w));
    el->addWidget(buildImageInactive("dot_12px_hard_alpha_W_G"));
    el->addWidget(buildNotificationCountText(indicator));

    // 3. Set hbox layout properties
    el->setAlignment(Alignment(gui::Alignment::Vertical::Center));
    el->setMargins(gui::Margins(0, 0, 0, 10));
    el->setPenWidth(style::window::default_border_no_focus_w);
    el->setPenFocusWidth(style::window::default_border_focus_w);
    el->setEdges(RectangleEdge::Bottom | RectangleEdge::Top);

    el->focusChangedCallback = [el, onFocus](Item &) -> bool {
        onFocus(el->focus);
        return true;
    };
    el->activatedCallback = [showCallback](Item &) { return showCallback(); };
    el->inputCallback     = [showCallback, clearCallback, this](Item &, const InputEvent &event) -> bool {
        if (event.isShortPress() && event.is(KeyCode::KEY_RF) && clearCallback) {
            return clearCallback();
        }
        return false;
    };

    this->addWidget(el);
    return el->visible;
}

bool NotificationsBox::onInput(const InputEvent &inputEvent)
{

    if (inputEvent.isShortPress() && (inputEvent.is(KeyCode::KEY_UP) || inputEvent.is(KeyCode::KEY_DOWN))) {
        auto handled = handleNavigation(inputEvent);
        if (!handled) {
            setFocus(false);
        }
        return true;
    }
    return VBox::onInput(inputEvent);
}

bool NotificationsBox::clearAll(const InputEvent &event)
{
    while (!children.empty()) {
        inputCallback(*this, event);
    }
    return true;
}

void NotificationsBox::navigateToBottom()
{
    setFocusItem(children.back());
}
