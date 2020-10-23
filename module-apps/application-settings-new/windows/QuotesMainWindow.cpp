// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesMainWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"

#include <InputEvent.hpp>
#include <CheckBoxWithLabel.hpp>
#include <i18/i18.hpp>
#include <json/json11.hpp>
#include <vfs.hpp>

namespace gui
{
    namespace quotes
    {
        inline static std::string path = "sys/data/applications/settings/quotes.json";
    } // namespace quotes

    QuotesMainWindow::QuotesMainWindow(app::Application *app) : BaseSettingsWindow(app, gui::window::name::quotes)
    {
        buildInterface();
    }

    void QuotesMainWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_locked_screen_quotes"));
        buildQuotesList(readQuotes(quotes::path));

        auto leftArrowImage = new gui::Image(this,
                                             style::settings::window::leftArrowImage::x,
                                             style::settings::window::leftArrowImage::y,
                                             style::settings::window::leftArrowImage::w,
                                             style::settings::window::leftArrowImage::h,
                                             "arrow_left");
        auto newQuoteImage  = new gui::Image(this,
                                            style::settings::window::crossImage::x,
                                            style::settings::window::crossImage::y,
                                            style::settings::window::crossImage::w,
                                            style::settings::window::crossImage::h,
                                            "cross");

        addWidget(leftArrowImage);
        addWidget(newQuoteImage);
    }

    void QuotesMainWindow::buildQuotesList(std::list<std::string> quotes)
    {
        auto body =
            new VBox(nullptr,
                     0,
                     title->offset_h() + style::margins::big,
                     this->getWidth(),
                     this->getHeight() - style::margins::big - this->title->offset_h() - bottomBar->getHeight());
        body->setEdges(gui::RectangleEdge::None);

        auto toggleBoxes = [this](CheckBoxWithLabel &active) {};

        for (auto quote : quotes) {
            boxes.push_back(new CheckBoxWithLabel(body, 0, 0, quote, toggleBoxes));
        }

        addWidget(body);
        setFocusItem(body);
    }

    bool QuotesMainWindow::onInput(const InputEvent &inputEvent)
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.state == InputEvent::State::keyReleasedShort) {
            switch (inputEvent.keyCode) {
            case gui::KeyCode::KEY_LEFT:
                application->switchWindow(gui::window::name::new_quote, nullptr);
                return true;
            default:
                break;
            }
        }
        return false;
    }

    std::list<std::string> QuotesMainWindow::readQuotes(fs::path fn)
    {
        std::list<std::string> quotes;
        std::string err;

        std::string fileContents = vfs.loadFileAsString(fn);

        auto obj = json11::Json::parse(fileContents, err).array_items();

        if (!err.empty()) {
            LOG_ERROR("Error while parsing quotes: %s", err.c_str());
        }

        std::transform(
            obj.begin(), obj.end(), std::back_inserter(quotes), [](auto item) { return item["quote"].string_value(); });

        return quotes;

    } // namespace gui
} // namespace gui
