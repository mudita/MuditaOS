// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Lock.hpp"
#include "PhoneLockBox.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    void PhoneLockBox::buildLockBox(unsigned int pinSize)
    {
        LockBoxConstantSize::buildLockBox(pinSize);
    }

    void PhoneLockBox::applyLockActionText(locks::PhoneLockInputTypeAction phoneLockInputTypeAction)
    {
        switch (phoneLockInputTypeAction) {
        case locks::PhoneLockInputTypeAction::Unlock:
            LockWindow->setTitleBar(false, false);
            textForInputRequired = "phone_lock_unlock";
            textForInvalidInput  = "phone_lock_unlock_invalid";
            leftNavBarState      = false;
            break;
        case locks::PhoneLockInputTypeAction::Enable:
        case locks::PhoneLockInputTypeAction::Disable:
        case locks::PhoneLockInputTypeAction::ConfirmCurrent:
        case locks::PhoneLockInputTypeAction::Change:
            LockWindow->setTitleBar(true, false);
            LockWindow->setText("phone_lock_configure", LockInputWindow::TextType::Title);

            textForInputRequired   = "phone_lock_current";
            textForInvalidInput    = "phone_lock_invalid";
            textForProvideNewInput = "phone_lock_enter_new";
            textForConfirmNewInput = "phone_lock_confirm_new";
            leftNavBarState        = false;
            break;
        case locks::PhoneLockInputTypeAction::Set:
            LockWindow->setTitleBar(true, true);
            LockWindow->setText("phone_lock_configure", LockInputWindow::TextType::Title);

            textForInputRequired   = "phone_lock_current";
            textForInvalidInput    = "phone_lock_invalid_retry";
            textForProvideNewInput = "phone_lock_set";
            textForConfirmNewInput = "phone_lock_confirm";
            leftNavBarState        = true;
            break;
        default:
            break;
        }
    }

    void PhoneLockBox::setVisibleStateBlocked(const std::string &formattedTime)
    {
        LockWindow->setText("phone_lock_blocked_information",
                            LockInputWindow::TextType::Primary,
                            {{LockWindow->getToken(LockInputWindow::Token::Time), formattedTime}});
        LockWindow->setImage("error_W_G");
        LockWindow->setNavBarWidgetsActive(false, true, false);
        LockWindow->setNavBarText(utils::translate(style::strings::common::ok), nav_bar::Side::Center);
    }

    void PhoneLockBox::setVisibleStateError(unsigned int errorCode)
    {
        LOG_ERROR("No use case for UnhandledError");
    }

    void PhoneLockBox::setVisibleStateInputRequired(InputActionType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);

        switch (type) {
        case LockBox::InputActionType::ProvideInput: {
            LockWindow->setText(textForInputRequired, LockInputWindow::TextType::Primary);
            break;
        }
        case LockBox::InputActionType::ProvideNewInput: {
            LockWindow->setText(textForProvideNewInput, LockInputWindow::TextType::Primary);
            break;
        }
        case LockBox::InputActionType::ConfirmNewInput:
            LockWindow->setText(textForConfirmNewInput, LockInputWindow::TextType::Primary);
            break;
        }

        LockWindow->setImage("unlock_icon_W_G");
        LockWindow->setNavBarWidgetsActive(leftNavBarState, false, true);
    }

    void PhoneLockBox::setVisibleStateInputInvalid(InputErrorType type, unsigned int value)
    {
        switch (type) {
        case LockBox::InputErrorType::InvalidInput:

            LockWindow->setText(textForInvalidInput,
                                LockInputWindow::TextType::Primary,
                                {{LockWindow->getToken(LockInputWindow::Token::Attempts), static_cast<int>(value)}});

            break;

        case LockBox::InputErrorType::NewInputConfirmFailed:
            LockWindow->setText(textForInvalidInput,
                                LockInputWindow::TextType::Primary,
                                {{LockWindow->getToken(LockInputWindow::Token::Attempts), static_cast<int>(value)}});
            break;
        }
        LockWindow->setImage("info_icon_W_G");
        LockWindow->setNavBarWidgetsActive(false, true, false);
        LockWindow->setNavBarText(utils::translate(style::strings::common::ok), nav_bar::Side::Center);
    }
} // namespace gui
