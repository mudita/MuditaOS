// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLockBox.hpp"

#include "PinLockBaseWindow.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include <i18n/i18n.hpp>

namespace label_style = style::window::pin_lock::pin_label;

namespace gui
{

    constexpr auto timeToUnlock = 10;

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
        ScreenLockBox::buildPinLabels(pinSize);
    }
    void ScreenLockBox::buildPinLabels(unsigned int pinSize)
    {
        constexpr auto pinLabelWidth = style::window::default_body_width;
        pinLabels.clear();

        if (pinSize == 0) {
            return;
        }

        unsigned int singleLabelWidth        = label_style::size;
        unsigned int maxNoMarginsLabelsWidth = pinLabelWidth - pinSize * 2 * label_style::margin;

        if (pinSize * singleLabelWidth > maxNoMarginsLabelsWidth) {
            singleLabelWidth = maxNoMarginsLabelsWidth / pinSize;
        }

        auto itemBuilder = [this, singleLabelWidth]() {
            auto label = new PinLabel(nullptr, singleLabelWidth, label_style::size);
            label->setEdges(RectangleEdge::Bottom);
            label->setMargins(Margins(label_style::margin, 0, label_style::margin, 0));
            pinLabels.push_back(label);
            return label;
        };

        LockWindow->buildPinLabels(
            itemBuilder, pinSize, style::window::default_left_margin, label_style::y, pinLabelWidth);
        LockWindow->pinLabelsBox->setEdges(RectangleEdge::None);
    }
    void ScreenLockBox::setVisibleStateEnterPin(EnterPasscodeType type)
    {
        LockWindow->pinLabelsBox->setVisible(true);
        LockWindow->setText("app_desktop_screen_enter_passcode_to_unlock", PinLockBaseWindow::TextType::Primary, true);
        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(true, false, true);
    }

    void ScreenLockBox::setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value)
    {
        switch (type) {
        case PinLockBox::PasscodeErrorType::InvalidPasscode:
            LockWindow->setTitleBar(false, false);
            if (value == 1) {
                LockWindow->setText(
                    "app_desktop_screen_wrong_passcode_last_attempt", PinLockBaseWindow::TextType::Primary, true);
                LockWindow->setText("app_desktop_screen_wrong_passcode_last_attempt_warning",
                                    PinLockBaseWindow::TextType::Secondary,
                                    true,
                                    {{LockWindow->getToken(PinLockBaseWindow::Token::Mins), timeToUnlock}});
            }
            else {
                LockWindow->setText(
                    "app_desktop_screen_wrong_passcode",
                    PinLockBaseWindow::TextType::Primary,
                    true,
                    {{LockWindow->getToken(PinLockBaseWindow::Token::Attempts), static_cast<int>(value)}});
            }
            break;

        case PinLockBox::PasscodeErrorType::NewPasscodeConfirmFailed:
            LOG_ERROR("No use case for NewPasscodeConfirmFailed");
            break;
        case PinLockBox::PasscodeErrorType::UnhandledError:
            LOG_ERROR("No use case for UnhandledError");
            break;
        }
        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(false, true, true);
    }
    void ScreenLockBox::setVisibleStateBlocked()
    {
        LockWindow->setText("app_desktop_screen_blocked_info", PinLockBaseWindow::TextType::Primary);
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
