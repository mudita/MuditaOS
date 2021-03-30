// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "OptionsWidgetMaker.hpp"

namespace gui
{
    ///  @brief AlarmClock vibration and volume settings
    ///
    class AlarmClockWindow : public BaseSettingsWindow
    {
      public:
        ///  Constructor
        ///  @app parent application pointer
        explicit AlarmClockWindow(app::Application *app);

      private:
        ///  Switches alarm vibration state
        void switchVibrationState();

        ///  Sets the volume
        ///  @param vol volume value
        void setVolume(uint8_t vol);

        ///  Build the list of options to be drawn
        ///  @return list of options
        std::list<Option> buildOptionsList() override;

        ///  Widget maker object
        OptionsWidgetMaker mWidgetMaker;

        ///  Flag describing if phone vibration is enabled
        bool mVibrationsEnabled = false;
    };
} // namespace gui
