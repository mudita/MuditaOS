// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmClockWindow.hpp"

#include <application-settings/widgets/SpinBoxOptionSettings.hpp>
#include <application-settings/windows/WindowNames.hpp>

namespace gui
{
    AlarmClockWindow::AlarmClockWindow(app::Application *app,
                                       std::unique_ptr<audio_settings::AbstractAudioSettingsModel> &&audioModel)
        : BaseSettingsWindow(app, gui::window::name::alarm_clock), mWidgetMaker(this),
          mAudioModel(std::move(audioModel))
    {
        setTitle(utils::translate("app_settings_apps_alarm_clock"));
    }

    std::list<Option> AlarmClockWindow::buildOptionsList()
    {
        std::list<gui::Option> optionList;
        mVibrationsEnabled = mAudioModel->isVibrationEnabled();
        mWidgetMaker.addSwitchOption(optionList, utils::translate("app_settings_vibration"), mVibrationsEnabled, [&]() {
            switchVibrationState();
        });

        auto focusCallback = [&](gui::Item &item) {
            if (item.focus) {
                setBottomBarText(UTF8(), BottomBar::Side::CENTER);
            }
            return true;
        };

        optionList.emplace_back(std::make_unique<gui::SpinBoxOptionSettings>(
            utils::translate("app_settings_volume"),
            mAudioModel->getVolume(),
            std::ceil(10.0),
            [&](uint8_t value) {
                setVolume(value);
                return true;
            },
            focusCallback));

        return optionList;
    }

    void AlarmClockWindow::switchVibrationState()
    {
        (mVibrationsEnabled) ? mAudioModel->setVibrationDisabled() : mAudioModel->setVibrationEnabled();
        refreshOptionsList();
    }

    void AlarmClockWindow::setVolume(uint8_t vol)
    {
        LOG_INFO("setVolume %d", static_cast<int>(vol));
        mAudioModel->setVolume(vol);
    }

} // namespace gui
