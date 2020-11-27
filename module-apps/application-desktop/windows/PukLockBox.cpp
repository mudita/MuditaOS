// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PukLockBox.hpp"

#include "PinLockBaseWindow.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include <module-utils/i18n/i18n.hpp>
#include <Style.hpp>

namespace lock_style = style::window::pin_lock;

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
        LockWindow->buildInfoText(lock_style::info_text_h_puk);
        buildPinLabels(0);
    }
    void PukLockBox::buildPinLabels(unsigned int pinSize)
    {
        auto itemBuilder = []() {
            auto label = new gui::Image("dot_12px_hard_alpha_W_G");
            return label;
        };

        LockWindow->buildPinLabels(itemBuilder,
                                   pinSize,
                                   lock_style::pin_label_x,
                                   lock_style::pin_label_y,
                                   style::window_width - 2 * lock_style::pin_label_x);
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
        LockWindow->infoText->clear();

        switch (type) {
        case PinLockBox::EnterPasscodeType::ProvidePasscode:
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_blocked"));
            LockWindow->infoText->addText("\n");
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_type_puk"));
            break;
        case PinLockBox::EnterPasscodeType::ProvideNewPasscode:
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_enter_pin"));
            break;
        case PinLockBox::EnterPasscodeType::ConfirmNewPasscode:
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_confirm_pin"));
        }
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(false, false, true);
    }
    void PukLockBox::setVisibleStateVerifiedPin()
    {
        LockWindow->pinLabelsBox->setVisible(false);
        LockWindow->infoText->setVisible(false);
    }
    void PukLockBox::setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value)
    {
        LockWindow->pinLabelsBox->setVisible(false);
        LockWindow->infoText->clear();

        switch (type) {
        case PinLockBox::PasscodeErrorType::InvalidPasscode:

            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_wrong_puk"));
            LockWindow->infoText->addText("\n");
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_you_have"));
            LockWindow->infoText->addText(" ");
            LockWindow->infoText->addText(std::to_string(value));
            LockWindow->infoText->addText(" ");

            if (value > 1) {
                LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_attempt_left_plural"));
            }
            else {
                LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_attempt_left_singular"));
                LockWindow->infoText->addText("\n\n");
                LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_warning1"));
                LockWindow->infoText->addText("\n");
                LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_warning2"));
                LockWindow->infoText->addText("\n");
                LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_warning3"));
            }
            break;
        case PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed:
            LockWindow->infoText->setText(utils::localize.get("app_desktop_sim_wrong_pin"));
            break;
        case PinLockBox::PasscodeErrorType::UnhandledError:
            LOG_ERROR("No use case for UnhandledError in PukLockBox");
            break;
        }

        LockWindow->infoText->setVisible(true);
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, true, false);
    }
    void PukLockBox::setVisibleStateBlocked()
    {
        LockWindow->pinLabelsBox->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_blocked_info1"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_blocked_info2"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, false, false);
    }

    void PukLockBox::clear()
    {
        rebuildPinLabels(0);
    }
} // namespace gui
