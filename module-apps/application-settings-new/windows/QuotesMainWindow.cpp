// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesMainWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"
#include "windows/OptionSetting.hpp"

#include <InputEvent.hpp>
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
        setTitle(utils::localize.get("app_settings_display_locked_screen_quotes"));
        readQuotes(quotes::path);
    }

    auto QuotesMainWindow::onInput(const InputEvent &inputEvent) -> bool
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

    void QuotesMainWindow::readQuotes(fs::path fn)
    {
        std::string err;

        std::string fileContents = utils::filesystem::loadFileAsString(fn);

        auto obj = json11::Json::parse(fileContents, err).array_items();

        if (!err.empty()) {
            LOG_ERROR("Error while parsing quotes: %s", err.c_str());
        }

        std::transform(obj.begin(), obj.end(), std::back_inserter(quotes), [](auto item) {
            return std::pair{item["quote"].string_value(), false};
        });
    }

    auto QuotesMainWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        for (auto &quote : quotes) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                utils::translateI18(quote.first),
                [&quote, this](gui::Item &item) {
                    switchHandler(quote.second);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::Switch),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                quote.second ? RightItem::Checked : RightItem::Disabled));
        }

        return optionsList;
    }

    void QuotesMainWindow::switchHandler(bool &optionSwitch)
    {
        optionSwitch = !optionSwitch;
        rebuildOptionList();
    }
} // namespace gui
