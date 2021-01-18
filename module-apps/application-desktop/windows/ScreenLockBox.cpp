// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-desktop/widgets/PinLock.hpp"
#include "ScreenLockBox.hpp"

namespace gui
{
    constexpr auto timeToUnlock = 10;

    void ScreenLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        ScreenLockBaseBox::buildLockBox(pinSize);
    }

    void ScreenLockBox::setVisibleStateBlocked()
    {
        LockWindow->setText("app_desktop_screen_blocked_info", LockWindow::TextType::Primary);
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, false);
    }

    void ScreenLockBox::setVisibleStateEnterPin(EnterPasscodeType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);
        LockWindow->setText("app_desktop_screen_enter_passcode_to_unlock", LockWindow::TextType::Primary, true);
        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(true, false, true);
    }

    void ScreenLockBox::setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value)
    {
        switch (type) {
        case PinLockBox::PasscodeErrorType::InvalidPasscode:
            LockWindow->setTitleBar(false, false);
            if (value == 1) {
                LockWindow->setText(
                    "app_desktop_screen_wrong_passcode_last_attempt", LockWindow::TextType::Primary, true);
                LockWindow->setText("app_desktop_screen_wrong_passcode_last_attempt_warning",
                                    LockWindow::TextType::Secondary,
                                    true,
                                    {{LockWindow->getToken(LockWindow::Token::Mins), timeToUnlock}});
            }
            else {
                LockWindow->setText("app_desktop_screen_wrong_passcode",
                                    LockWindow::TextType::Primary,
                                    true,
                                    {{LockWindow->getToken(LockWindow::Token::Attempts), static_cast<int>(value)}});
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
} // namespace gui
