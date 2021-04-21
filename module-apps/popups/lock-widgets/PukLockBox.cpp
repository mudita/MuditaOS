// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PukLockBox.hpp"

#include "popups/data/LockStyle.hpp"
#include "popups/lock-windows/PinLockBaseWindow.hpp"
#include "Lock.hpp"
#include "gui/widgets/Image.hpp"
#include <i18n/i18n.hpp>
#include <Style.hpp>

namespace label_style = style::window::pin_lock::pin_label;
namespace gui
{
    void PukLockBox::popChar(unsigned int charNum)
    {
        rebuildPinLabels(charNum);
    }
    void PukLockBox::putChar(unsigned int charNum)
    {
        rebuildPinLabels(++charNum);
    }
    void PukLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        buildPinLabels(0);
    }
    void PukLockBox::buildPinLabels(unsigned int pinSize)
    {
        auto itemBuilder = []() {
            auto label = new gui::Image("dot_12px_hard_alpha_W_G");
            return label;
        };

        LockWindow->buildPinLabels(itemBuilder, pinSize, label_style::x, label_style::y, label_style::w);
        LockWindow->pinLabelsBox->setEdges(RectangleEdge::Bottom);
    }

    void PukLockBox::rebuildPinLabels(unsigned int pinSize)
    {
        LockWindow->pinLabelsBox->erase();
        buildPinLabels(pinSize);
    }

    void PukLockBox::setVisibleStateEnterPin(EnterPasscodeType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);
        switch (type) {
        case PinLockBox::EnterPasscodeType::ProvidePasscode: {
            LockWindow->setText("app_desktop_sim_setup_enter_puk", PinLockBaseWindow::TextType::Primary, true);
            break;
        }
        case PinLockBox::EnterPasscodeType::ProvideNewPasscode: {
            LockWindow->setText("app_desktop_sim_enter_new_pin", PinLockBaseWindow::TextType::Primary);
            break;
        }
        case PinLockBox::EnterPasscodeType::ConfirmNewPasscode:
            LockWindow->setText("app_desktop_sim_confirm_new_pin", PinLockBaseWindow::TextType::Primary);
            break;
        }

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(false, false, true);
    }
    void PukLockBox::setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value)
    {
        switch (type) {
        case PinLockBox::PasscodeErrorType::InvalidPasscode:
            if (value > 1) {
                LockWindow->setText(
                    "app_desktop_sim_setup_wrong_puk",
                    PinLockBaseWindow::TextType::Primary,
                    true,
                    {{LockWindow->getToken(PinLockBaseWindow::Token::Attempts), static_cast<int>(value)}});
            }
            else {
                LockWindow->setText(
                    "app_desktop_sim_setup_wrong_puk_last_attempt", PinLockBaseWindow::TextType::Primary, true);
                LockWindow->setText("app_desktop_sim_setup_wrong_puk_last_attempt_warning",
                                    PinLockBaseWindow::TextType::Secondary,
                                    true);
            }
            break;
        case PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed: {
            LockWindow->setText("app_desktop_sim_wrong_pin_confirmation", PinLockBaseWindow::TextType::Primary);
            break;
        }
        case PinLockBox::PasscodeErrorType::UnhandledError:
            LOG_ERROR("No use case for UnhandledError in PukLockBox");
            break;
        }
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, false);
    }
    void PukLockBox::setVisibleStateBlocked()
    {
        LockWindow->setText("app_desktop_sim_puk_blocked", PinLockBaseWindow::TextType::Primary, true);
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, false, false);
    }

    void PukLockBox::clear()
    {
        rebuildPinLabels(0);
    }
} // namespace gui
