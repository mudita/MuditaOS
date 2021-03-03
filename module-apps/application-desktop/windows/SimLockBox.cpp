// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimLockBox.hpp"

#include "PinLockBaseWindow.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include <Style.hpp>
#include <Image.hpp>

namespace label_style = style::window::pin_lock::pin_label;

namespace gui
{

    void SimLockBox::popChar(unsigned int charNum)
    {
        rebuildPinLabels(charNum);
    }
    void SimLockBox::putChar(unsigned int charNum)
    {
        rebuildPinLabels(++charNum);
    }

    void SimLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        buildPinLabels(0);
    }
    void SimLockBox::buildPinLabels(unsigned int pinSize)
    {
        auto itemBuilder = []() {
            auto label = new gui::Image("dot_12px_hard_alpha_W_G");
            return label;
        };

        LockWindow->buildPinLabels(itemBuilder, pinSize, label_style::x, label_style::y, label_style::w);
        LockWindow->pinLabelsBox->setEdges(RectangleEdge::Bottom);
    }

    void SimLockBox::rebuildPinLabels(unsigned int pinSize)
    {
        LockWindow->pinLabelsBox->erase();
        buildPinLabels(pinSize);
    }

    void SimLockBox::setVisibleStateEnterPin(EnterPasscodeType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);
        switch (type) {
        case PinLockBox::EnterPasscodeType::ProvidePasscode: {
            LockWindow->setText(
                "app_desktop_sim_enter_pin_unlock",
                PinLockBaseWindow::TextType::Primary,
                true,
                {{LockWindow->getToken(PinLockBaseWindow::Token::PinType), LockWindow->lock->getPasscodeName()}});
            break;
        }
        case PinLockBox::EnterPasscodeType::ProvideNewPasscode:
            LockWindow->setText("app_desktop_sim_enter_new_pin", PinLockBaseWindow::TextType::Primary);
            break;
        case PinLockBox::EnterPasscodeType::ConfirmNewPasscode:
            LockWindow->setText("app_desktop_sim_confirm_new_pin", PinLockBaseWindow::TextType::Primary);
            break;
        }

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(false, false, true);
    }
    void SimLockBox::setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value)
    {
        switch (type) {
        case PinLockBox::PasscodeErrorType::InvalidPasscode:
            if (value == 1) {
                LockWindow->setText(
                    "app_desktop_sim_setup_wrong_pin_last_attempt", PinLockBaseWindow::TextType::Primary, true);
            }
            else {
                LockWindow->setText(
                    "app_desktop_sim_setup_wrong_pin",
                    PinLockBaseWindow::TextType::Primary,
                    true,
                    {{LockWindow->getToken(PinLockBaseWindow::Token::Attempts), static_cast<int>(value)}});
            }
            break;
        case PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed:
            LockWindow->setText("app_desktop_sim_wrong_pin_confirmation", PinLockBaseWindow::TextType::Primary);
            break;
        case PinLockBox::PasscodeErrorType::UnhandledError: {
            LockWindow->setText("app_desktop_sim_cme_error",
                                PinLockBaseWindow::TextType::Primary,
                                true,
                                {{LockWindow->getToken(PinLockBaseWindow::Token::CmeCode), static_cast<int>(value)}});
            break;
        }
        }
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, true);
    }
    void SimLockBox::setVisibleStateBlocked()
    {
        LockWindow->setText("app_desktop_sim_puk_blocked", PinLockBaseWindow::TextType::Primary);
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, true);
    }

    void SimLockBox::clear()
    {
        rebuildPinLabels(0);
    }
} // namespace gui
