// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PukLockBox.hpp"

#include "PinLockBaseWindow.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include <i18/i18.hpp>
#include <Style.hpp>

namespace lock_style = style::window::pin_lock;

namespace gui
{
    void PukLockBox::popChar(uint32_t charNum)
    {
        rebuildPinLabels(charNum);
    }
    void PukLockBox::putChar(uint32_t charNum)
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
        // labels with stars for displaying entered digits
        const uint32_t pinLabelBoxWidth = style::window_width - 2 * lock_style::pin_label_x;

        LockWindow->pinLabelBox = new gui::HBox(
            LockWindow, lock_style::pin_label_x, lock_style::pin_label_y, pinLabelBoxWidth, lock_style::label_size);

        LockWindow->pinLabelBox->setAlignment(
            Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        LockWindow->pinLabelBox->setEdges(RectangleEdge::Bottom);

        for (uint32_t i = 0; i < pinSize; i++) {

            auto label = new gui::Image("dot_12px_hard_alpha_W_G");
            label->setFilled(false);
            label->setBorderColor(gui::ColorFullBlack);

            label->setPenWidth(2);
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
            label->setVisible(true);
            LockWindow->pinLabelBox->addWidget(label);
            label->activeItem = false;
        }
    }

    void PukLockBox::rebuildPinLabels(unsigned int pinSize)
    {
        LockWindow->pinLabelBox->erase();
        buildPinLabels(pinSize);
    }

    void PukLockBox::setVisibleStateEnterPin()
    {
        LockWindow->pinLabelBox->setVisible(true);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_blocked"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_type_puk"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(false, false, true);
    }
    void PukLockBox::setVisibleStateVerifiedPin()
    {
        LockWindow->pinLabelBox->setVisible(false);
        LockWindow->infoText->setVisible(false);
    }
    void PukLockBox::setVisibleStateInvalidPin()
    {
        LockWindow->pinLabelBox->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_wrong_puk"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_you_have"));
        LockWindow->infoText->addText(" ");
        LockWindow->infoText->addText(std::to_string(LockWindow->lock.getRemainingAttempts()));
        LockWindow->infoText->addText(" ");

        if (LockWindow->lock.getRemainingAttempts() > 1) {
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
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, true, false);
    }
    void PukLockBox::setVisibleStateBlocked()
    {
        LockWindow->pinLabelBox->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_blocked_info1"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_blocked_info2"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, false, false);
    }

} // namespace gui
