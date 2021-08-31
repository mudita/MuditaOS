// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LockInputWindow.hpp"

#include <locks/data/LockStyle.hpp>
#include <locks/widgets/Lock.hpp>

#include <service-appmgr/Controller.hpp>
#include <header/IceAction.hpp>
#include <FontManager.hpp>
#include <i18n/i18n.hpp>

namespace lock_style = style::window::lock_input;

namespace gui
{
    void LockInputWindow::build()
    {
        buildBody();
        buildImage();
        buildInfoTexts();
        buildPinBody();
        buildIceBox();
        buildBottomBar();

        body->resizeItems();
    }

    void LockInputWindow::buildBody()
    {
        body = new VBox(this,
                        style::window::default_left_margin,
                        style::window::default_vertical_pos,
                        style::window::default_body_width,
                        style::window::default_body_height);
        body->setEdges(RectangleEdge::None);
    }

    void LockInputWindow::buildImage()
    {
        using namespace style::window::lock_input;

        infoImage = new gui::ImageBox(body, 0, 0, 0, 0, new gui::Image(""));
        infoImage->setMinimumSize(image::wh, image::wh);
        infoImage->setMargins(Margins(0, image::image_top_margin, 0, image::image_bottom_margin));
        infoImage->showImage(false);
    }

    void LockInputWindow::buildInfoTexts()
    {
        using namespace style::window::lock_input;

        primaryText = new Text(body, 0, 0, 0, 0);
        primaryText->setMinimumSize(style::window::default_body_width, primary_text::h);
        primaryText->setFont(style::window::font::medium);
        primaryText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        secondaryText = new Text(body, 0, 0, 0, secondary_text::h);
        secondaryText->setMinimumSize(style::window::default_body_width, secondary_text::h);
        secondaryText->setMargins(Margins(0, secondary_text::top_margin, 0, 0));
        secondaryText->setFont(style::window::font::medium);
        secondaryText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
    }

