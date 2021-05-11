// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Lock.hpp"
#include "PhoneLockBox.hpp"

namespace gui
{
    constexpr auto timeToUnlock = 10;

    void PhoneLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        PhoneLockBaseBox::buildLockBox(pinSize);
    }

    top_bar::Configuration PhoneLockBox::configureTopBarLocked()
    {
        top_bar::Configuration appConfiguration;
        appConfiguration.disable(top_bar::Indicator::Time);
        appConfiguration.enable(top_bar::Indicator::Lock);

        return appConfiguration;
    }

    top_bar::Configuration PhoneLockBox::configureTopBarUnLocked()
    {
        top_bar::Configuration appConfiguration;
        appConfiguration.enable(top_bar::Indicator::Time);
        appConfiguration.disable(top_bar::Indicator::Lock);

        return appConfiguration;
    }

    void PhoneLockBox::applyLockActionText(locks::PhoneLockInputTypeAction phoneLockInputTypeAction)
    {
        switch (phoneLockInputTypeAction) {
        case locks::PhoneLockInputTypeAction::Unlock:
            LockWindow->setTitleBar(false, false);
            LockWindow->configureTopBar(configureTopBarLocked());
            textForInputRequired = "phone_lock_unlock";
            textForInvalidInput  = "phone_lock_unlock_invalid";
            leftBottomBarState   = false;
            break;
        case locks::PhoneLockInputTypeAction::Enable:
        case locks::PhoneLockInputTypeAction::Disable:
        case locks::PhoneLockInputTypeAction::ConfirmCurrent:
        case locks::PhoneLockInputTypeAction::Change:
            LockWindow->setTitleBar(true, false);
            LockWindow->setText("phone_lock_configure", LockWindow::TextType::Title);
            LockWindow->configureTopBar(configureTopBarUnLocked());

            textForInputRequired   = "phone_lock_current";
            textForInvalidInput    = "phone_lock_invalid";
            textForProvideNewInput = "phone_lock_enter_new";
            textForConfirmNewInput = "phone_lock_confirm_new";
            leftBottomBarState     = false;
            break;
        case locks::PhoneLockInputTypeAction::Set:
            LockWindow->setTitleBar(true, false);
            LockWindow->setText("phone_lock_configure", LockWindow::TextType::Title);
            LockWindow->configureTopBar(configureTopBarUnLocked());

            textForInputRequired   = "phone_lock_current";
            textForInvalidInput    = "phone_lock_invalid_retry";
            textForProvideNewInput = "phone_lock_set";
            textForConfirmNewInput = "phone_lock_confirm";
            leftBottomBarState     = true;
            break;
        default:
            break;
        }
    }

    void PhoneLockBox::setVisibleStateBlocked()
    {
        LockWindow->setText("phone_lock_blocked", LockWindow::TextType::Primary);
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, false);
    }

    void PhoneLockBox::setVisibleStateInputRequired(InputActionType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);

        switch (type) {
        case LockBox::InputActionType::ProvideInput: {
            LockWindow->setText(textForInputRequired, PinLockBaseWindow::TextType::Primary);
            break;
        }
        case LockBox::InputActionType::ProvideNewInput: {
            LockWindow->setText(textForProvideNewInput, PinLockBaseWindow::TextType::Primary);
            break;
        }
        case LockBox::InputActionType::ConfirmNewInput:
            LockWindow->setText(textForConfirmNewInput, PinLockBaseWindow::TextType::Primary);
            break;
        }

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(leftBottomBarState, false, true);
    }

    void PhoneLockBox::setVisibleStateInputInvalid(InputErrorType type, unsigned int value)
    {
        switch (type) {
        case LockBox::InputErrorType::InvalidInput:
            if (value == 1) {
                LockWindow->setText("phone_lock_unlock_last_attempt", LockWindow::TextType::Primary);
                LockWindow->setText("phone_lock_unlock_last_attempt_warning",
                                    LockWindow::TextType::Secondary,
                                    {{LockWindow->getToken(LockWindow::Token::Mins), timeToUnlock}});
            }
            else {
                LockWindow->setText(textForInvalidInput,
                                    LockWindow::TextType::Primary,
                                    {{LockWindow->getToken(LockWindow::Token::Attempts), static_cast<int>(value)}});
            }
            break;

        case LockBox::InputErrorType::NewInputConfirmFailed:
            LockWindow->setText(textForInvalidInput,
                                LockWindow::TextType::Primary,
                                {{LockWindow->getToken(LockWindow::Token::Attempts), static_cast<int>(value)}});
            break;
        case LockBox::InputErrorType::UnhandledError:
            LOG_ERROR("No use case for UnhandledError");
            break;
        }
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, true);
    }
} // namespace gui
