#include "SettingsChange.hpp"
#include "../ApplicationSettings.hpp"
#include "i18/i18.hpp"
#include "tools/Common.hpp"
#include <widgets/Text.hpp>
#include <widgets/TextFixedSize.hpp>
#include <FontManager.hpp>
#include <service-db/api/DBServiceAPI.hpp>

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
        app::Application *app = nullptr;

      public:
        ChangePin(app::Application *app) : app(app)
        {}

        [[nodiscard]] auto build() const -> Item * override
        {
            auto *rect     = new gui::HBox(nullptr,
                                       style::window::default_left_margin,
                                       0,
                                       style::window::default_body_width,
                                       style::window::label::big_h);
            auto font      = FontManager::getInstance().getFont(style::window::font::medium);
            auto font_bold = FontManager::getInstance().getFont(style::window::font::mediumbold);
            auto settings  = app->getSettings();

            rect->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
            rect->setReverseOrder(true);

            auto text = new TextFixedSize(nullptr, style::option::text_left_padding, 0, 0, 0);
            text->setMaximumSize(rect->getWidth(), rect->getHeight());
            text->setUnderline(false);
            text->setEditMode(EditMode::BROWSE);
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
            pin_text->setEditMode(EditMode::EDIT);
            pin_text->setFont(font);
            pin_text->setText(std::to_string(settings.lockPassHash));
            pin_text->setPenWidth(0);
            pin_text->setInputMode(new InputMode(
                {InputMode::digit}, [=](const UTF8 &text) {}, [=]() {}, [=]() {}));
            pin_text->setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
            auto lapp = app;

            pin_text->activatedCallback = [pin_text, lapp](gui::Item &item) {
                auto settings = lapp->getSettings();
                auto text     = pin_text->getText();
                if (text.length() == 0) {
                    LOG_DEBUG("remove pin");
                    settings.lockPassHash = 0;
                    DBServiceAPI::SettingsUpdate(lapp, settings);
                }
                else if (text.length() != 4) {
                    pin_text->setText("bad value - needs 4 digits");
                }
                else {
                    auto value = std::stoi(text);
                    LOG_DEBUG("setting pin to: %d", value);
                    settings.lockPassHash = value;
                    DBServiceAPI::SettingsUpdate(lapp, settings);
                }
                return true;
            };

            rect->addWidget(pin_text);
            rect->addWidget(text);
            return rect;
        }
    };
} // namespace gui::option

std::list<gui::Option> settingsChangeWindow(app::Application *app)
{
    std::list<gui::Option> list;
    list.emplace_back(std::make_unique<gui::option::ChangePin>(app));
    return list;
}
