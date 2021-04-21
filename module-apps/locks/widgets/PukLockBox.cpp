// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PukLockBox.hpp"

#include "locks/data/LockStyle.hpp"
#include "locks/windows/PinLockBaseWindow.hpp"
#include "Lock.hpp"
#include "gui/widgets/Image.hpp"
#include <i18n/i18n.hpp>
#include <Style.hpp>

namespace label_style = style::window::pin_lock::pin_label;
namespace gui
{
    void PukLockBox::popChar(unsigned int charNum)
    {
        rebuildInputLabels(charNum);
    }
    void PukLockBox::putChar(unsigned int charNum)
    {
        rebuildInputLabels(++charNum);
    }
    void PukLockBox::buildLockBox(unsigned int inputSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        buildInputLabels(0);
    }
    void PukLockBox::buildInputLabels(unsigned int inputSize)
    {
        auto itemBuilder = []() {
            auto label = new gui::Image("dot_12px_hard_alpha_W_G");
            return label;
        };

        LockWindow->buildPinLabels(itemBuilder, inputSize, label_style::x, label_style::y, label_style::w);
        LockWindow->pinLabelsBox->setEdges(RectangleEdge::Bottom);
    }

    void PukLockBox::rebuildInputLabels(unsigned int inputSize)
    {
        LockWindow->pinLabelsBox->erase();
        buildInputLabels(inputSize);
    }

    void PukLockBox::setVisibleStateInputRequired(InputActionType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);
        switch (type) {
        case LockBox::InputActionType::ProvideInput: {
            LockWindow->setText("app_desktop_sim_setup_enter_puk", PinLockBaseWindow::TextType::Primary, true);
            break;
        }
        case LockBox::InputActionType::ProvideNewInput: {
            LockWindow->setText("app_desktop_sim_enter_new_pin", PinLockBaseWindow::TextType::Primary);
            break;
        }
        case LockBox::InputActionType::ConfirmNewInput:
            LockWindow->setText("app_desktop_sim_confirm_new_pin", PinLockBaseWindow::TextType::Primary);
            break;
        }

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(false, false, true);
    }
    void PukLockBox::setVisibleStateInputInvalid(InputErrorType type, unsigned int value)
    {
        switch (type) {
        case LockBox::InputErrorType::InvalidInput:
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
        case LockBox::InputErrorType::NewInputConfirmFailed: {
            LockWindow->setText("app_desktop_sim_wrong_pin_confirmation", PinLockBaseWindow::TextType::Primary);
            break;
        }
        case LockBox::InputErrorType::UnhandledError:
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
        rebuildInputLabels(0);
    }
} // namespace gui
