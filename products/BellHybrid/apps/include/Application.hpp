// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>

namespace app
{
    class Application : public ApplicationCommon
    {
      public:
        using ApplicationCommon::ApplicationCommon;

      protected:
        void attachPopups(const std::vector<gui::popup::ID> &popupsList) override;
        void idleTimerRestartIfActive();
        void idleTimerRestart();
        void idleTimerStop();

      private:
        virtual sys::MessagePointer handleKBDKeyEvent(sys::Message *msgl) override;
        virtual sys::MessagePointer handleApplicationSwitch(sys::Message *msgl) override;
        virtual sys::MessagePointer handleAppClose(sys::Message *msgl) override;
        virtual sys::MessagePointer handleAppFocusLost(sys::Message *msgl) override;

        virtual void idleTimerKeyPressedAction();
        virtual void idleTimerApplicationStartAction();
        virtual void idleTimerApplicationStopAction();
    };
} // namespace app
