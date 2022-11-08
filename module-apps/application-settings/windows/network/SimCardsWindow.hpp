// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>
#include <application-settings/ApplicationSettings.hpp>

namespace app::settingsInterface
{
    class SimParams;
}; // namespace app::settingsInterface

namespace gui
{
    class SimCardsWindow : public BaseSettingsWindow
    {
      private:
        auto buildOptionsList() -> std::list<Option> override;
        app::settingsInterface::SimParams *simParams;

        std::list<gui::Option> createListWithNoConnectedSim(const Store::GSM::SelectedSIM sim,
                                                            const std::string &simStr);
        std::list<gui::Option> createListWithBlockedSim(const Store::GSM::SelectedSIM sim, const std::string &simStr);
        std::list<gui::Option> createListWithActiveSim(const Store::GSM::SelectedSIM sim,
                                                       const std::string &simStr,
                                                       const std::string &phoneNumber);
        bool createSwitchSimAction(const Store::GSM::SelectedSIM selectedSim) const;
        bool createNavBarText(const gui::Item &item);

      public:
        SimCardsWindow(app::ApplicationCommon *app, app::settingsInterface::SimParams *simParams);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        OptionWindowDestroyer rai_destroyer = OptionWindowDestroyer(*this);
    };
} // namespace gui
