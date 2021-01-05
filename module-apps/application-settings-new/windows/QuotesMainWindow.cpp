// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesMainWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"
#include "windows/OptionSetting.hpp"

#include <InputEvent.hpp>
#include <i18n/i18n.hpp>
#include <json/json11.hpp>
#include <purefs/filesystem_paths.hpp>
#include <Utils.hpp>
#include <string>

namespace gui
{
    QuotesMainWindow::QuotesMainWindow(app::Application *app) : BaseSettingsWindow(app, gui::window::name::quotes)
    {
        const auto quotesPath = purefs::dir::getCurrentOSPath() / "data/applications/settings/quotes.json";
        setTitle(utils::localize.get("app_settings_display_locked_screen_quotes"));
        readQuotes(quotesPath);
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

    void QuotesMainWindow::readQuotes(std::filesystem::path fn)
    {
        std::string err;

        const auto fileContents = readFileToString(fn);
        auto obj                = json11::Json::parse(fileContents.c_str(), err).array_items();

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

    std::string QuotesMainWindow::readFileToString(const std::filesystem::path &fn)
    {
        constexpr auto tar_buf = 8192 * 4;
        auto file              = std::fopen(fn.c_str(), "r");
        if (!file) {
            return {};
        }
        const auto length = utils::filesystem::filelength(file);
        if (length >= tar_buf) {
            LOG_ERROR("File %s length is too high!", fn.c_str());
            std::fclose(file);
            return {};
        }
        auto buffer = std::make_unique<char[]>(length + 1);
        std::fread(buffer.get(), 1, length, file);
        std::fclose(file);
        return std::string(buffer.get());
    }

} // namespace gui
