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

    ScreenLockBox::PinLabel::PinLabel(Item *parent, uint32_t w, uint32_t h) : HBox(parent, 0, 0, w, h)
    {}
    void ScreenLockBox::PinLabel::setVisibleState(bool isImageVisible)
    {
        if (isImageVisible && image == nullptr) {
            image = new gui::Image("dot_12px_hard_alpha_W_G");
            image->setVisible(true);
            image->activeItem = false;
            addWidget(image);
        }
        else if (!isImageVisible && image != nullptr) {
            erase(image);
            image = nullptr;
        }
    }

    void ScreenLockBox::popChar(unsigned int charNum)
    {
        if (charNum < pinLabels.size()) {
            pinLabels[charNum]->setVisibleState(false);
        }
    }
    void ScreenLockBox::putChar(unsigned int charNum)
    {
        if (charNum < pinLabels.size()) {
            pinLabels[charNum]->setVisibleState(true);
        }
    }
    void ScreenLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        LockWindow->buildInfoText(lock_style::info_text_h_screen);
        ScreenLockBox::buildPinLabels(pinSize);
    }
    void ScreenLockBox::buildPinLabels(unsigned int pinSize)
    {
        constexpr auto pinLabelWidth  = style::window::default_body_width;
        pinLabels.clear();

        if (pinSize == 0) {
            return;
        }

        unsigned int singleLabelWidth        = lock_style::label_size;
        unsigned int maxNoMarginsLabelsWidth = pinLabelWidth - pinSize * 2 * lock_style::label_margins;

        if (pinSize * singleLabelWidth > maxNoMarginsLabelsWidth) {
            singleLabelWidth = maxNoMarginsLabelsWidth / pinSize;
        }

        auto itemBuilder = [this, singleLabelWidth]() {
            auto label = new PinLabel(nullptr, singleLabelWidth, lock_style::label_size);
            label->setEdges(RectangleEdge::Bottom);
            label->setMargins(Margins(lock_style::label_margins, 0, lock_style::label_margins, 0));
            pinLabels.push_back(label);
            return label;
        };

        LockWindow->buildPinLabels(
            itemBuilder, pinSize, style::window::default_left_margin, lock_style::pin_label_y_screen, pinLabelWidth);
        LockWindow->pinLabelsBox->setEdges(RectangleEdge::None);
    }
    void ScreenLockBox::setVisibleStateEnterPin(EnterPasscodeType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_screen_enter_passcode"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(true, false, true);
    }
    void ScreenLockBox::setVisibleStateVerifiedPin()
    {
        clear();
        LockWindow->pinLabelsBox->setVisible(false);
        LockWindow->titleLabel->setVisible(false);
    }
    void ScreenLockBox::setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value)
    {
        clear();
        LockWindow->pinLabelsBox->setVisible(false);
        LockWindow->infoText->clear();
        LockWindow->titleLabel->setVisible(true);
        switch (type) {
        case PinLockBox::PasscodeErrorType::InvalidPasscode:
            LockWindow->titleLabel->setText(utils::localize.get("app_desktop_screen_wrong_pin"));

            LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_sim_you_have")));
            LockWindow->infoText->addText(" ");
            LockWindow->infoText->addText(std::to_string(value));
            LockWindow->infoText->addText(" ");
            LockWindow->infoText->addText(
                utils::localize.get(utils::localize.get("app_desktop_sim_attempt_left_plural")));
            break;
        case PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed:
            LockWindow->titleLabel->setText(utils::localize.get("app_desktop_screen_wrong_passcode"));
            break;
        case PinLockBox::PasscodeErrorType::UnhandledError:
            LOG_ERROR("No use case for UnhandledError");
            break;
        }

        LockWindow->infoText->setVisible(true);
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, true);
    }
    void ScreenLockBox::setVisibleStateBlocked()
    {
        LockWindow->pinLabelsBox->setVisible(false);
        LockWindow->titleLabel->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_screen_blocked_info")));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, false);
    }

    void ScreenLockBox::clear()
    {
        for (unsigned i = 0; i < pinLabels.size(); i++) {
            popChar(i);
        }
    }
} // namespace gui
