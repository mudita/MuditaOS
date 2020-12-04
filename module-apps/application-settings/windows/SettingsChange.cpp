// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsChange.hpp"
#include "../ApplicationSettings.hpp"
#include "module-utils/i18n/i18n.hpp"
#include "tools/Common.hpp"
#include <widgets/Text.hpp>
#include <widgets/TextFixedSize.hpp>
#include <FontManager.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/Settings.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <module-utils/Utils.hpp>

namespace style::option
{
    const inline gui::Length text_left_padding = 10;
}

namespace gui
{

    class PinChange : public gui::Text
    {
      public:
        PinChange() = default;
    };

} // namespace gui

namespace gui::option
{
    class ChangePin : public Base
    {
      private:
        app::Application *app      = nullptr;
        app::PinLockSetter *setter = nullptr;
        unsigned int lockHash      = 0;

      public:
        ChangePin(app::Application *app, app::PinLockSetter *setter, unsigned int lockHash)
            : app(app), setter(setter), lockHash(lockHash)
        {}

        void setLockHash(unsigned int hash)
        {
            lockHash = hash;
        }
        [[nodiscard]] auto build() const -> Item * override
        {
            auto *rect     = new gui::HBox(nullptr,
                                       style::window::default_left_margin,
                                       0,
                                       style::window::default_body_width,
                                       style::window::label::big_h);
            auto font      = FontManager::getInstance().getFont(style::window::font::medium);
            auto font_bold = FontManager::getInstance().getFont(style::window::font::mediumbold);

            rect->setEdges(RectangleEdge::None);
            rect->setReverseOrder(true);

            auto text = new TextFixedSize(nullptr, style::option::text_left_padding, 0, 0, 0);
            text->setMaximumSize(rect->getWidth(), rect->getHeight());
            text->setUnderline(false);
            text->setEditMode(EditMode::Browse);
            text->setFont(font_bold);
            text->setText("Change PIN");
            text->setAlignment(Alignment(Alignment::Vertical::Center));
            text->activeItem = false;
            text->setPenWidth(0);

            auto pin_text = new PinChange();

            pin_text->setMargins(Margins(0, 0, 0, 0));
            pin_text->setMinimumSize(60, rect->getHeight());
            pin_text->setMaximumSize(200, rect->getHeight());
            pin_text->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            pin_text->setEditMode(EditMode::Edit);
            pin_text->setFont(font);
            pin_text->setText(std::to_string(lockHash));
            pin_text->setPenWidth(0);
            pin_text->setInputMode(new InputMode(
                {InputMode::digit}, [=](const UTF8 &text) {}, [=]() {}, [=]() {}));
            pin_text->setEdges(RectangleEdge::All);

            pin_text->activatedCallback = [pin_text, this](gui::Item &item) {
                auto text     = pin_text->getText();
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

            rect->addWidget(pin_text);
            rect->addWidget(text);
            return rect;
        }
    };
} // namespace gui::option

std::list<gui::Option> settingsChangeWindow(app::Application *app, app::PinLockSetter *setter, unsigned int lockHash)
{
    std::list<gui::Option> list;
    list.emplace_back(std::make_unique<gui::option::ChangePin>(app, setter, lockHash));
    return list;
}
