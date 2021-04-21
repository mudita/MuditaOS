// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimLockBox.hpp"

#include "locks/data/LockStyle.hpp"
#include "locks/windows/PinLockBaseWindow.hpp"
#include "Lock.hpp"
#include <Style.hpp>
#include <Image.hpp>

namespace label_style = style::window::pin_lock::pin_label;

namespace gui
{

    void SimLockBox::popChar(unsigned int charNum)
    {
        rebuildInputLabels(charNum);
    }
    void SimLockBox::putChar(unsigned int charNum)
    {
        rebuildInputLabels(++charNum);
    }

    void SimLockBox::buildLockBox(unsigned int inputSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        buildInputLabels(0);
    }
    void SimLockBox::buildInputLabels(unsigned int inputSize)
    {
        auto itemBuilder = []() {
            auto label = new gui::Image("dot_12px_hard_alpha_W_G");
            return label;
        };

        LockWindow->buildPinLabels(itemBuilder, inputSize, label_style::x, label_style::y, label_style::w);
        LockWindow->pinLabelsBox->setEdges(RectangleEdge::Bottom);
    }

    void SimLockBox::rebuildInputLabels(unsigned int inputSize)
    {
        LockWindow->pinLabelsBox->erase();
        buildInputLabels(inputSize);
    }

    void SimLockBox::setVisibleStateInputRequired(InputActionType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);
        switch (type) {
        case LockBox::InputActionType::ProvideInput: {
            LockWindow->setText(
                "app_desktop_sim_enter_pin_unlock",
                PinLockBaseWindow::TextType::Primary,
                true,
                {{LockWindow->getToken(PinLockBaseWindow::Token::PinType), LockWindow->lock->getLockName()}});
            break;
        }
        case LockBox::InputActionType::ProvideNewInput:
            LockWindow->setText("app_desktop_sim_enter_new_pin", PinLockBaseWindow::TextType::Primary);
            break;
        case LockBox::InputActionType::ConfirmNewInput:
            LockWindow->setText("app_desktop_sim_confirm_new_pin", PinLockBaseWindow::TextType::Primary);
            break;
        }

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(false, false, true);
    }
    void SimLockBox::setVisibleStateInputInvalid(InputErrorType type, unsigned int value)
    {
        switch (type) {
        case LockBox::InputErrorType::InvalidInput:
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
        case LockBox::InputErrorType::NewInputConfirmFailed:
            LockWindow->setText("app_desktop_sim_wrong_pin_confirmation", PinLockBaseWindow::TextType::Primary);
            break;
        case LockBox::InputErrorType::UnhandledError: {
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
        rebuildInputLabels(0);
    }
} // namespace gui
