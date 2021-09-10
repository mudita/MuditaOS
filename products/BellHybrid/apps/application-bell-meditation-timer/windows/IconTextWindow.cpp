// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IconTextWindow.hpp"
#include "../data/MeditationSwitchData.hpp"

#include "log.hpp"
#include <i18n/i18n.hpp>
#include <Font.hpp>
#include <TextBlock.hpp>

namespace gui
{
    IconTextWindow::IconTextWindow(app::Application *app, std::string name) : WithTimerWindow(app, name)
    {
        buildInterface();
        disableInput();
    }

    void IconTextWindow::buildInterface()
    {
        WithTimerWindow::buildInterface();

        appImage = new gui::Image(this, itStyle::icon::x, itStyle::icon::y, 0, 0, itStyle::icon::imageSource);

        text = new gui::Text(this, itStyle::text::x, itStyle::text::y, itStyle::text::w, itStyle::text::h);
        text->setFont(style::window::font::small);
        text->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
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
            showText();
            startTimer(getTimeout());
        }
    }

    void IconTextWindow::showText()
    {
        std::vector<std::string> content = getText();
        for (uint32_t i = 0; i < content.size(); i++) {
            if (i < (content.size() - 1)) {
                text->addText(TextBlock(content[i], Font(27).raw(), TextBlock::End::Newline));
            }
            else {
                text->addText(TextBlock(content[i], Font(27).raw(), TextBlock::End::None));
            }
        }
    }
} // namespace gui
