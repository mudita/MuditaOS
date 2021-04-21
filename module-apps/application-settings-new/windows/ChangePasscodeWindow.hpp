// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/locks/windows/LockWindow.hpp"
#include "module-apps/locks/widgets/PhoneLockBaseBox.hpp"
#include "application-settings-new/data/ChangePasscodeData.hpp"
#include "application-settings-new/widgets/ChangePasscodeLockHandler.hpp"

namespace gui
{
    class ChangePasscodeWindow : public LockWindow
    {
      public:
        explicit ChangePasscodeWindow(app::Application *app);
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      protected:
        void buildBottomBar() override;
        void buildInterface() override;
        void buildTitleBar() override;
        void destroyInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        virtual void processPasscode();
        virtual void setVisibleState();

        ChangePasscodeAction changePasscodeAction;
        std::unique_ptr<PhoneLockBaseBox> lockBox  = nullptr;
        gui::Image *lockImage                      = nullptr;
        Lock::LockState lockState{Lock::LockState::InputRequired};
        ChangePasscodeLockHandler lockHandler;

        std::string textForEnterCurrentPassword = "app_settings_security_type_current_passcode";
        std::string textForEnterNewPassword     = "app_settings_security_enter_new_passcode";
        std::string testForConfirmPassword      = "app_settings_security_confirm_new_passcode";
        std::string textForWrongPassword        = "app_settings_security_wrong_passcode";
    };
} /* namespace gui */