    void LockInputWindow::buildBottomBar()
    {
        bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::skip));
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::confirm));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
        setBottomBarWidgetsActive(false, false, false);
    }

    void LockInputWindow::buildIceBox()
    {
        header->navigationIndicatorAdd(new gui::header::IceAction(), gui::header::BoxSelection::Left);
    }

    void LockInputWindow::buildPinBody()
    {
        pinLabelsBox = new gui::HBox(body, 0, 0, 0, 0);
        pinLabelsBox->setMinimumSize(lock_style::input_box::w, lock_style::input_box::h);
        pinLabelsBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        pinLabelsBox->setMargins(Margins(0, lock_style::input_box::top_margin, 0, 0));
        pinLabelsBox->setEdges(RectangleEdge::None);
    }

    void LockInputWindow::buildPinLabels(const std::function<Rect *()> &itemBuilder, unsigned int pinSize)
    {
        if (pinSize == 0) {
            return;
        }

        for (uint32_t i = 0; i < pinSize; i++) {
            auto label = itemBuilder();
            label->setFilled(false);
            label->setBorderColor(gui::ColorFullBlack);
            label->setPenWidth(2);
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            label->setVisible(true);
            label->activeItem = false;
            pinLabelsBox->addWidget(label);
        }
    }

    status_bar::Configuration LockInputWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(status_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.enable(status_bar::Indicator::Time);
        appConfiguration.enable(status_bar::Indicator::PhoneMode);
        appConfiguration.enable(status_bar::Indicator::Battery);
        appConfiguration.enable(status_bar::Indicator::Signal);
        appConfiguration.enable(status_bar::Indicator::SimCard);
        appConfiguration.enable(status_bar::Indicator::Bluetooth);
        return appConfiguration;
    }

    void LockInputWindow::restore() noexcept
    {
        infoImage->showImage(false);
        primaryText->setVisible(false);
        secondaryText->setVisible(false);
        pinLabelsBox->setVisible(false);
    }

    void LockInputWindow::setImage(const UTF8 &imageName)
    {
        infoImage->setImage(imageName);
        infoImage->showImage(true);
    }

    void LockInputWindow::setBottomBarWidgetsActive(bool left, bool center, bool right)
    {
        bottomBar->setActive(BottomBar::Side::LEFT, left);
        bottomBar->setActive(BottomBar::Side::CENTER, center);
        bottomBar->setActive(BottomBar::Side::RIGHT, right);
    }

    void LockInputWindow::setText(const std::string &value, TextType type, text::RichTextParser::TokenMap tokens)
    {
        switch (type) {
        case TextType::Title: {
            header->setTitleVisibility(true);
            if (!tokens.empty()) {
                TextFormat format(FontManager::getInstance().getFont(style::window::font::medium));
                setTitle(text::RichTextParser().parse(utils::translate(value), &format, std::move(tokens))->getText());
            }
            else {
                setTitle(utils::translate(value));
            }
            break;
        }
        case TextType::Primary:
            primaryText->setVisible(true);
            primaryText->setRichText(utils::translate(value), std::move(tokens));
            break;
        case TextType::Secondary:
            secondaryText->setVisible(true);
            secondaryText->setRichText(utils::translate(value), std::move(tokens));
            break;
        }
    }

    void LockInputWindow::setTitleBar(bool titleVisible, bool iceVisible)
    {
        header->setTitleVisibility(titleVisible);

        if (iceVisible) {
            header->navigationIndicatorAdd(new gui::header::IceAction(), gui::header::BoxSelection::Left);
        }
        else {
            header->navigationIndicatorRemove(gui::header::BoxSelection::Left);
        }
    }

    auto LockInputWindow::getToken(LockInputWindow::Token token) const -> std::string
    {
        if (token == Token::PinType) {
            return "$PINTYPE";
        }
        else if (token == Token::Sim) {
            return "$SIM";
        }
        else if (token == Token::Attempts) {
            return "$ATTEMPTS";
        }
        else if (token == Token::Time) {
            return "$TIME";
        }
        else if (token == Token::CmeCode) {
            return "$CMECODE";
        }
        return std::string{};
    }

    void LockInputWindow::setVisibleState()
    {
        restore();
        switch (lock->getState()) {
        case locks::Lock::LockState::InputRequired:
            lockBox->setVisibleStateInputRequired(LockBox::InputActionType::ProvideInput);
            break;
        case locks::Lock::LockState::InputInvalid:
            lockBox->setVisibleStateInputInvalid(LockBox::InputErrorType::InvalidInput, lock->getAttemptsLeft());
            break;
        case locks::Lock::LockState::Blocked:
            lockBox->setVisibleStateBlocked(lock->getNextUnlockAttemptFormattedTime());
            break;
        case locks::Lock::LockState::NewInputRequired:
            lockBox->setVisibleStateInputRequired(LockBox::InputActionType::ProvideNewInput);
            break;
        case locks::Lock::LockState::NewInputConfirmRequired:
            lockBox->setVisibleStateInputRequired(LockBox::InputActionType::ConfirmNewInput);
            break;
        case locks::Lock::LockState::NewInputInvalid:
            lockBox->setVisibleStateInputInvalid(LockBox::InputErrorType::NewInputConfirmFailed,
                                                 lock->getAttemptsLeft());
            break;
        default:
            break;
        }
    }

    auto LockInputWindow::isIceVisible() const noexcept -> bool
    {
        return header->navigationIndicatorVisible(header::BoxSelection::Left);
    }

    auto LockInputWindow::isInInputState() const noexcept -> bool
    {
        return lock && (lock->isState(locks::Lock::LockState::InputRequired) ||
                        lock->isState(locks::Lock::LockState::NewInputRequired) ||
                        lock->isState(locks::Lock::LockState::NewInputConfirmRequired));
    }

    auto LockInputWindow::isInInvalidInputState() const noexcept -> bool
    {
        return lock && (lock->isState(locks::Lock::LockState::InputInvalid) ||
                        lock->isState(locks::Lock::LockState::NewInputInvalid));
    }

} // namespace gui
