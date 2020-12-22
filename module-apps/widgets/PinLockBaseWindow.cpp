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
    void PinLockBaseWindow::invalidate() noexcept
    {
        title         = nullptr;
        primaryText   = nullptr;
        secondaryText = nullptr;
    }
    void PinLockBaseWindow::build()
    {
        buildBottomBar();
        buildTopBar();
        buildTitleBar();
        buildInfoTexts();
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
    void PinLockBaseWindow::setBottomBarWidgetsActive(bool left, bool center, bool right)
    {
        bottomBar->setActive(BottomBar::Side::LEFT, left);
        bottomBar->setActive(BottomBar::Side::CENTER, center);
        bottomBar->setActive(BottomBar::Side::RIGHT, right);
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
        pinLabelsBox->setVisible(false);
    }
} // namespace gui
