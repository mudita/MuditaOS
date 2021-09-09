// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ConnectionFrequencyWindow.hpp"

#include <application-settings/ApplicationSettings.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{
    ConnectionFrequencyWindow::ConnectionFrequencyWindow(app::ApplicationCommon *app,
                                                         app::settingsInterface::ConnectionSettings *connectionSettings)
        : BaseSettingsWindow(app, gui::window::name::connection_frequency), connectionSettings(connectionSettings)
    {
        setTitle(utils::translate("app_settings_title_connection_frequency"));
    }

    void ConnectionFrequencyWindow::onBeforeShow(ShowMode m, SwitchData *d)
    {
        refreshOptionsList(
            std::distance(frequency.begin(),
                          std::find(frequency.begin(), frequency.end(), connectionSettings->getConnectionFrequency())));
    }
    auto ConnectionFrequencyWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optList;

        auto intervalText = [](uint8_t value) {
            if (value == 0) {
                return utils::translate("app_alarm_clock_repeat_never");
            }
            const std::string toReplace = "%0";
            std::string temp            = utils::translate("app_meditation_interval_every_x_minutes");
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(value));
            return temp;
        };

        for (auto freq : frequency) {
            optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                intervalText(freq),
                [=](gui::Item &item) {
                    updateInterval(freq);
                    return true;
                },
                nullptr,
                nullptr,
                (freq == connectionSettings->getConnectionFrequency()) ? option::SettingRightItem::Checked
                                                                       : option::SettingRightItem::Disabled));
        }

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        return optList;
    }
    void ConnectionFrequencyWindow::rebuild()
    {
        clearOptions();
        addOptions(buildOptionsList());
    }
    void ConnectionFrequencyWindow::updateInterval(uint8_t value)
    {
        connectionSettings->setConnectionFrequency(value);
        refreshOptionsList();
    }

} // namespace gui
