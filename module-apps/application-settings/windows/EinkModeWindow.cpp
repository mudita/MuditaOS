// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <memory>
#include <functional>

#include "messages/EinkModeMessage.hpp"
#include "service-appmgr/Controller.hpp"

#include "../ApplicationSettings.hpp"

#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "EinkModeWindow.hpp"
#include <Style.hpp>

namespace gui
{

    EinkModeWindow::EinkModeWindow(app::Application *app) : AppWindow(app, "EinkModeWindow")
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle("Change eink mode");
        auto label = new Label(this, 100,200,300,50,"Change mode on click");
        label->activatedCallback = [this](Item &) -> bool {
            static auto last_mode = seink::EinkModeMessage::Mode::Normal;
            if ( last_mode == seink::EinkModeMessage::Mode::Normal) 
            {
                last_mode = seink::EinkModeMessage::Mode::Invert;
            } else 
            {
                last_mode = seink::EinkModeMessage::Mode::Normal;
            }

            sys::Bus::SendUnicast(std::make_shared<seink::EinkModeMessage>(last_mode),"ServiceEink",this->application, 5000);
            return true;
        };
        setFocusItem(label);
    }

} /* namespace gui */
