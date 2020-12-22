// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLockBoxDesktop.hpp"
#include "widgets/PinLock.hpp"

namespace gui
{
    void ScreenLockBoxDesktop::buildLockBox(unsigned int pinSize)
    {
        lockWindow->buildImages("pin_lock", "pin_lock_info");
        ScreenLockBox::buildPinLabels(pinSize);
    }
    void ScreenLockBoxDesktop::setVisibleStateEnterPin(EnterPasscodeType type)
    {
        lockWindow->pinLabelsBox->setVisible(true);
        lockWindow->setText("app_desktop_screen_enter_passcode_to_unlock", PinLockBaseWindow::TextType::Primary, true);
        lockWindow->setImagesVisible(true, false);
        lockWindow->setBottomBarWidgetsActive(true, false, true);
    }

    void ScreenLockBoxDesktop::setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value)
    {
        switch (type) {
        case PinLockBox::PasscodeErrorType::InvalidPasscode:
            lockWindow->setTitleBar(false, false);
            if (value == 1) {
                constexpr auto timeToUnlock = 10;
                lockWindow->setText(
                    "app_desktop_screen_wrong_passcode_last_attempt", PinLockBaseWindow::TextType::Primary, true);
                lockWindow->setText("app_desktop_screen_wrong_passcode_last_attempt_warning",
                                    PinLockBaseWindow::TextType::Secondary,
                                    true,
                                    {{lockWindow->getToken(PinLockBaseWindow::Token::Mins), timeToUnlock}});
            }
            else {
                lockWindow->setText(
                    "app_desktop_screen_wrong_passcode",
                    PinLockBaseWindow::TextType::Primary,
                    true,
                    {{lockWindow->getToken(PinLockBaseWindow::Token::Attempts), static_cast<int>(value)}});
            }
            break;

        case PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed:
            LOG_ERROR("No use case for NewPasscodeConfirmFailed");
            break;
        case PinLockBox::PasscodeErrorType::UnhandledError:
            LOG_ERROR("No use case for UnhandledError");
            break;
        }
        lockWindow->setImagesVisible(false, true);
        lockWindow->setBottomBarWidgetsActive(false, true, true);
    }
    void ScreenLockBoxDesktop::setVisibleStateBlocked()
    {
        lockWindow->setText("app_desktop_screen_blocked_info", PinLockBaseWindow::TextType::Primary);
        lockWindow->setImagesVisible(false, true);
        lockWindow->setBottomBarWidgetsActive(false, true, false);
    }
} // namespace gui
