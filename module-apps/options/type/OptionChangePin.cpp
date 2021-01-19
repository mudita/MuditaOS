// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionChangePin.hpp"
#include <widgets/Text.hpp>
#include <widgets/TextFixedSize.hpp>
#include <FontManager.hpp>
#include <service-db/Settings.hpp>

auto gui::option::ChangePin::build() const -> gui::ListItem *
{
    auto optionItem = new gui::ListItem();
    optionItem->setEdges(RectangleEdge::None);
    optionItem->setMinimumSize(style::window::default_body_width, style::window::label::big_h);

    auto optionBodyHBox = new gui::HBox(optionItem, 0, 0, 0, 0);
    auto font           = FontManager::getInstance().getFont(style::window::font::medium);
    auto font_bold      = FontManager::getInstance().getFont(style::window::font::mediumbold);

    optionBodyHBox->setEdges(RectangleEdge::None);
    optionBodyHBox->setReverseOrder(true);

    auto text = new TextFixedSize(nullptr, 0, 0, 0, 0);
    text->setMaximumSize(style::window::default_body_width, style::window::label::big_h);
    text->setUnderline(false);
    text->setEditMode(EditMode::Browse);
    text->setFont(font_bold);
    text->setText("Change PIN");
    text->setAlignment(Alignment(Alignment::Vertical::Center));
    text->activeItem = false;
    text->setPenWidth(0);

    auto pin_text = new PinChange();

    pin_text->setMargins(Margins(0, 0, 0, 0));
    pin_text->setMinimumSize(60, style::window::label::big_h);
    pin_text->setMaximumSize(200, style::window::label::big_h);
    pin_text->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
    pin_text->setEditMode(EditMode::Edit);
    pin_text->setFont(font);
    pin_text->setText(std::to_string(lockHash));
    pin_text->setPenWidth(style::window::default_border_focus_w);
    pin_text->setInputMode(new InputMode(
        {InputMode::digit}, [=](const UTF8 &text) {}, [=]() {}, [=]() {}));
    pin_text->setEdges(RectangleEdge::All);

    optionItem->dimensionChangedCallback = [optionBodyHBox](gui::Item &, const BoundingBox &newDim) -> bool {
        optionBodyHBox->setPosition(0, 0);
        optionBodyHBox->setSize(newDim.w, newDim.h);
        return true;
    };

    optionItem->activatedCallback = [pin_text, this](gui::Item &item) {
        auto text = pin_text->getText();
        if (text.length() == 0) {
            LOG_DEBUG("remove pin");
            setter->clearPin();
        }
        else if (text.length() != 4) {
            pin_text->setText("bad value - needs 4 digits");
        }
        else {
            auto value = std::stoi(text);
            LOG_DEBUG("setting pin to: %d", value);
            setter->setPin(value);
        }
        return true;
    };

    optionItem->inputCallback = [pin_text]([[maybe_unused]] Item &item, const InputEvent &event) {
        return pin_text->onInput(event);
    };

    optionBodyHBox->addWidget(pin_text);
    optionBodyHBox->addWidget(text);

    return optionItem;
}

std::list<gui::Option> settingChangePinWindow(app::Application *app, app::PinLockSetter *setter, unsigned int lockHash)
{
    std::list<gui::Option> list;
    list.emplace_back(std::make_unique<gui::option::ChangePin>(app, setter, lockHash));
    return list;
}
