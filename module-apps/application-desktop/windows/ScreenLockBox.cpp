// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLockBox.hpp"

#include "PinLockBaseWindow.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include <i18/i18.hpp>

namespace lock_style = style::window::pin_lock;

namespace gui
{

    ScreenLockBox::PinLabel::PinLabel(Item *parent, const uint32_t &w, const uint32_t &h) : HBox(parent, 0, 0, w, h)
    {}
    void ScreenLockBox::PinLabel::setVisibleState(bool isImageVisible)
    {
        if (isImageVisible) {
            image = new gui::Image("dot_12px_hard_alpha_W_G");
            image->setVisible(true);
            image->activeItem = false;
            addWidget(image);
        }
        else {
            image->erase();
        }
    }

    void ScreenLockBox::popChar(uint32_t charNum)
    {
        pinLabels[charNum]->setVisibleState(false);
    }
    void ScreenLockBox::putChar(uint32_t charNum)
    {
        pinLabels[charNum]->setVisibleState(true);
    }
    void ScreenLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        LockWindow->buildInfoText(lock_style::info_text_h_screen);
        ScreenLockBox::buildPinLabels(pinSize);
    }
    void ScreenLockBox::buildPinLabels(unsigned int pinSize)
    {
        // labels with stars for displaying entered digits
        const uint32_t pinLabelBoxWidth = style::window::default_body_width; // - 2 * lock_style::pin_label_x;

        LockWindow->pinLabelBox = new gui::HBox(LockWindow,
                                                style::window::default_left_margin,
                                                lock_style::pin_label_y,
                                                pinLabelBoxWidth,
                                                lock_style::label_size_screen);

        LockWindow->pinLabelBox->setAlignment(
            Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        LockWindow->pinLabelBox->setEdges(RectangleEdge::All);

        unsigned int singleLabelWidth = lock_style::label_size_screen;
        if (pinSize == 0) {
            return;
        }
        if (pinSize * singleLabelWidth > LockWindow->pinLabelBox->getWidth() - pinSize * 2 * 10) {
            singleLabelWidth = (LockWindow->pinLabelBox->getWidth() - pinSize * 2 * 10) / pinSize;
        }

        for (uint32_t i = 0; i < pinSize; i++) {

            auto label = new PinLabel(nullptr, singleLabelWidth, lock_style::label_size);
            label->setFilled(false);
            label->setBorderColor(gui::ColorFullBlack);

            label->setPenWidth(2);
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            label->setVisible(true);
            pinLabels.push_back(label);
            label->activeItem = false;

            label->setEdges(RectangleEdge::All);
            label->setMargins(Margins(10, 0, 10, 0));

            LockWindow->pinLabelBox->addWidget(label);
            LockWindow->pinLabelBox->resizeItems();
        }
    }
    void ScreenLockBox::setVisibleStateEnterPin()
    {
        LockWindow->pinLabelBox->setVisible(true);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_screen_enter_passcode"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(true, false, true);
    }
    void ScreenLockBox::setVisibleStateVerifiedPin()
    {
        clearPinLabels();
        LockWindow->pinLabelBox->setVisible(false);
        LockWindow->titleLabel->setVisible(false);
    }
    void ScreenLockBox::setVisibleStateInvalidPin()
    {
        clearPinLabels();
        LockWindow->pinLabelBox->setVisible(false);

        LockWindow->titleLabel->setVisible(true);
        LockWindow->titleLabel->setText(utils::localize.get("app_desktop_screen_wrong_pin"));

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_screen_allowed_attempts")));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(std::to_string(LockWindow->lock.getRemainingAttempts()));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, true);
    }
    void ScreenLockBox::setVisibleStateBlocked()
    {
        LockWindow->pinLabelBox->setVisible(false);
        LockWindow->titleLabel->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_screen_blocked_info")));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, false);
    }

    void ScreenLockBox::clearPinLabels()
    {
        for (unsigned i = 0; i < pinLabels.size(); i++) {
            popChar(i);
        }
    }
} // namespace gui
