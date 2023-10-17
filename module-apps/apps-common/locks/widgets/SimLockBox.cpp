// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimLockBox.hpp"
#include "locks/windows/LockInputWindow.hpp"

namespace gui
{
    void SimLockBox::buildLockBox(unsigned int pinSize)
    {
        LockBoxAlternatingSize::buildLockBox(pinSize);
    }

    void SimLockBox::applyLockActionText(locks::SimInputTypeAction simInputTypeAction)
    {
        LockWindow->setText("sim_header_setup",
                            LockInputWindow::TextType::Title,
                            {{LockWindow->getToken(LockInputWindow::Token::Sim), LockWindow->lock->getLockName()}});
        LockWindow->setTitleBar(true, true);

        switch (simInputTypeAction) {
        case locks::SimInputTypeAction::UnlockWithPin:
            textForInputRequired                  = "sim_enter_pin_unlock";
            textForInvalidInput                   = "sim_setup_wrong_pin";
            textForInvalidInputLastAttempt        = "sim_setup_wrong_pin_last_attempt";
            textForInvalidInputLastAttemptWarning = "";
            break;
        case locks::SimInputTypeAction::UnlockWithPuk:
            textForInputRequired                  = "sim_setup_enter_puk";
            textForInvalidInput                   = "sim_setup_wrong_puk";
            textForInvalidInputLastAttempt        = "sim_setup_wrong_puk_last_attempt";
            textForInvalidInputLastAttemptWarning = "sim_setup_wrong_puk_last_attempt_warning";
            break;
        case locks::SimInputTypeAction::ChangePin:
        case locks::SimInputTypeAction::EnablePin:
        case locks::SimInputTypeAction::DisablePin:
            textForInputRequired                  = "sim_enter_enter_current";
            textForInvalidInput                   = "sim_wrong_pin_confirmation";
            textForInvalidInputLastAttempt        = "";
            textForInvalidInputLastAttemptWarning = "";
            break;
        default:
            break;
        }
    }

    void SimLockBox::setVisibleStateInputRequired(InputActionType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);
        switch (type) {
        case LockBox::InputActionType::ProvideInput: {
            LockWindow->setText(
                textForInputRequired,
                LockInputWindow::TextType::Primary,
                {{LockWindow->getToken(LockInputWindow::Token::PinType), LockWindow->lock->getLockName()}});
            break;
        }
        case LockBox::InputActionType::ProvideNewInput:
            LockWindow->setText("sim_enter_new_pin", LockInputWindow::TextType::Primary);
            break;
        case LockBox::InputActionType::ConfirmNewInput:
            LockWindow->setText("sim_confirm_new_pin", LockInputWindow::TextType::Primary);
            break;
        }

        LockWindow->setImage("sim_128px_W_G");
        LockWindow->setNavBarWidgetsActive(false, LockWindow->lock->canVerify(), true);
    }

    void SimLockBox::setVisibleStateInputInvalid(InputErrorType type, unsigned int value)
    {
        switch (type) {
        case LockBox::InputErrorType::InvalidInput:
            if (value == 1) {
                LockWindow->setText(textForInvalidInputLastAttempt, LockInputWindow::TextType::Primary);
                LockWindow->setText(textForInvalidInputLastAttemptWarning, LockInputWindow::TextType::Secondary);
            }
            else {
                LockWindow->setText(
                    textForInvalidInput,
                    LockInputWindow::TextType::Primary,
                    {{LockWindow->getToken(LockInputWindow::Token::Attempts), static_cast<int>(value)}});
            }
            break;
        case LockBox::InputErrorType::NewInputConfirmFailed:
            LockWindow->setText("sim_wrong_pin_confirmation", LockInputWindow::TextType::Primary);
            break;
        }
        LockWindow->setImage("info_128px_W_G");
        LockWindow->setNavBarWidgetsActive(false, true, true);
    }

    void SimLockBox::setVisibleStateBlocked(const std::string &formattedTime)
    {
        LockWindow->setText("sim_puk_blocked", LockInputWindow::TextType::Primary);
        LockWindow->setImage("sim_128px_W_G");
        LockWindow->setNavBarWidgetsActive(false, false, true);
    }

    void SimLockBox::setVisibleStateError(unsigned int errorCode)
    {
        LockWindow->setText("sim_cme_error",
                            LockInputWindow::TextType::Primary,
                            {{LockWindow->getToken(LockInputWindow::Token::CmeCode), static_cast<int>(errorCode)}});
        LockWindow->setImage("info_128px_W_G");
        LockWindow->setNavBarWidgetsActive(false, false, true);
    }
} // namespace gui
