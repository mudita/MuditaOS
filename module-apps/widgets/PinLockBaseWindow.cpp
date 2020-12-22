// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PinLockBaseWindow.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include "PinLock.hpp"
#include <i18n/i18n.hpp>

#include "FontManager.hpp"

namespace lock_style = style::window::pin_lock;
namespace gui
{

    void PinLockBaseWindow::build()
    {
        buildBottomBar();
        buildTopBar();
        buildTitleBar();
        buildInfoTexts();
    }
    void PinLockBaseWindow::buildBottomBar()
    {
        setBottomBarWidgetsActive(false, false, false);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_desktop_emergency"));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::confirm));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
    }
    void PinLockBaseWindow::buildTopBar()
    {
        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);
        topBar->setActive(TopBar::Elements::LOCK, true);
    }
    void PinLockBaseWindow::buildTitleBar()
    {
        using namespace style::window::pin_lock;

        iceBox = new gui::HBox(this, ice::x, ice::y, ice::w, ice::h);
        iceBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        iceBox->setEdges(RectangleEdge::None);
        iceBox->setVisible(false);

        auto arrow        = new gui::Image("left_label_arrow");
        arrow->activeItem = false;
        arrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        arrow->setMargins(Margins(0, 0, ice::margin, 0));
        iceBox->addWidget(arrow);

        auto iceText        = new gui::Text(nullptr, 0, 0, ice::text::w, ice::h);
        iceText->activeItem = false;
        iceText->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        iceText->setFont(style::window::font::verysmall);
        iceText->setText(utils::localize.get("app_desktop_emergency"));
        iceBox->addWidget(iceText);

        title = new gui::Text(this, title::x, title::y, title::w, title::h);
        title->setFilled(false);
        title->setBorderColor(gui::ColorFullBlack);
        title->setFont(style::header::font::title);
        title->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        title->setVisible(false);
        title->setPenWidth(2);
    }
    void PinLockBaseWindow::buildInfoTexts()
    {
        using namespace style::window::pin_lock;

        primaryText = new Text(this, primary_text::x, primary_text::y, primary_text::w, primary_text::h);
        primaryText->setFont(style::window::font::medium);
        primaryText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        secondaryText = new Text(this, secondary_text::x, secondary_text::y, secondary_text::w, secondary_text::h);
        secondaryText->setFont(style::window::font::medium);
        secondaryText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
    }
    void PinLockBaseWindow::buildPinLabels(std::function<Rect *()> itemBuilder,
                                           unsigned int pinSize,
                                           unsigned int offsetX,
                                           unsigned int offsetY,
                                           unsigned int boxWidth)
    {
        pinLabelsBox = new gui::HBox(this, offsetX, offsetY, boxWidth, lock_style::pin_label::size);
        pinLabelsBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

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
    void PinLockBaseWindow::buildImages(const std::string &lockImg, const std::string &infoImg)
    {
        lockImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, lockImg);
        infoImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, infoImg);
    }
    void PinLockBaseWindow::setBottomBarWidgetsActive(bool left, bool center, bool right)
    {
        bottomBar->setActive(BottomBar::Side::LEFT, left);
        bottomBar->setActive(BottomBar::Side::CENTER, center);
        bottomBar->setActive(BottomBar::Side::RIGHT, right);
    }
    void PinLockBaseWindow::setImagesVisible(bool lockImg, bool infoImg)
    {
        lockImage->setVisible(lockImg);
        infoImage->setVisible(infoImg);
    }

    void PinLockBaseWindow::setTitleBar(bool isVisible, bool isIceActive)
    {
        iceBox->setVisible(isIceActive);
        title->setVisible(isVisible);
        if (isVisible) {
            title->setEdges(RectangleEdge::Bottom);
        }
        else {
            title->clear();
            title->setEdges(RectangleEdge::None);
        }
    }

    void PinLockBaseWindow::setText(const std::string &value,
                                    TextType type,
                                    bool isReach,
                                    text::RichTextParser::TokenMap tokens)
    {
        auto text = getText(type);
        text->setVisible(true);
        if (isReach) {
            TextFormat format(FontManager::getInstance().getFont(style::window::font::medium));
            text::RichTextParser rtParser;
            auto parsedText = rtParser.parse(utils::localize.get(value), &format, std::move(tokens));
            text->setText(std::move(parsedText));
        }
        else {
            text->setText(utils::localize.get(value));
        }
    }

    auto PinLockBaseWindow::getText(TextType type) noexcept -> gui::Text *
    {
        if (type == TextType::Title) {
            return title;
        }
        else if (type == TextType::Primary) {
            return primaryText;
        }
        return secondaryText;
    }

    void PinLockBaseWindow::restore() noexcept
    {
        primaryText->setVisible(false);
        secondaryText->setVisible(false);
        lockImage->setVisible(false);
        infoImage->setVisible(false);
        pinLabelsBox->setVisible(false);
    }

    auto PinLockBaseWindow::getToken(Token token) const -> std::string
    {
        if (token == Token::Sim) {
            return "$SIM";
        }
        else if (token == Token::Attempts) {
            return "$ATTEMPTS";
        }
        else if (token == Token::Mins) {
            return "$MINUTES";
        }
        else if (token == Token::CmeCode) {
            return "$CMECODE";
        }
        return std::string{};
    }
} // namespace gui
