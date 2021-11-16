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
        bool isPopupPermitted(gui::popup::ID popupId) const override;
        void startIdleTimer();
        void restartIdleTimer();
        void stopIdleTimer();
        void stopAllAudio();

      private:
        sys::MessagePointer handleKBDKeyEvent(sys::Message *msgl) override;
        sys::MessagePointer handleApplicationSwitch(sys::Message *msgl) override;
        sys::MessagePointer handleAppClose(sys::Message *msgl) override;
        sys::MessagePointer handleAppFocusLost(sys::Message *msgl) override;

        virtual void onKeyPressed();
        virtual void onStart();
        virtual void onStop();
    };
} // namespace app
