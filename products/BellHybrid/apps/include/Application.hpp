// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>
#include <common/models/AlarmModel.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/PreWakeUpModel.hpp>

namespace app
{
    class Application : public ApplicationCommon
    {
      public:
        using ApplicationCommon::ApplicationCommon;
        void resumeIdleTimer();
        void suspendIdleTimer();
        explicit Application(std::string name,
                             std::string parent                  = "",
                             StatusIndicators statusIndicators   = StatusIndicators{},
                             StartInBackground startInBackground = {false},
                             std::uint32_t stackDepth            = 1024 * 4,
                             sys::ServicePriority priority       = sys::ServicePriority::Idle);

        sys::ReturnCodes InitHandler() override;

      protected:
        void attachPopups(const std::vector<gui::popup::ID> &popupsList) override;
        std::optional<gui::popup::Blueprint> popupBlueprintFallback(gui::popup::ID id) override;
        void startIdleTimer();
        void restartIdleTimer();
        void stopIdleTimer();
        void stopAllAudio();
        virtual void onStop();

        std::unique_ptr<app::AlarmModel> alarmModel;
        std::unique_ptr<AbstractBatteryModel> batteryModel;
        std::unique_ptr<AbstractPreWakeUpModel> preWakeUpModel;

      private:
        sys::MessagePointer handleKBDKeyEvent(sys::Message *msgl) override;
        sys::MessagePointer handleApplicationSwitch(sys::Message *msgl) override;
        sys::MessagePointer handleAppClose(sys::Message *msgl) override;
        sys::MessagePointer handleAppFocusLost(sys::Message *msgl) override;
        void updateStatuses(gui::AppWindow *window) const override;
        sys::MessagePointer handlePreWakeUpChangeState(sys::Message *msgl);
        bool isInputEventToHandlePreWakeUp(const gui::InputEvent inputEvent);

        virtual void onKeyPressed();
        virtual void onStart();
        bool idleTimerActiveFlag = true;
    };
} // namespace app
