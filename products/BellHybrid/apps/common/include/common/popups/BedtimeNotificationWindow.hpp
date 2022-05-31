// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/WindowWithTimer.hpp>
#include <AsyncTask.hpp>

namespace app
{
    class ApplicationCommon;
}
namespace gui
{
    class Icon;

    class BedtimeNotificationWindow : public WindowWithTimer, public app::AsyncCallbackReceiver
    {
      private:
        static constexpr auto bedtimeNotificationIcon   = "big_bedtime_W_G";
        static constexpr auto bedtimeNotificationText   = "app_bell_bedtime_notification";
        static constexpr auto bedtimeNotificationTimout = std::chrono::seconds{6};

        app::ApplicationCommon *app;
        Icon *icon = nullptr;
        bool onInput(const InputEvent &inputEvent) override;
        void returnToPreviousWindow();
        void buildInterface() override;

      public:
        explicit BedtimeNotificationWindow(app::ApplicationCommon *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
} /* namespace gui */
