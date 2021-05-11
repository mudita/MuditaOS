// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "locks/data/LockStyle.hpp"
#include "locks/widgets/Lock.hpp"
#include "FontManager.hpp"
#include <i18n/i18n.hpp>
#include "LockWindow.hpp"

namespace lock_style = style::window::pin_lock;

namespace gui
{
    void LockWindow::build()
    {
        buildBottomBar();
        buildTitleBar();
        buildInfoTexts();
    }

    void LockWindow::buildInfoTexts()
    {
        using namespace style::window::pin_lock;

        primaryText = new Text(this, primary_text::x, primary_text::y, primary_text::w, primary_text::h);
        primaryText->setFont(style::window::font::medium);
        primaryText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        secondaryText = new Text(this, secondary_text::x, secondary_text::y, secondary_text::w, secondary_text::h);
        secondaryText->setFont(style::window::font::medium);
        secondaryText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
    }

    void LockWindow::buildPinLabels(const std::function<Rect *()> &itemBuilder,
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

    void LockWindow::restore() noexcept
    {
        primaryText->setVisible(false);
        secondaryText->setVisible(false);
        pinLabelsBox->setVisible(false);
    }

    void LockWindow::setBottomBarWidgetsActive(bool left, bool center, bool right)
    {
        bottomBar->setActive(BottomBar::Side::LEFT, left);
        bottomBar->setActive(BottomBar::Side::CENTER, center);
        bottomBar->setActive(BottomBar::Side::RIGHT, right);
    }

    void LockWindow::setText(const std::string &value, TextType type, text::RichTextParser::TokenMap tokens)
    {
        switch (type) {
        case TextType::Title: {
            title->setVisible(true);
            if (!tokens.empty()) {
                TextFormat format(FontManager::getInstance().getFont(style::window::font::medium));
                title->setText(
                    text::RichTextParser().parse(utils::translate(value), &format, std::move(tokens))->getText());
            }
            else {
                title->setText(utils::translate(value));
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

    void LockWindow::setTitleBar(bool isVisible)
    {
        title->setVisible(isVisible);
    }

    void LockWindow::buildBottomBar()
    {
        bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::skip));
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::confirm));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
    }

} // namespace gui
