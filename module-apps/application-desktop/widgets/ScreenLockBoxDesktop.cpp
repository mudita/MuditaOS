// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLockBoxDesktop.hpp"
#include "widgets/PinLock.hpp"
#include "widgets/PinLockBaseWindow.hpp"

namespace gui
{
    void ScreenLockBoxDesktop::setVisibleStateEnterPin(EnterPasscodeType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);
        LockWindow->setText("app_desktop_screen_enter_passcode_to_unlock", PinLockBaseWindow::TextType::Primary, true);
        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(true, false, true);
    }

    void ScreenLockBoxDesktop::setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value)
    {
        switch (type) {
        case PinLockBox::PasscodeErrorType::InvalidPasscode:
            LockWindow->setTitleBar(false, false);
            if (value == 1) {
                constexpr auto timeToUnlock = 10;
                LockWindow->setText(
                    "app_desktop_screen_wrong_passcode_last_attempt", PinLockBaseWindow::TextType::Primary, true);
                LockWindow->setText("app_desktop_screen_wrong_passcode_last_attempt_warning",
                                    PinLockBaseWindow::TextType::Secondary,
                                    true,
                                    {{LockWindow->getToken(PinLockBaseWindow::Token::Mins), timeToUnlock}});
            }
            else {
                LockWindow->setText(
                    "app_desktop_screen_wrong_passcode",
                    PinLockBaseWindow::TextType::Primary,
                    true,
                    {{LockWindow->getToken(PinLockBaseWindow::Token::Attempts), static_cast<int>(value)}});
            }
            break;

        case PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed:
            LOG_ERROR("No use case for NewPasscodeConfirmFailed");
            break;
        case PinLockBox::PasscodeErrorType::UnhandledError:
            LOG_ERROR("No use case for UnhandledError");
            break;
        }
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, true);
    }
    void ScreenLockBoxDesktop::setVisibleStateBlocked()
    {
        LockWindow->setText("app_desktop_screen_blocked_info", PinLockBaseWindow::TextType::Primary);
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, false);
    }
} // namespace gui
