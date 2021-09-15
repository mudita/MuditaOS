// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IconTextWindow.hpp"
#include "MeditationSwitchData.hpp"

#include "log.hpp"
#include <i18n/i18n.hpp>
#include <Font.hpp>
#include <TextBlock.hpp>
#include <gui/core/ImageManager.hpp>
#include <gui/core/FontManager.hpp>

namespace gui
{
    IconTextWindow::IconTextWindow(app::Application *app, std::string name) : WithTimerWindow(app, std::move(name))
    {
        buildInterface();
        disableInput();
    }

    void IconTextWindow::buildInterface()
    {
        WithTimerWindow::buildInterface();

        appImage = new gui::Image(this, 0, 0);

        text = new gui::Text(this, itStyle::text::x, itStyle::text::y, itStyle::text::w, itStyle::text::h);
        text->setFont(style::window::font::small);
        text->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
    }

    void IconTextWindow::destroyInterface()
    {
        erase();
        appImage = nullptr;
        text     = nullptr;
    }

    void IconTextWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WithTimerWindow::onBeforeShow(mode, data);

        if (mode == ShowMode::GUI_SHOW_INIT) {
            showImage();
            showText();

            std::chrono::seconds secs = getTimeout();
            if (secs > std::chrono::seconds::zero()) {
                startTimer(secs);
            }
        }
    }

    void IconTextWindow::showImage()
    {
        uint32_t x, y;
        auto name = getImageName();

        if (name == itStyle::icon::imageSource) {
            x = itStyle::icon::x;
            y = itStyle::icon::y;
        }
        else {
            const auto id = ImageManager::getInstance().getImageMapID(name);
            auto imageMap = ImageManager::getInstance().getImageMap(id);
            if (imageMap != nullptr) {
                LOG_INFO("Image size -> w: %d, h: %d", imageMap->getWidth(), imageMap->getHeight());
                x = itStyle::icon::x + (itStyle::icon::w - imageMap->getWidth()) / 2;
                y = itStyle::icon::y + (itStyle::icon::h - imageMap->getHeight()) / 2;
            }
            else {
                x = itStyle::icon::x;
                y = itStyle::icon::y;
            }
        }
        appImage->set(name);
        appImage->setPosition(x, y);
    }

    void IconTextWindow::showText()
    {
        TextFormat format(FontManager::getInstance().getFont(itStyle::text::font));
        gui::text::RichTextParser parser;
        auto textParsed = parser.parse(getText(), &format);

        text->setText(std::move(textParsed));
    }
} // namespace gui
